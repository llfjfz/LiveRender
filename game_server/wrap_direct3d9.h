#ifndef __WRAP_DIRECT3D9__
#define __WRAP_DIRECT3D9__

#include "game_server.h"
extern CommandServer cs;

class WrapperDirect3D9 : public IDirect3D9 {
private:
	IDirect3D9* m_d3d;
	int id;
	static HashSet m_list;
public:
	WrapperDirect3D9(IDirect3D9* m_ptr, int _id);
	static int ins_count;

	static WrapperDirect3D9* GetWrapperD3D9(IDirect3D9* ptr);
	int GetID();
	void SetID(int id);

public:
	COM_METHOD(HRESULT, QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	COM_METHOD(ULONG, AddRef)(THIS);
	COM_METHOD(ULONG, Release)(THIS);

	/*** IDirect3D9 methods ***/
	COM_METHOD(HRESULT, RegisterSoftwareDevice)(THIS_ void* pInitializeFunction);
	COM_METHOD(UINT, GetAdapterCount)(THIS);
	COM_METHOD(HRESULT, GetAdapterIdentifier)(THIS_ UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier);
	COM_METHOD(UINT, GetAdapterModeCount)(THIS_ UINT Adapter,D3DFORMAT Format);
	COM_METHOD(HRESULT, EnumAdapterModes)(THIS_ UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode);
	COM_METHOD(HRESULT, GetAdapterDisplayMode)(THIS_ UINT Adapter,D3DDISPLAYMODE* pMode);
	COM_METHOD(HRESULT, CheckDeviceType)(THIS_ UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed) ;
	COM_METHOD(HRESULT, CheckDeviceFormat)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat) ;
	COM_METHOD(HRESULT, CheckDeviceMultiSampleType)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels) ;
	COM_METHOD(HRESULT, CheckDepthStencilMatch)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
	COM_METHOD(HRESULT, CheckDeviceFormatConversion)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat) ;
	COM_METHOD(HRESULT, GetDeviceCaps)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps) ;
	COM_METHOD(HMONITOR, GetAdapterMonitor)(THIS_ UINT Adapter) ;
	COM_METHOD(HRESULT, CreateDevice)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface) ;
};

#endif


