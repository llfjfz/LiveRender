#include "game_server.h"

HHOOK kehook = 0;
extern bool enableRender;
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam){
	
	//MessageBox(NULL,"Enter hook", "WARNING", MB_OK);
	if( lParam & 0x80000000) // pressed
	{
		if( wParam == VK_F7) // f7 pressed
		{
			//MessageBox(NULL,"F7 pressed", "WARNING", MB_OK);
			enableRender = false;
			//Log::slog("F7 pressed");
		}
	}
	//
	return CallNextHookEx(kehook,nCode,wParam,lParam); 
}

void SetKeyboardHook(HINSTANCE hmode, DWORD dwThreadId) {
	// set the keyboard hook
	//MessageBox(NULL,"Enter hook", "WARNING", MB_OK);
	//Log::slog("set the keyboard hook!\n");
	kehook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hmode, dwThreadId);
}
