#ifndef __INPUTSERVER_H__
#define __INPUTSERVER_H__

#include "SDL2\SDL.h"
#include "common_net.h"

#define	CTRL_MAX_ID_LENGTH	64
#define	CTRL_CURRENT_VERSION	"GACtrlV01"
#define	CTRL_QUEUE_SIZE		65536	// 64K

#define	SDL_EVENT_MSGTYPE_NULL		0
#define	SDL_EVENT_MSGTYPE_KEYBOARD	1
#define	SDL_EVENT_MSGTYPE_MOUSEKEY	2
#define SDL_EVENT_MSGTYPE_MOUSEMOTION	3
#define SDL_EVENT_MSGTYPE_MOUSEWHEEL	4

#define	VK_PRETENDED_LCONTROL	0x98	// L_CONTROL key: 162
#define	VK_PRETENDED_LALT	0x99	// L_ALT key: 164

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

// bzero
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

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

struct WIN32IMAGE {
	int width;
	int height;
	int bytes_per_line;
};

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


typedef void (*msgfunc)(void *, int);

// handshake message: 
struct ctrlhandshake {
	unsigned char length;
	char id[CTRL_MAX_ID_LENGTH];
};

struct queuemsg {
	unsigned short msgsize;		// a general header for messages
	unsigned char msg[2];		// use '2' to prevent Windows from complaining
};

struct myRect{
	int left, top;
	int right, bottom;
	int width, height;
	int linesize;
	int size;
};

int			ctrl_queue_init(int size, int maxunit);
struct queuemsg *	ctrl_queue_read_msg();
void			ctrl_queue_release_msg();
int			ctrl_queue_write_msg(void *msg, int msgsize);
void			ctrl_queue_clear();

int	ctrl_socket_init(struct RTSPConf *conf);

int	ctrl_client_init(struct RTSPConf *conf, const char *ctrlid);
void*	ctrl_client_thread(void *rtspconf);
void	ctrl_client_sendmsg(void *msg, int msglen);

int	ctrl_server_init( const char *ctrlid);
msgfunc ctrl_server_setreplay(msgfunc);
DWORD WINAPI ctrl_server_thread(LPVOID lpParameter);
int	crtl_server_readnext(void *msg, int msglen);



double intall_input_hook(CommonNet &dis,HMODULE hModule);

DWORD WINAPI listenKey(LPVOID lpParameter);

//HHOOK kehook = 0;
//bool enableRender = true;
//extern void SetKeyboardHook(HINSTANCE hmode, DWORD dwThreadId);

#endif
