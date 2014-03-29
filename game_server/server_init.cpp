#include "game_server.h"
#include "wrap_direct3d9.h"

#include "wrap_direct3d9.h"
#include "wrap_direct3dvertexbuffer9.h"
#include "wrap_direct3dindexbuffer9.h"
#include "wrap_direct3dvertexdeclaration9.h"
#include "wrap_direct3dvertexshader9.h"
#include "wrap_direct3dpixelshader9.h"
#include "wrap_direct3dtexture9.h"
#include "wrap_direct3dstateblock9.h"
#include "wrap_direct3ddevice9.h"
#include "wrap_direct3dcubetexture9.h"
#include "wrap_direct3dswapchain9.h"
#include "wrap_direct3dsurface9.h"

#include "ymesh.h"


int WrapperDirect3D9::ins_count = 0;
int WrapperDirect3DDevice9::ins_count = 0;
int WrapperDirect3DVertexBuffer9::ins_count = 0;
int WrapperDirect3DIndexBuffer9::ins_count = 0;
int WrapperDirect3DVertexDeclaration9::ins_count = 0;
int WrapperDirect3DVertexShader9::ins_count = 0;
int WrapperDirect3DPixelShader9::ins_count = 0;
int WrapperDirect3DTexture9::ins_count = 0;
int WrapperDirect3DStateBlock9::ins_count = 0;
int WrapperDirect3DCubeTexture9::ins_count = 0;
int WrapperDirect3DSwapChain9::ins_count = 0;
int WrapperDirect3DSurface9::ins_count = 0;

HashSet WrapperDirect3D9::m_list;
HashSet WrapperDirect3DIndexBuffer9::m_list;
HashSet WrapperDirect3DVertexBuffer9::m_list;
HashSet WrapperDirect3DPixelShader9::m_list;
HashSet WrapperDirect3DVertexShader9::m_list;
HashSet WrapperDirect3DTexture9::m_list;
HashSet WrapperDirect3DDevice9::m_list;
HashSet WrapperDirect3DCubeTexture9::m_list;
HashSet WrapperDirect3DSwapChain9::m_list;
HashSet WrapperDirect3DSurface9::m_list;

HWND WINAPI CreateWindowCallback(DWORD dwExStyle,LPCSTR lpClassName,LPCSTR lpWindowName, DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent, HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {
	Log::slog("CreateWindowCallback() called, width:%d, height:%d\n", nWidth, nHeight);
	/*if( nWidth < 800)nWidth = 800;
	if(nHeight < 600) nHeight = 600;*/
	
	cs.begin_command(CreateWindow_Opcode, 0);
	cs.write_uint(dwExStyle);
	cs.write_uint(dwStyle);
	cs.write_int(x);
	cs.write_int(y);
	cs.write_int(nWidth);
	cs.write_int(nHeight);
	cs.end_command();
	
	return CreateWindowNext(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
}

HWND WINAPI CreateWindowExWCallback( DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
 int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
	Log::slog("CreateWindowExWCallback() called, Widht:%d, Height:%d\n", nWidth, nHeight);
	
	/*if( nWidth < 800)nWidth = 800;
	if(nHeight < 600) nHeight = 600;*/
	cs.begin_command(CreateWindow_Opcode, 0);
	cs.write_uint(dwExStyle);
	cs.write_uint(dwStyle);
	cs.write_int(X);
	cs.write_int(Y);
	cs.write_int(nWidth);
	cs.write_int(nHeight);
	cs.end_command();
	
	return CreateWindowExWNext(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
}

IDirect3D9* WINAPI Direct3DCreate9Callback(UINT SDKVersion) {
	Log::log("Direct3DCreate9Callback() called\n");
	IDirect3D9* base_d3d9 = Direct3DCreate9Next(SDKVersion);
	IDirect3D9* pv = NULL;

	if(base_d3d9) {
		pv = dynamic_cast<IDirect3D9*>(WrapperDirect3D9::GetWrapperD3D9(base_d3d9));
		cs.begin_command(DirectCreate_Opcode, 0);
		cs.end_command();
	}

	return pv;
}

/*
HRESULT WINAPI DirectInput8CreateCallback(HINSTANCE hinst,DWORD dwVersion,REFIID riidltf,LPVOID *ppvOut,LPUNKNOWN punkOuter) {
	Log::log("DirectInput8CreateCallback() called\n");
	LPVOID out = NULL;
	HRESULT hr = DirectInput8CreateNext(hinst, dwVersion, riidltf, &out,punkOuter);
	IDirectInput8 * pv = NULL;
	WrapperDirectInput8::ins_count_s++;
	if(out){
		pv = dynamic_cast<IDirectInput8 *>(new WrapperDirectInput8((IDirectInput8 *)out,WrapperDirectInput8::ins_count_s - 1));
		// return the wrapper object instead of original instance
		(*ppvOut) = pv;
	}

	cs.begin_command(DirectInputCreate_Opcode,0);

	cs.end_command();
	return D3D_OK;
}
*/

void GetSocketsFromCmd() {
	Log::log("GetSocketsFromCmd() called\n");

	char * cmdLine;
	cmdLine = GetCommandLine();
	int len = strlen(cmdLine);

	if(len < 100) {
		Log::log("GetSocketsFromCmd(), cmdLine=%s\n", cmdLine);
		string str = cmdLine;
		istringstream in(str);
		DWORD command_socket_handle, input_socket_handle, loader_process_id;
		string appname;

		//string local, port;
		in >> appname >> need_dump_mesh >> command_socket_handle >> input_socket_handle >> loader_process_id;

		if(command_socket_handle == -1) {
			cs.set_connect_socket(-1);
			dis.set_connect_socket(-1);
		}
		else {
			cs.set_connect_socket(GetProcessSocket(command_socket_handle, loader_process_id));
			dis.set_connect_socket(GetProcessSocket(input_socket_handle, loader_process_id));
		}

	}
	else {
		Log::log("GetSocketsFromCmd(), cmd len >= 100\n");
	}
}

SOCKET GetProcessSocket(SOCKET oldsocket, DWORD source_pid) {
	RaiseToDebugP();
	HANDLE source_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, source_pid);
	HANDLE new_handle = 0;
	if(source_handle == NULL) {
		Log::log("GetProcessSocket(), error happen\n");
	}
	DuplicateHandle(source_handle, (HANDLE)oldsocket, GetCurrentProcess(), &new_handle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	CloseHandle(source_handle);
	Log::log("GetProcessSocket(), pid: %d,old sock: %d, socket: %d\n",source_pid,oldsocket,new_handle);
	return (SOCKET)new_handle;
}

void WINAPI ExitProcessCallback(UINT uExitCode) {
	Log::slog("exit process called\n");

	//do the clean job here

	//if(need_dump_mesh) {
		YMesh::log_meshes("stat_mesh.log");
	//}

	cs.shut_down();
	Log::close();

	ExitProcessNext(uExitCode);
}

void RaiseToDebugP()
{
	HANDLE hToken;
	HANDLE hProcess = GetCurrentProcess();
	if ( OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
	{
		TOKEN_PRIVILEGES tkp;
		if ( LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid) )
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			BOOL bREt = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0) ;
		}
		CloseHandle(hToken);
	}    
}

