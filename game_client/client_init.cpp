#include "utility.h"
#include "game_client.h"
#define SDL_WINDOW

HWND hWnd = NULL;
void DestroyD3DWindow() {
	printf("Destroying D3D window...\n");

	if(g_d3d != NULL) g_d3d->Release();
	g_d3d = NULL;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch(msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;

	case WM_KEYUP:
		if(wp == VK_ESCAPE) PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

extern HWND CreateWindowWithSDL(int w, int h, int x, int y);

bool init_window(int width, int height, DWORD dwExStyle, DWORD dwStyle) {
	Log::slog("init_window(), width=%d, height=%d\n", width, height);
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, WINDOW_CLASS, NULL };
	RegisterClassEx(&wc);

	if(width < 800) width = 800;
	if(height < 600) height = 600;

#ifdef SDL_WINDOW
	hWnd = CreateWindowWithSDL(width, height, 0, 0);
	window_handle = hWnd;
	//hh = hWnd;
#else
	if(use_server_style) {
		//window_handle = CreateWindow(WINDOW_CLASS, WINDOW_NAME, dwStyle, 0, 0, width, height, GetDesktopWindow(), NULL, wc.hInstance, NULL);
		if(game_name[0] == 'C' || game_name[0] == 'c')
			window_handle = CreateWindow(WINDOW_CLASS, WINDOW_NAME, dwExStyle, 0, 0, width, height, GetDesktopWindow(), NULL, wc.hInstance, NULL);
		else
			window_handle = CreateWindowEx(dwExStyle,WINDOW_CLASS,WINDOW_NAME,dwStyle,0,0,width,height,GetDesktopWindow(),NULL,wc.hInstance,NULL);
	}
	else
		window_handle = CreateWindow(WINDOW_CLASS, WINDOW_NAME, WS_OVERLAPPEDWINDOW, 0, 0, width, height, GetDesktopWindow(), NULL, wc.hInstance, NULL);
#endif
	return true;
}

D3DDISPLAYMODE displayMode;
D3DPRESENT_PARAMETERS d3dpp;

extern IDirect3DDevice9* cur_device;

HRESULT client_init() {
	Log::slog("client_init() called\n");

	if(g_d3d == NULL) {
		g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	}

	if(FAILED(g_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode))) return false;

	/////////////////////////////////////////////
	//read data
	int id = cc.read_int();
	UINT Adapter = cc.read_uint();
	D3DDEVTYPE DeviceType = (D3DDEVTYPE)(cc.read_uint());
	DWORD BehaviorFlags = cc.read_uint();

	cc.read_byte_arr((char*)(&d3dpp), sizeof(d3dpp));
	d3dpp.BackBufferFormat = displayMode.Format;
	/////////////////////////////////////////////

	Log::log("client_init(), init_window start, Presentation Parameter back buffer witth:%d, back buffer height:%d\n", d3dpp.BackBufferWidth, d3dpp.BackBufferHeight);
	d3dpp.BackBufferHeight = 600;
	d3dpp.BackBufferWidth = 800;

	//init_window(d3dpp.BackBufferWidth, d3dpp.BackBufferHeight);
	HWND hh = FindWindow(NULL,"game-client");
	if(hh == NULL) {
		Log::slog("window_handle is NULL\n");
	}

	d3dpp.hDeviceWindow = hh;
	window_handle = hh;

	LPDIRECT3DDEVICE9 base_device = NULL;
	HRESULT hr = g_d3d->CreateDevice(Adapter, DeviceType, hh, BehaviorFlags, &d3dpp, &base_device);
	device_list[id] = base_device;
	cur_device= base_device;

	switch(hr){
	case D3D_OK:
		Log::log("client_init(), create devie return D3D_OK\n");
		break;
	case D3DERR_DEVICELOST:
		Log::log("client_init(), create device return D3DERR_DEVICELOST.\n");
		break;
	case D3DERR_INVALIDCALL:
		Log::log("client_init(), create devie return D3DERR_INVALIDCALL\n");
		break;
	case D3DERR_NOTAVAILABLE:
		Log::log("client_init(), create device return D3DERR_NOTAVAILABLE.\n");
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		Log::log("client_init(), create device return D3DERR_OUTOFVIDEOMEMORY.\n");
		break;
	default:
		break;

	}
	if(base_device == NULL) {
		Log::log("client_init(), device is NULL, id=%d\n", id);
	}
	else {
		Log::log("client_init(), device is good, id=%d\n", id);
	}

	Log::log("server_init(), CreateDevice end\n");

	ShowWindow(window_handle, SW_SHOWNORMAL);
	UpdateWindow(window_handle);
	atexit(DestroyD3DWindow);

	return hr;
}