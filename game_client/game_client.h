#ifndef __GAME_CLIENT__
#define __GAME_CLIENT__

#include "utility.h"
#include "opcode.h"
#include "client_api.h"
#include <d3d9.h>
#include <d3dx9core.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define COM_METHOD(TYPE, METHOD) TYPE STDMETHODCALLTYPE METHOD

#include "client_vertexbuffer9.h"
#include "client_indexbuffer9.h"
#include "client_texture9.h"
#include "client_stateblock9.h"
#include "client_cubetexture9.h"
#include "client_swapchain9.h"
#include "client_surface9.h"

extern IDirect3D9* g_d3d;

extern HWND window_handle;
extern int op_code;
extern int obj_id;
extern CommandClient cc;

#define WINDOW_CLASS "MYWINDOWCLASS"
#define WINDOW_NAME "game-client"

#define Max_Obj_Cnt 20010

extern void* device_list[Max_Obj_Cnt];

extern string game_name;
extern bool use_server_style;

HRESULT client_init();
bool init_window(int width,int height, DWORD dwExStyle, DWORD dwStyle);
void init_fptable();


extern DWORD WINAPI InputThread(LPVOID lpParameter);
extern CRITICAL_SECTION main_section;
extern PCRITICAL_SECTION p_main_section;
extern bool main_thread_running;

typedef unsigned (__stdcall * PTHREAD_START)(void *);
#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, \
	pvParam, fdwCreate, pdwThreadID) \
	((HANDLE) _beginthreadex( \
	(void *)(psa), \
	(unsigned)(cbStack), \
	(PTHREAD_START)(pfnStartAddr), \
	(void *)(pvParam), \
	(unsigned )(fdwCreate), \
	(unsigned *)(pdwThreadID)))


#endif