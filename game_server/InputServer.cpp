#include <stdio.h>
#include <string.h>
#include "InputServer.h"
#include <iostream>
#include <conio.h>

#include <map>

using namespace std;

static char * myctrlid = NULL;
static int ctrlsocket = -1;
static struct sockaddr_in ctrlsin;
// message queue
static int qhead, qtail, qsize, qunit;
static unsigned char * qbuffer = NULL;

static msgfunc replay = NULL;

#define	INVALID_KEY	0xffff
typedef WORD	KeySym;		// map to Windows Virtual Keycode
static int cxsize, cysize;	// for mapping mouse coordinates

static map<int, KeySym> keymap;
static KeySym SDLKeyToKeySym(int sdlkey);
static bool keymap_initialized = false;

#define	SDLK_KP0	SDLK_KP_0
#define	SDLK_KP1	SDLK_KP_1
#define	SDLK_KP2	SDLK_KP_2
#define	SDLK_KP3	SDLK_KP_3
#define	SDLK_KP4	SDLK_KP_4
#define	SDLK_KP5	SDLK_KP_5
#define	SDLK_KP6	SDLK_KP_6
#define	SDLK_KP7	SDLK_KP_7
#define	SDLK_KP8	SDLK_KP_8
#define	SDLK_KP9	SDLK_KP_9
//
#define SDLK_NUMLOCK	SDLK_NUMLOCKCLEAR
#define SDLK_SCROLLOCK	SDLK_SCROLLLOCK
#define SDLK_RMETA	SDLK_RGUI
#define SDLK_LMETA	SDLK_LGUI
//#define SDLK_LSUPER
//#define SDLK_RSUPER
//#define SDLK_COMPOSE
//#define SDLK_PRINT
#define SDLK_BREAK	SDLK_PRINTSCREEN

struct sdlmsg * sdlmsg_ntoh(struct sdlmsg * msg){
#if SDL_VERSION_ATLEAST(2,0,0)
	if(msg->scancode) msg->scancode = ntohs(msg->sdlkey);
	if(msg->sdlkey) msg->sdlkey = (int)ntohl(msg->sdlkey);
	if(msg->unicode) msg->unicode = ntohl(msg->unicode);
#else
	if(msg->sdlkey) msg->sdlkey = ntohs(msg->sdlkey);
	if(msg->unicode) msg->unicode = ntohs(msg->unicode);
#endif
	if(msg->sdlmod) msg->sdlmod = ntohs(msg->sdlmod);
	if(msg->mousex) msg->mousex = ntohs(msg->mousex);
	if(msg->mousey) msg->mousey = ntohs(msg->mousey);
	if(msg->mouseRelX) msg->mouseRelX = ntohs(msg->mouseRelX);
	if(msg->mouseRelY) msg->mouseRelY = ntohs(msg->mouseRelY);
	return msg;
}

struct sdlmsg * 
#if SDL_VERSION_ATLEAST(2,0,0)
	sdlmsg_keyboard(struct sdlmsg *msg, unsigned char pressed, unsigned short scancode, SDL_Keycode key, unsigned short mod, unsigned int unicode)
#else
	sdlmsg_keyboard(struct sdlmsg *msg, unsigned char pressed, unsigned char scancode, SDL_Keycode key, SDLMod mod, unsigned short unicode)
#endif
{
	bzero(msg, sizeof(struct sdlmsg));
	msg->msgsize = htons(sizeof(struct sdlmsg));
	msg->msgtype = SDL_EVENT_MSGTYPE_KEYBOARD;
	msg->is_pressed = pressed;
#if SDL_VERSION_ATLEAST(2,0,0)
	msg->scancode = htons(scancode);
	msg->sdlkey = htonl(key);
	msg->unicode = htonl(unicode);
#else
	msg->scancode = scancode;
	msg->sdlkey=  htons(key);
	msg->unicode = htons(unicode);
#endif
	msg->sdlmod = htons(mod);
	return msg;
}

struct sdlmsg * sdlmsg_mousekey(struct sdlmsg *msg, unsigned char pressed , unsigned char button){
	bzero(msg, sizeof(struct sdlmsg));
	msg->msgsize = htons(sizeof(struct sdlmsg));
	msg->msgtype = SDL_EVENT_MSGTYPE_MOUSEKEY;
	msg->is_pressed = pressed;
	msg->mousebutton = button;
	return msg;
}
#if SDL_VERSION_ATLEAST(2,0,0)
struct sdlmsg * sdlmsg_mousewheel(struct sdlmsg * msg, unsigned short mousex, unsigned short mousey){
	bzero(msg, sizeof(struct sdlmsg));
	msg->msgsize = htons(sizeof(struct sdlmsg));
	msg->msgtype = SDL_EVENT_MSGTYPE_MOUSEWHEEL;
	msg->mousex = htons(mousex);
	msg->mousey = htons(mousey);
	return msg;
}
#endif

struct sdlmsg * sdlmsg_mousemotion(struct sdlmsg * msg, unsigned short mousex, unsigned short mousey, unsigned short relx, unsigned short rely, unsigned char state,
	int relativeMouseMode){
		bzero(msg, sizeof(struct sdlmsg));
		msg->msgsize = htons(sizeof(struct sdlmsg));
		msg->msgtype = SDL_EVENT_MSGTYPE_MOUSEMOTION;
		msg->mousestate = state;
		msg->mousex = htons(mousex);
		msg->mousey = htons(mousey);
		msg->mouseRelX = htons(relx);
		msg->mouseRelY = htons(rely);
		msg->relativeMouseMode = (relativeMouseMode !=0 ) ? 1 : 0;
		return msg;
}

static unsigned long name_resolve(const char * hostname){}

// queue routines
int ctrl_queue_init(int size, int maxunit){
	qunit = maxunit + sizeof(struct queuemsg);
	qsize = size - (size % qunit);
	qhead = qtail =0;
	if((qbuffer = (unsigned char *)malloc(qsize))== NULL){
		return -1;
	}
	// warning message
	return 0;
}
struct queuemsg * ctrl_queue_read_msg(){
	struct queuemsg * msg;
	if(qtail == qhead){
		// queue is empty
		msg = NULL;
	}else{
		msg = (struct queuemsg* )(qbuffer + qhead);
	}
	return msg;
}

void ctrl_queue_release_msg(struct queuemsg *msg) {
	struct queuemsg *currmsg;

	if(qhead == qtail) {
		// queue is empty

		return;
	}
	currmsg = (struct queuemsg *) (qbuffer + qhead);
	if(msg != currmsg) {

	}
	qhead += qunit;
	if(qhead == qsize) {
		qhead = 0;
	}

	return;
}

int ctrl_queue_write_msg(void *msg, int msgsize) {
	int nextpos;
	struct queuemsg *qmsg;
	//
	if((msgsize + sizeof(struct queuemsg)) > qunit) {

		return 0;
	}

	//
	nextpos = qtail + qunit;
	if(nextpos == qsize) {
		nextpos = 0;
	}
	//
	if(nextpos == qhead) {
		// queue is full
		msgsize = 0;
	} else {
		qmsg = (struct queuemsg*) (qbuffer + qtail);
		qmsg->msgsize = msgsize;
		memcpy(msg, qmsg->msg, msgsize);
		qtail = nextpos;
	}

	//
	return msgsize;
}

void
	ctrl_queue_clear() {

		qhead = qtail = 0;

}
struct myRect winRect;
static struct myRect *prect = NULL;

//extern CommandServer cs;
extern bool F9Pressed;

extern CRITICAL_SECTION f9;

int serverInputArrive = 0;

static void
	sdlmsg_replay_native(struct sdlmsg *msg) 
{
	INPUT in;
	//
	switch(msg->msgtype) {
	case SDL_EVENT_MSGTYPE_KEYBOARD:
		//Log::log("keyboard event, code=%x key=%x mod= %x pressed=%u\n", msg->scancode, msg->sdlkey, msg->sdlmod, msg->is_pressed);


		bzero(&in, sizeof(in));
		in.type = INPUT_KEYBOARD;
		if((in.ki.wVk = SDLKeyToKeySym(msg->sdlkey)) != INVALID_KEY) {
			in.ki.wScan = MapVirtualKey(in.ki.wVk, 0);
			if(msg->is_pressed == 0) {
				in.ki.dwFlags |= KEYEVENTF_KEYUP;		
				//in.ki.wScan |= 0x80;
			}
			else{

			}


			if( in.ki.wVk == VK_DELETE){
				serverInputArrive = GetTickCount();
			}

			//in.ki.wScan |= 0x80;
			//ga_error("sdl replayer: vk=%x scan=%x\n", in.ki.wVk, in.ki.wScan);
			if(in.ki.wVk ==VK_UP ||in.ki.wVk ==VK_DOWN || in.ki.wVk ==VK_LEFT ||in.ki.wVk ==VK_RIGHT ){
				in.ki.wScan |= 0x80;
			}

			SendInput(1, &in, sizeof(in));
		} else {

			MessageBox(NULL,"invalid keyboard input", "WARNING", MB_OK);
		}
		break;
	case SDL_EVENT_MSGTYPE_MOUSEKEY:
		//ga_error("sdl replayer: button event btn=%u pressed=%d\n", msg->mousebutton, msg->is_pressed);
		bzero(&in, sizeof(in));
		in.type = INPUT_MOUSE;
		if(msg->mousebutton == 1 && msg->is_pressed != 0) {
			in.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		} else if(msg->mousebutton == 1 && msg->is_pressed == 0) {
			in.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		} else if(msg->mousebutton == 2 && msg->is_pressed != 0) {
			in.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
		} else if(msg->mousebutton == 2 && msg->is_pressed == 0) {
			in.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
		} else if(msg->mousebutton == 3 && msg->is_pressed != 0) {
			in.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		} else if(msg->mousebutton == 3 && msg->is_pressed == 0) {
			in.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		} else if(msg->mousebutton == 4 && msg->is_pressed != 0) {
			// mouse wheel forward
			in.mi.dwFlags = MOUSEEVENTF_WHEEL;
			in.mi.mouseData = +WHEEL_DELTA;
		} else if(msg->mousebutton == 5 && msg->is_pressed != 0) {
			// mouse wheel backward
			in.mi.dwFlags = MOUSEEVENTF_WHEEL;
			in.mi.mouseData = -WHEEL_DELTA;
		}
		SendInput(1, &in, sizeof(in));
		break;
	case SDL_EVENT_MSGTYPE_MOUSEWHEEL:
		if(msg->mousex != 0) {
			bzero(&in, sizeof(in));
			in.type = INPUT_MOUSE;
			if(((short) msg->mousex) > 0) {
				// mouse wheel forward
				in.mi.dwFlags = MOUSEEVENTF_WHEEL;
				in.mi.mouseData = +WHEEL_DELTA;
			} else if(((short) msg->mousex) < 0 ) {
				// mouse wheel backward
				in.mi.dwFlags = MOUSEEVENTF_WHEEL;
				in.mi.mouseData = -WHEEL_DELTA;
			}
			SendInput(1, &in, sizeof(in));
		}
#if 0
		if(msg->mousey != 0) {
			bzero(&in, sizeof(in));
			in.type = INPUT_MOUSE;
			if(((short) msg->mousey) > 0) {
				// mouse wheel forward
				in.mi.dwFlags = MOUSEEVENTF_HWHEEL;
				in.mi.mouseData = +WHEEL_DELTA;
			} else if(((short) msg->mousey) < 0 ) {
				// mouse wheel backward
				in.mi.dwFlags = MOUSEEVENTF_HWHEEL;
				in.mi.mouseData = -WHEEL_DELTA;
			}
			SendInput(1, &in, sizeof(in));
		}
#endif
		break;
	case SDL_EVENT_MSGTYPE_MOUSEMOTION:
		//ga_error("sdl replayer: motion event x=%u y=%d\n", msg->mousex, msg->mousey);
		bzero(&in, sizeof(in));
		in.type = INPUT_MOUSE;

#if 1
		//msg->relativeMouseMode = 1;
		// mouse x/y has to be mapped to (0,0)-(65535,65535)
		if(msg->relativeMouseMode == 0) {
			if(prect == NULL) {
				in.mi.dx = (DWORD)
					(65536.0 * msg->mousex) / cxsize;
				in.mi.dy = (DWORD)
					(65536.0 * msg->mousey) / cysize;
			} else {
				in.mi.dx = (DWORD)
					(65536.0 * (prect->left + msg->mousex)) / cxsize;
				in.mi.dy = (DWORD)
					(65536.0 * (prect->top + msg->mousey)) / cysize;
			}
			in.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
		} 
		else {
#endif
			in.mi.dx = (short) msg->mouseRelX;
			in.mi.dy = (short) msg->mouseRelY;
			in.mi.dwFlags = MOUSEEVENTF_MOVE;
		}
		SendInput(1, &in, sizeof(in));
		break;
	default: // do nothing
		break;
	}
	return;
}

int sdlmsg_replay(struct sdlmsg * msg){
	// convert from network byte order to host byte order
	sdlmsg_ntoh(msg);
	//if(msg->type == INPUT_KEYBOARD){
	//	/*char buff[100];
	//	sprintf(buff, "vk:%d",msg->ki.wVk);
	//	MessageBox(NULL,buff,"WARNING", MB_OK);*/
	//	msg->ki.wScan = MapVirtualKey(msg->ki.wVk, MAPVK_VK_TO_VSC);
	//}else if(msg->type == INPUT_MOUSE){
	//	// mouse move event should corrent the coordinate

	//}
	//SendInput(1,msg,sizeof(INPUT));
	sdlmsg_replay_native(msg);
	return 0;
}
void
	sdlmsg_replay_callback(void *msg, int msglen) {
		if(msglen != sizeof(struct sdlmsg)) {
			// ...
			MessageBox(NULL,"Msglen is not the size of INPUT", "WARNING", MB_OK);
		}
		sdlmsg_replay((struct sdlmsg *)msg);
		return;
}
static void
	SDLKeyToKeySym_init() {
		unsigned short i;
		//
		keymap[SDLK_BACKSPACE]	= VK_BACK;		//		= 8,
		keymap[SDLK_TAB]	= VK_TAB;		//		= 9,
		keymap[SDLK_CLEAR]	= VK_CLEAR;		//		= 12,
		keymap[SDLK_RETURN]	= VK_RETURN;		//		= 13,
		keymap[SDLK_PAUSE]	= VK_PAUSE;		//		= 19,
		keymap[SDLK_ESCAPE]	= VK_ESCAPE;		//		= 27,
		// Latin 1: starting from space (0x20)
		keymap[SDLK_SPACE]	= VK_SPACE;		//		= 32,
		// (0x20) space, exclam, quotedbl, numbersign, dollar, percent, ampersand,
		// (0x27) quoteright, parentleft, parentright, asterisk, plus, comma,
		// (0x2d) minus, period, slash
		//SDLK_EXCLAIM		= 33,
		keymap[SDLK_QUOTEDBL]	= VK_OEM_7;		//		= 34,
		//SDLK_HASH		= 35,
		//SDLK_DOLLAR		= 36,
		//SDLK_AMPERSAND		= 38,
		keymap[SDLK_QUOTE]	= VK_OEM_7;		//		= 39,
		//SDLK_LEFTPAREN		= 40,
		//SDLK_RIGHTPAREN		= 41,
		//SDLK_ASTERISK		= 42,
		keymap[SDLK_PLUS]	= VK_OEM_PLUS;		//		= 43,
		keymap[SDLK_COMMA]	= VK_OEM_COMMA;		//		= 44,
		keymap[SDLK_MINUS]	= VK_OEM_MINUS;		//		= 45,
		keymap[SDLK_PERIOD]	= VK_OEM_PERIOD;	//		= 46,
		keymap[SDLK_SLASH]	= VK_OEM_2;		//		= 47,
		keymap[SDLK_COLON]	= VK_OEM_1;		//		= 58,
		keymap[SDLK_SEMICOLON]	= VK_OEM_1;		//		= 59,
		keymap[SDLK_LESS]	= VK_OEM_COMMA;		//		= 60,
		keymap[SDLK_EQUALS]	= VK_OEM_PLUS;		//		= 61,
		keymap[SDLK_GREATER]	= VK_OEM_PERIOD;	//		= 62,
		keymap[SDLK_QUESTION]	= VK_OEM_2;		//		= 63,
		//SDLK_AT			= 64,
		/* 
		Skip uppercase letters
		*/
		keymap[SDLK_LEFTBRACKET]= VK_OEM_4;		//		= 91,
		keymap[SDLK_BACKSLASH]	= VK_OEM_5;		//		= 92,
		keymap[SDLK_RIGHTBRACKET]= VK_OEM_6;		//		= 93,
		//SDLK_CARET		= 94,
		keymap[SDLK_UNDERSCORE]	= VK_OEM_MINUS;		//		= 95,
		keymap[SDLK_BACKQUOTE]	= VK_OEM_3;		//		= 96,
		// (0x30-0x39) 0-9
		for(i = 0x30; i <= 0x39; i++) {
			keymap[i] = i;
		}
		// (0x3a) colon, semicolon, less, equal, greater, question, at
		// (0x41-0x5a) A-Z
		// SDL: no upper cases, only lower cases
		// (0x5b) bracketleft, backslash, bracketright, asciicircum/caret,
		// (0x5f) underscore, grave
		// (0x61-7a) a-z
		for(i = 0x61; i <= 0x7a; i++) {
			keymap[i] = i & 0xdf;	// convert to uppercases
		}
		keymap[SDLK_DELETE]	= VK_DELETE;		//		= 127,
		// SDLK_WORLD_0 (0xa0) - SDLK_WORLD_95 (0xff) are ignored
		/** @name Numeric keypad */
		keymap[SDLK_KP0]	= VK_NUMPAD0;	//		= 256,
		keymap[SDLK_KP1]	= VK_NUMPAD1;	//		= 257,
		keymap[SDLK_KP2]	= VK_NUMPAD2;	//		= 258,
		keymap[SDLK_KP3]	= VK_NUMPAD3;	//		= 259,
		keymap[SDLK_KP4]	= VK_NUMPAD4;	//		= 260,
		keymap[SDLK_KP5]	= VK_NUMPAD5;	//		= 261,
		keymap[SDLK_KP6]	= VK_NUMPAD6;	//		= 262,
		keymap[SDLK_KP7]	= VK_NUMPAD7;	//		= 263,
		keymap[SDLK_KP8]	= VK_NUMPAD8;	//		= 264,
		keymap[SDLK_KP9]	= VK_NUMPAD9;	//		= 265,
		keymap[SDLK_KP_PERIOD]	= VK_DECIMAL;	//		= 266,
		keymap[SDLK_KP_DIVIDE]	= VK_DIVIDE;	//		= 267,
		keymap[SDLK_KP_MULTIPLY]= VK_MULTIPLY;	//		= 268,
		keymap[SDLK_KP_MINUS]	= VK_SUBTRACT;	//		= 269,
		keymap[SDLK_KP_PLUS]	= VK_ADD;	//		= 270,
		//keymap[SDLK_KP_ENTER]	= XK_KP_Enter;	//		= 271,
		//keymap[SDLK_KP_EQUALS]	= XK_KP_Equal;	//		= 272,
		/** @name Arrows + Home/End pad */
		keymap[SDLK_UP]		= VK_UP;	//		= 273,
		keymap[SDLK_DOWN]	= VK_DOWN;	//		= 274,
		keymap[SDLK_RIGHT]	= VK_RIGHT;	//		= 275,
		keymap[SDLK_LEFT]	= VK_LEFT;	//		= 276,
		keymap[SDLK_INSERT]	= VK_INSERT;	//		= 277,
		keymap[SDLK_HOME]	= VK_HOME;	//		= 278,
		keymap[SDLK_END]	= VK_END;	//		= 279,
		keymap[SDLK_PAGEUP]	= VK_PRIOR;	//		= 280,
		keymap[SDLK_PAGEDOWN]	= VK_NEXT;	//		= 281,
		/** @name Function keys */
		keymap[SDLK_F1]		= VK_F1;	//		= 282,
		keymap[SDLK_F2]		= VK_F2;	//		= 283,
		keymap[SDLK_F3]		= VK_F3;	//		= 284,
		keymap[SDLK_F4]		= VK_F4;	//		= 285,
		keymap[SDLK_F5]		= VK_F5;	//		= 286,
		keymap[SDLK_F6]		= VK_F6;	//		= 287,
		keymap[SDLK_F7]		= VK_F7;	//		= 288,
		keymap[SDLK_F8]		= VK_F8;	//		= 289,
		keymap[SDLK_F9]		= VK_F9;	//		= 290,
		keymap[SDLK_F10]	= VK_F10;	//		= 291,
		keymap[SDLK_F11]	= VK_F11;	//		= 292,
		keymap[SDLK_F12]	= VK_F12;	//		= 293,
		keymap[SDLK_F13]	= VK_F13;	//		= 294,
		keymap[SDLK_F14]	= VK_F14;	//		= 295,
		keymap[SDLK_F15]	= VK_F15;	//		= 296,
		/** @name Key state modifier keys */
		keymap[SDLK_NUMLOCK]	= VK_NUMLOCK;	//		= 300,
		keymap[SDLK_CAPSLOCK]	= VK_CAPITAL;	//		= 301,
		keymap[SDLK_SCROLLOCK]	= VK_SCROLL;	//		= 302,
		keymap[SDLK_RSHIFT]	= VK_RSHIFT;	//		= 303,
		keymap[SDLK_LSHIFT]	= VK_LSHIFT;	//		= 304,
		keymap[SDLK_RCTRL]	= VK_RCONTROL;	//		= 305,
		keymap[SDLK_LCTRL]	= VK_LCONTROL;	//		= 306,
		keymap[SDLK_RALT]	= VK_RMENU;	//		= 307,
		keymap[SDLK_LALT]	= VK_LMENU;	//		= 308,
		keymap[SDLK_RMETA]	= VK_RWIN;	//		= 309,
		keymap[SDLK_LMETA]	= VK_LWIN;	//		= 310,
		//keymap[SDLK_LSUPER]	= XK_Super_L;	//		= 311,		/**< Left "Windows" key */
		//keymap[SDLK_RSUPER]	= XK_Super_R;	//		= 312,		/**< Right "Windows" key */
		keymap[SDLK_MODE]	= VK_MODECHANGE;//		= 313,		/**< "Alt Gr" key */
		//keymap[SDLK_COMPOSE]	= XK_Multi_key;	//		= 314,		/**< Multi-key compose key */
		/** @name Miscellaneous function keys */
		keymap[SDLK_HELP]	= VK_HELP;	//		= 315,
#if ! SDL_VERSION_ATLEAST(2,0,0)
		keymap[SDLK_PRINT]	= VK_PRINT;	//		= 316,
#endif
		//keymap[SDLK_SYSREQ]	= XK_Sys_Req;	//		= 317,
		keymap[SDLK_BREAK]	= VK_CANCEL;	//		= 318,
		keymap[SDLK_MENU]	= VK_MENU;	//		= 319,
#if 0
		SDLK_POWER		= 320,		/**< Power Macintosh power key */
			SDLK_EURO		= 321,		/**< Some european keyboards */
#endif
			//keymap[SDLK_UNDO]	= XK_Undo;	//		= 322,		/**< Atari keyboard has Undo */
			//

			keymap_initialized = true;
		return;
}

static KeySym
#if SDL_VERSION_ATLEAST(2,0,0)
	SDLKeyToKeySym(int sdlkey) {
		map<int, KeySym>::iterator mi;
#else
	SDLKeyToKeySym(unsigned short sdlkey) {
		map<unsigned short, KeySym>::iterator mi;
#endif
		if(keymap_initialized == false) {
			SDLKeyToKeySym_init();
			//keymap_initialized = true;
		}
		if((mi = keymap.find(sdlkey)) != keymap.end()) {
			return mi->second;
		}
		return INVALID_KEY;
}



int sdlmsg_replay_init(void * arg){
	//struct

	if(keymap_initialized == false){
		SDLKeyToKeySym_init();
		//keymap_initialized = true;
	}
	cxsize = GetSystemMetrics(SM_CXSCREEN);
	cysize = GetSystemMetrics(SM_CYSCREEN);
	prect = &winRect;

	return 1;
} 

void checkKeyboard(){


}

DWORD WINAPI listenKey(LPVOID lpParameter){
	// listen 
	while(true){
		checkKeyboard();
		Sleep(100);
	}
	return 1;
}

DWORD WINAPI ctrl_server_thread(LPVOID lpParameter){
	// enter the input server thread, everything should be done here.
	CommonNet * dis = (CommonNet *)lpParameter;
	//UnstructuredNet * dis = (UnstructuredNet*)lpParameter;
	struct sockaddr_in csin, xsin;
	int socket;
	int csinlen, xsinlen;;
	int clientaccepted = 0;
	unsigned char buf[8192];
	int bufhead, buflen;

	// init the replayer
	sdlmsg_replay_init(lpParameter);
	// set replay method
	replay = sdlmsg_replay_callback;

	buflen = 0;
	//cxsize = GetSystemMetrics(SM_CXSCREEN);
	//cysize = GetSystemMetrics(SM_CYSCREEN);
	while(true){
		//Sleep(30);
		int rlen, msglen;

		bufhead = 0;
readmore:
		// receive the input data from client
		if((rlen = dis->get_data())<=0){
			// net work error
			//MessageBox(NULL,"input server get data failed!", "Warning", MB_OK);
			continue;
		}else{
			char sss[100];
			//sprintf(sss,"recv input data:%d\n",rlen);
			//Log::slog("recv input data :%d\n", rlen);
			//MessageBox(NULL,sss, "Warning", MB_OK);
		}
		dis->read_byte_arr((char *)buf+buflen, rlen); // copy msg data to buf
		buflen += rlen;
		// distribute it to queue


tcp_again:

		if(buflen< 2){
			MessageBox(NULL,"not big enough", "Warning", MB_OK);
			goto readmore;
		}
		msglen = ntohs(*((unsigned short*)(buf + bufhead)));
		//msglen = sizeof();
		if(msglen == 0){
			continue;
		}

		/*if(buflen < msglen){
		bcopy(buf-bufhead, buf, buflen);
		goto readmore;
		}*/

		// replay or queue the event
		//INPUT *in = (INPUT*)(buf+bufhead);
		//SendInput(1, in, sizeof(INPUT));
		//printf("input set\n");


		if(replay !=NULL){
			//MessageBox(NULL,"replay input", "warning",MB_OK);
			replay(buf+bufhead, msglen);
		}else if(ctrl_queue_write_msg(buf+bufhead,msglen)!= msglen){
			// error message
		}else{
			// wake up thread
		}
		// handle buffers
		if(buflen > msglen){
			bufhead+= msglen;
			buflen -= msglen;
			goto tcp_again;
		}
		buflen = 0;
	}
	//clientaccepted = 0;

	return NULL;
}

int
	ctrl_server_readnext(void *msg, int msglen) {
		int ret;
		struct queuemsg *qm;
again:
		if((qm = ctrl_queue_read_msg()) != NULL) {
			if(qm->msgsize > msglen) {
				ret = -1;
			} else {
				memcpy(qm->msg, msg, qm->msgsize);
				ret = qm->msgsize;
			}
			ctrl_queue_release_msg(qm);
			return ret;
		}
		// nothing available, wait for next input

		goto again;
		// never return from here
		return 0;
}
#if 0
HHOOK kehook = 0;
extern bool enableRender;
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam){
	if( wParam == VK_DELETE &&  lParam & 0x80000000) // f9 pressed
	{
		EnterCriticalSection(&f9);
		F9Pressed = true;
		Log::log("server deal time:%d\n", GetTickCount() - serverInputArrive);
		LeaveCriticalSection(&f9);
	}
	//MessageBox(NULL,"Enter hook", "WARNING", MB_OK);
	if( lParam & 0x80000000) // pressed
	{
		if( wParam == VK_F8) // f8 pressed
		{
			//MessageBox(NULL,"F8 pressed", "WARNING", MB_OK);
			enableRender = false;
			//Log::slog("F8 pressed");
		}
	}
	// 
	return CallNextHookEx(kehook,nCode,wParam,lParam); 
}
void SetKeyboardHook(HINSTANCE hmode, DWORD dwThreadId){
	// set the keyboard hook
	//MessageBox(NULL,"Enter hook", "WARNING", MB_OK);
	Log::slog("set the keyboard hook!\n");
	kehook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hmode, dwThreadId);
}
#endif

