#include "common_net.h"

#include "SDL2/SDL.h"
//#include "SDL.h"
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <ws2ipdef.h>
#include <dinput.h>
#pragma comment(lib,"SDL2.lib")
#ifndef bzero
#define	bzero(m,n)		ZeroMemory(m, n)
#endif
// bcopy
#ifndef bcopy
#define	bcopy(s,d,n)		CopyMemory(d, s, n)
#endif
// strncasecmp
#ifndef strncasecmp
#define	strncasecmp(a,b,n)	_strnicmp(a,b,n)
#endif
// strcasecmp
#ifndef strcasecmp
#define	strcasecmp(a,b)		_stricmp(a,b)
#endif
// strdup
#ifndef strdup
#define	strdup(s)		_strdup(s)
#endif
// strtok_r
#ifndef strtok_r
#define strtok_r(s,d,v)		strtok_s(s,d,v)
#endif
// snprintf
#ifndef snprintf
#define	snprintf(b,n,f,...)	_snprintf_s(b,n,_TRUNCATE,f,__VA_ARGS__)
#endif
// vsnprintf
#ifndef vsnprintf
#define	vsnprintf(b,n,f,ap)	vsnprintf_s(b,n,_TRUNCATE,f,ap)
#endif
// strncpy
#ifndef strncpy
#define	strncpy(d,s,n)		strncpy_s(d,n,s,_TRUNCATE)
#endif
// getpid
#ifndef getpid
#define	getpid			_getpid
#endif
// gmtimr_r
#ifndef gmtime_r
#define	gmtime_r(pt,ptm)	gmtime_s(ptm,pt)
#endif
// dlopen
#ifndef dlopen
#define	dlopen(f,opt)		LoadLibrary(f)
#endif
// dlsym
#ifndef dlsym
#define	dlsym(h,name)		GetProcAddress(h,name)
#endif
// dlclose
#ifndef dlclose
#define	dlclose(h)		FreeLibrary(h)
#endif

#define	SDL_EVENT_MSGTYPE_NULL		0
#define	SDL_EVENT_MSGTYPE_KEYBOARD	1
#define	SDL_EVENT_MSGTYPE_MOUSEKEY	2
#define SDL_EVENT_MSGTYPE_MOUSEMOTION	3
#define SDL_EVENT_MSGTYPE_MOUSEWHEEL	4

typedef unsigned (__stdcall * PTHREAD_START) (void *);
#define chBEGINTHREADEX(psa, cbStack,pfnStartAddr, \
	pvParam, fdwCreate, pdwThreadID) \
	((HANDLE) _beginthreadex( \
	(void *)(psa), \
	(unsigned)(cbStack), \
	(PTHREAD_START)(pfnStartAddr), \
	(void *)(pvParam), \
	(unsigned )(fdwCreate), \
	(unsigned *)(pdwThreadID)))


LRESULT CALLBACK mouse_message_callback(int nCode, WPARAM wParam, LPARAM iParam);
LRESULT CALLBACK keyboard_message_callback(int nCode, WPARAM wParam, LPARAM iParam);
void SetMouseAndKeyboardHook(DWORD threadId);
bool UnHookInput();

struct sdlmsg {
	unsigned short msgsize;		// size of this data-structure
	// every message MUST start from a
	// unsigned short message size
	// the size includes the 'msgsize'
	unsigned char msgtype;
	unsigned char is_pressed;	// for keyboard/mousekey
	unsigned char mousebutton;	// mouse button
	unsigned char mousestate;	// mouse state - key combinations for motion
#if SDL_VERSION_ATLEAST(2,0,0)
	unsigned char unused1[2];		// padding - 3+1 chars
	unsigned short scancode;	// keyboard scan code
	int sdlkey;			// SDLKey value
	unsigned int unicode;		// unicode or ASCII value
#else
	unsigned char scancode;		// keyboard scan code
	unsigned short sdlkey;		// SDLKey value
	unsigned short unicode;		// unicode or ASCII value
#endif
	unsigned short sdlmod;		// SDLMod value
	unsigned short mousex;		// mouse position (big-endian)
	unsigned short mousey;		// mouse position (big-endian)
	unsigned short mouseRelX;	// mouse relative position (big-endian)
	unsigned short mouseRelY;	// mouse relative position (big-endian)
	unsigned char relativeMouseMode;// relative mouse mode?
	unsigned char padding[8];	// reserved padding
};
CRITICAL_SECTION queue_section;
CRITICAL_SECTION main_section;
CRITICAL_SECTION capture_section;
LPCRITICAL_SECTION p_queue_section = NULL;
LPCRITICAL_SECTION p_main_section = NULL;
LPCRITICAL_SECTION p_capture_section = NULL;

struct queuemsg {
	unsigned short msgsize;		// a general header for messages
	unsigned char msg[2];		// use '2' to prevent Windows from complaining
};

static int qhead, qtail, qsize, qunit;
static unsigned char *qbuffer = NULL;


extern HINSTANCE hinstance;
extern HWND hWnd;


int inputTickStart = 0;
int inputTickEnd = 0;

int
	ga_error(const char *fmt, ...) {
		char msg[4096];
		struct timeval tv;
		va_list ap;

		va_start(ap, fmt);
#ifdef ANDROID
		__android_log_vprint(ANDROID_LOG_INFO, "ga_log", fmt, ap);
#else
		vsnprintf(msg, sizeof(msg), fmt, ap);
#endif
		va_end(ap);
		//fprintf(stderr, "# [%d] %ld.%06ld %s", getpid(), tv.tv_sec, tv.tv_usec, msg);

		//
		return -1;
}



int
	ctrl_queue_write_msg(void *msg, int msgsize) {
		int nextpos;
		struct queuemsg *qmsg;
		//
		if((msgsize + sizeof(struct queuemsg)) > qunit) {
			ga_error("controller queue: msg size exceeded (%d > %d).\n",
				msgsize + sizeof(struct queuemsg), qunit);
			return 0;
		}
		//WaitForSingleObject(queue_mutex,INFINITE);
		EnterCriticalSection(p_queue_section);
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
			bcopy(msg, qmsg->msg, msgsize);
			qtail = nextpos;
		}
		LeaveCriticalSection(p_queue_section);
		//
		return msgsize;
}

void
	ctrl_client_sendmsg(void *msg, int msglen) {

		if(ctrl_queue_write_msg(msg, msglen) != msglen) {
			ga_error("queue full, message dropped.\n");
		} else {

		}
		return;
}



struct sdlmsg *
#if SDL_VERSION_ATLEAST(2,0,0)
	sdlmsg_keyboard(struct sdlmsg *msg, unsigned char pressed, unsigned short scancode, SDL_Keycode key, unsigned short mod, unsigned int unicode){
#else
	sdlmsg_keyboard(struct sdlmsg *msg, unsigned char pressed, unsigned char scancode, SDLKey key, SDLMod mod, unsigned short unicode){
#endif
		ga_error("sdl client: key event code=%x key=%x mod=%x pressed=%u\n", scancode, key, mod, pressed);
		bzero(msg, sizeof(struct sdlmsg));
		msg->msgsize = htons(sizeof(struct sdlmsg));
		msg->msgtype = SDL_EVENT_MSGTYPE_KEYBOARD;
		msg->is_pressed = pressed;
#if SDL_VERSION_ATLEAST(2,0,0)
#if 1
		msg->scancode = htons(scancode);
		msg->sdlkey = htonl(key);
		msg->unicode = htonl(unicode);
#else
		msg->scancode = scancode;
		msg->sdlkey = key;
		msg->unicode = unicode;
#endif
#else
		msg->scancode = scancode;
		msg->sdlkey = htons(key);
		msg->unicode = htons(unicode);
#endif
		msg->sdlmod = htons(mod);
		ga_error("delt key event code=%x key=%x mod=%x pressed=%u\n", msg->scancode, msg->sdlkey , mod, pressed);
		return msg;
}
struct sdlmsg *
	sdlmsg_mousekey(struct sdlmsg *msg, unsigned char pressed, unsigned char button) {
		//ga_error("sdl client: button event btn=%u pressed=%u\n", button, pressed);
		bzero(msg, sizeof(struct sdlmsg));
		msg->msgsize = htons(sizeof(struct sdlmsg));
		msg->msgtype = SDL_EVENT_MSGTYPE_MOUSEKEY;
		msg->is_pressed = pressed;
		msg->mousebutton = button;
		return msg;
}

struct sdlmsg *
	sdlmsg_mousemotion(struct sdlmsg *msg, unsigned short mousex, unsigned short mousey, unsigned short relx, unsigned short rely, unsigned char state, int relativeMouseMode) {
		//ga_error("sdl client: motion event x=%u y=%u\n", mousex, mousey);
		bzero(msg, sizeof(struct sdlmsg));
		msg->msgsize = htons(sizeof(struct sdlmsg));
		msg->msgtype = SDL_EVENT_MSGTYPE_MOUSEMOTION;
		msg->mousestate = state;
		msg->mousex = htons(mousex);
		msg->mousey = htons(mousey);
		msg->mouseRelX = htons(relx);
		msg->mouseRelY = htons(rely);
		msg->relativeMouseMode = (relativeMouseMode != 0) ? 1 : 0;
		return msg;
}
unsigned char relativeMouseMode = 1;

void ProcessEvent(SDL_Event *event) {
	struct sdlmsg m;
	int showCursor =1;
	relativeMouseMode = 1;
	switch(event->type) {
	case SDL_KEYUP:
		if(event->key.keysym.sym == SDLK_BACKQUOTE && relativeMouseMode !=0
			) {
				showCursor = 1 - showCursor;
				//SDL_ShowCursor(showCursor);
				if(showCursor)
					SDL_SetRelativeMouseMode(SDL_FALSE);
				else
					SDL_SetRelativeMouseMode(SDL_TRUE);
		}
		//

		sdlmsg_keyboard(&m, 0,
			event->key.keysym.scancode,
			event->key.keysym.sym,
			event->key.keysym.mod,
			event->key.keysym.unicode);
		ctrl_client_sendmsg(&m, sizeof(m));

		break;
	case SDL_KEYDOWN:

		if(event->key.keysym.sym == SDLK_DELETE){
			// special key for response
			//Log::slog("F9 pressed!\n");
			inputTickStart = GetTickCount();
		}

		sdlmsg_keyboard(&m, 1,
			event->key.keysym.scancode,
			event->key.keysym.sym,
			event->key.keysym.mod,
			event->key.keysym.unicode);
		ctrl_client_sendmsg(&m, sizeof(m));

		break;
	case SDL_MOUSEBUTTONUP:

		sdlmsg_mousekey(&m, 0, event->button.button);
		ctrl_client_sendmsg(&m, sizeof(m));

		break;
	case SDL_MOUSEBUTTONDOWN:

		sdlmsg_mousekey(&m, 1, event->button.button);
		ctrl_client_sendmsg(&m, sizeof(m));

		break;
	case SDL_MOUSEMOTION:

		sdlmsg_mousemotion(&m,
			event->motion.x,
			event->motion.y,
			event->motion.xrel,
			event->motion.yrel,
			event->motion.state,
			relativeMouseMode == 0?0:1);
		ctrl_client_sendmsg(&m, sizeof(m));

		break;
	case SDL_QUIT:
		//main_thread_running = false;
		return;
	default:
		// do nothing
		break;
	}
	return;
}
// queue routines
int
	ctrl_queue_init(int size, int maxunit) {
		qunit = maxunit + sizeof(struct queuemsg);
		qsize = size - (size % qunit);
		qhead = qtail = 0;
		if((qbuffer = (unsigned char*) malloc(qsize)) == NULL) {
			return -1;
		}
		ga_error("controller queue: initialized size=%d (%d units)\n",
			qsize, qsize/qunit);
		return 0;
}

struct queuemsg *
	ctrl_queue_read_msg() {
		struct queuemsg *msg;
		//
		//pthread_mutex_lock(&queue_mutex);
		EnterCriticalSection(p_queue_section);
		if(qtail == qhead) {
			// queue is empty
			msg = NULL;
			//msg->msgsize = 0;
		} else {
			msg = (struct queuemsg *) (qbuffer + qhead);
			//msg->msgsize = sizeof(struct sdlmsg);
		}
		//pthread_mutex_unlock(&queue_mutex);
		LeaveCriticalSection(p_queue_section);
		//
		return msg;
}

void
	ctrl_queue_release_msg(struct queuemsg *msg) {
		struct queuemsg *currmsg;
		EnterCriticalSection(p_queue_section);
		if(qhead == qtail) {
			// queue is empty
			LeaveCriticalSection(p_queue_section);
			return;
		}
		currmsg = (struct queuemsg *) (qbuffer + qhead);
		if(msg != currmsg) {
			ga_error("controller queue: WARNING - release an incorrect msg?\n");
		}
		qhead += qunit;
		if(qhead == qsize) {
			qhead = 0;
		}
		LeaveCriticalSection(p_queue_section);
		return;
}

bool capture_thread_running = true;
bool main_thread_running = true;

DWORD WINAPI InputUploadThread(LPVOID lpParameter){
	// the thread send the input data to server
	//UnstructuredNet * dis = (UnstructuredNet *)lpParameter;
	CommonNet * dis = (CommonNet *)lpParameter;
	int wlen;
	int buf_msg_size = 0;

	while(main_thread_running){
		struct queuemsg * qm;

		//MessageBox(NULL,"SendInput data before mutex", "WARNING", MB_OK);

		dis->begin_transaction();
		while((qm = ctrl_queue_read_msg()) != NULL){

			if(!dis){
				//Log::log("dis NULL\n");
			}
			// send data
			dis->write_byte_arr((char *)qm->msg, qm->msgsize);

			// check the input
			struct sdlmsg * mms =(struct sdlmsg *)qm->msg;

			buf_msg_size += qm->msgsize;
			ctrl_queue_release_msg(qm);
			//if(buf_msg_size>100){
			//send data and release mutex
			dis->end_transaction();

			Log::log("send msg\n");
			if( mms->sdlkey == htonl( SDLK_F9) && mms->is_pressed){
				// send f9 key message
				//Log::slog("client deal time:%d\n", GetTickCount() - inputTickStart);
			}

			dis->begin_transaction();

			buf_msg_size = 0;
			//}


		}
		// when msg not null, send msg data to server

	}

	return NULL;
}

// the main entry for input thread
DWORD WINAPI InputThread(LPVOID lpParameter){
	CommonNet * dis = (CommonNet *)lpParameter;
	//UnstructuredNet * dis = (UnstructuredNet *)lpParameter;
	// parameters all in lpParameter
	//SDL_Event eventt;
	// init the mutex for msg queue
	//queue_mutex = CreateMutex(NULL,TRUE, NULL);
	InitializeCriticalSection(&queue_section);
	InitializeCriticalSection(&capture_section);
	p_queue_section  = &queue_section;
	p_capture_section = &capture_section;
	//init sdl

	if(ctrl_queue_init(32768, sizeof(struct sdlmsg)) < 0) {
		//error message
	}

	// create upload thread
	DWORD dwThreadID;
	dwThreadID = GetCurrentThreadId();
	//SetMouseAndKeyboardHook(dwThreadID);
	HANDLE hThreadInput = chBEGINTHREADEX(NULL,0,InputUploadThread, dis,0,&dwThreadID);
	atexit(SDL_Quit);

	SDL_Event eventt;
	while(main_thread_running){
		if(SDL_WaitEvent(&eventt)){
			ProcessEvent(&eventt);
			switch(eventt.type){
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				//printf("key event\n");
				//Log::log("key event\n");
				break;
			case SDL_MOUSEBUTTONDOWN:
				//printf("mouse down\n");
				//Log::log("mouse down\n");
				break;
			default:
				break;
			}
		}
	}


	EnterCriticalSection(p_capture_section);
	capture_thread_running = false;
	LeaveCriticalSection(p_capture_section);
	//WaitForSingleObject(hThreadInput,INFINITE);
	return 0;
}
void capture_event()
{
	SDL_Event eventt;
	//MessageBox(NULL,"try event capture!", "WARNING", MB_OK);
	if(SDL_PollEvent(&eventt)){
		//MessageBox(NULL,"event captured!", "WARNING", MB_OK);
		ProcessEvent(&eventt);
	}
}

HWND GetWindowHandleByPID(DWORD dwProcessID){
	HWND h = GetTopWindow(0);
	while(h){
		DWORD pid = 0;
		DWORD dwThreadId = GetWindowThreadProcessId(h, &pid);
		if(dwThreadId !=0){
			if(pid == dwProcessID)// find the process id
			{
				// here h is the handle to the window
				return h;
			}
		}
		h = GetNextWindow(h, GW_HWNDNEXT);
	}
	return NULL;
}
HWND CreateWindowWithSDL(int w, int h, int x,int y){
	SDL_Window * window;
	SDL_Event evet;
	int done;

	if(SDL_Init(SDL_INIT_VIDEO)<0){
		fprintf(stderr, "Couldn't initilize SDL: %s\n", SDL_GetError());
		return NULL;
	}
	window = SDL_CreateWindow("game-client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
	if(!window){
		fprintf(stderr, "Couldn't create window:%s\n", SDL_GetError());
		return NULL;
	}
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(0);
	SDL_StartTextInput();

	return GetWindowHandleByPID(GetCurrentProcessId());
}