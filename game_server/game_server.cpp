#include <process.h>
#include "common_net.h"
#include "game_server.h"
#include "InputServer.h"

int need_dump_mesh = 0;
CommandServer cs(Max_Buf_Size);

bool enableRender = true;
bool F9Pressed = false;

CRITICAL_SECTION f9;
CommonNet dis(1);

IDirect3D9* (WINAPI* Direct3DCreate9Next)(UINT SDKVersion) = Direct3DCreate9;
HRESULT (WINAPI * DirectInput8CreateNext)(HINSTANCE hinst,DWORD dwVersion,REFIID riidltf,LPVOID *ppvOut,LPUNKNOWN punkOuter) = DirectInput8Create;
HWND (WINAPI *CreateWindowNext)( 
	DWORD dwExStyle,
	LPCSTR lpClassName,
	LPCSTR lpWindowName,
	DWORD dwStyle,
	int X,
	int Y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	HINSTANCE hInstance,
	LPVOID lpParam) = CreateWindowExA;

HWND (WINAPI *CreateWindowExWNext)(
	DWORD dwExStyle,
	LPCWSTR lpClassName,
	LPCWSTR lpWindowName,
	DWORD dwStyle,
	int X,
	int Y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	HINSTANCE hInstance,
	LPVOID lpParam) = CreateWindowExW;

//Ωÿ»°ExitProcess
void (WINAPI* ExitProcessNext)(UINT uExitCode) = ExitProcess;

void StartHook() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID*)&CreateWindowNext, CreateWindowCallback);
	DetourAttach((PVOID*)&CreateWindowExWNext, CreateWindowExWCallback);
	DetourAttach((PVOID*)&Direct3DCreate9Next, Direct3DCreate9Callback);

	DetourAttach(&(PVOID&)ExitProcessNext, ExitProcessCallback);

	DetourTransactionCommit();
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ) {

	static int first = 0;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			
			Log::init("game_server.log");


			Log::log(GetCommandLine());
			cs.start_up();
			GetSocketsFromCmd();
			Log::log("Dllmain(), connect_socket=%d, input socket = %d\n", cs.get_connect_socket(), -1);

			enableRender = true;

			StartHook();

			SetKeyboardHook(NULL, GetCurrentThreadId());

			// create the input server thread
			InitializeCriticalSection(&f9);
			DWORD dwThreadId;
#ifdef USE_CLIENT_INPUT
			HANDLE hThreadInput = chBEGINTHREADEX(NULL, 0, ctrl_server_thread, &dis, 0, &dwThreadId);
#endif
			first = 0;

			break;
		}
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	case DLL_PROCESS_DETACH:
		{
			//don't do anything here
			break;
		}
		WM_ACTIVATE;
	}
	return TRUE;
}
