#ifndef __GAME_SERVER__
#define __GAME_SERVER__

#include "common_net.h"
#include "opcode.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdlib.h>
#include <assert.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "comctl32.lib")


#define COM_METHOD(TYPE, METHOD) TYPE STDMETHODCALLTYPE METHOD

#include "detours/detours.h"
#pragma comment(lib, "detours/detours.lib")

extern int need_dump_mesh;
extern bool tex_send[4024];
extern CommandServer cs;
extern CommonNet dis;

//要截取的三个全局函数
extern IDirect3D9* (WINAPI* Direct3DCreate9Next)(UINT SDKVersion);
// hook DirectInput8Create
extern HRESULT (WINAPI * DirectInput8CreateNext)(HINSTANCE hinst,DWORD dwVersion,REFIID riidltf,LPVOID *ppvOut,LPUNKNOWN punkOuter);

// hook the create window
extern HWND (WINAPI *CreateWindowNext)( 
	__in DWORD dwExStyle,
	__in_opt LPCSTR lpClassName,
	__in_opt LPCSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in_opt HWND hWndParent,
	__in_opt HMENU hMenu,
	__in_opt HINSTANCE hInstance,
	__in_opt LPVOID lpParam);
// hook the create window with unicode
extern HWND (WINAPI *CreateWindowExWNext)(
	__in DWORD dwExStyle,
	__in_opt LPCWSTR lpClassName,
	__in_opt LPCWSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in_opt HWND hWndParent,
	__in_opt HMENU hMenu,
	__in_opt HINSTANCE hInstance,
	__in_opt LPVOID lpParam);

extern void (WINAPI* ExitProcessNext)(UINT uExitCode);

HWND WINAPI CreateWindowCallback(DWORD dwExStyle,LPCSTR lpClassName,LPCSTR lpWindowName, DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent, HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
HWND WINAPI CreateWindowExWCallback( DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HRESULT WINAPI DirectInput8CreateCallback(HINSTANCE hinst,DWORD dwVersion,REFIID riidltf,LPVOID *ppvOut,LPUNKNOWN punkOuter);
IDirect3D9* WINAPI Direct3DCreate9Callback(UINT SDKVersion);
void WINAPI ExitProcessCallback(UINT uExitCode);

//从loader传进来的cmd那里获得CmmandStream，InputStream的socket句柄
void GetSocketsFromCmd();
SOCKET GetProcessSocket(SOCKET oldsocket, DWORD source_pid);
void RaiseToDebugP();

void SetKeyboardHook(HINSTANCE hmode, DWORD dwThreadId);

extern bool enableRender;
extern bool F9Pressed;
#endif

