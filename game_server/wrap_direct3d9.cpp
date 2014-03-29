#include "game_server.h"
#include "wrap_direct3d9.h"
#include "wrap_direct3ddevice9.h"
#include "opcode.h"

IDirect3DDevice9 * cur_d3ddevice = NULL;
D3DCAPS9 d3d_caps;
bool GetDeviceCapsCalled = false;

WrapperDirect3D9::WrapperDirect3D9(IDirect3D9* ptr, int _id): m_d3d(ptr), id(_id) {}

int WrapperDirect3D9::GetID() {
	return this->id;
}

void WrapperDirect3D9::SetID(int id) {
	this->id = id;
}

WrapperDirect3D9* WrapperDirect3D9::GetWrapperD3D9(IDirect3D9* ptr) {
	WrapperDirect3D9* ret = (WrapperDirect3D9*)( m_list.GetDataPtr((PVOID)ptr) );
	if(ret == NULL) {
		Log::log("WrapperDirect3D9::GetWrapperD3D9(), ret is NULL ins_count:%d\n",ins_count);
		ret = new WrapperDirect3D9(ptr, ins_count++);
		m_list.AddMember(ptr, ret);
	}
	return ret;
}

STDMETHODIMP WrapperDirect3D9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3D9::QueryInterface() called\n");
	HRESULT hr = m_d3d->QueryInterface(riid,ppvObj);
	*ppvObj = this;
	return hr;
}

STDMETHODIMP_(ULONG) WrapperDirect3D9::AddRef(THIS) {
	Log::log("WrapperDirect3D9::AddRef() called\n");
	return m_d3d->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3D9::Release(THIS) {
	Log::log("WrapperDirect3D9::Release() called\n");
	return m_d3d->Release();
}

/*** IDirect3D9 methods ***/
STDMETHODIMP WrapperDirect3D9::RegisterSoftwareDevice(THIS_ void* pInitializeFunction) {
	Log::log("WrapperDirect3D9::RegisterSoftwareDevice() TODO!\n");
	return m_d3d->RegisterSoftwareDevice(pInitializeFunction);
}
STDMETHODIMP_(UINT) WrapperDirect3D9::GetAdapterCount(THIS) {
	UINT ret = m_d3d->GetAdapterCount();
	Log::log("WrapperDirect3D9::GetAdapterCount() TODO! ret:%d\n",ret);
	return ret;
}
STDMETHODIMP WrapperDirect3D9::GetAdapterIdentifier(THIS_ UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier) {
	Log::log("WrapperDirect3D9::GetAdapterIdentifier() TODO! Adapter:%d, Flags:%d\n", Adapter, Flags);
	return m_d3d->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}
STDMETHODIMP_(UINT) WrapperDirect3D9::GetAdapterModeCount(THIS_ UINT Adapter,D3DFORMAT Format) {
	UINT ret= m_d3d->GetAdapterModeCount(Adapter, Format);
	Log::log("WrapperDirect3D9::GetAdapterModeCount() TODO! Adapter:%d, Format:%d, ret:%d\n", Adapter, Format, ret);
	return ret;
}
STDMETHODIMP WrapperDirect3D9::EnumAdapterModes(THIS_ UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode) {
	Log::log("WrapperDirect3D9::EnumAdapterModes() TODO! Adapter:%d, Format:%d, Mode:%d\n", Adapter, Format, Mode);
	return m_d3d->EnumAdapterModes(Adapter, Format, Mode, pMode);
}
STDMETHODIMP WrapperDirect3D9::GetAdapterDisplayMode(THIS_ UINT Adapter,D3DDISPLAYMODE* pMode) {
	Log::log("WrapperDirect3D9::GetAdapterDisplayMode() TODO! Adapter:%d\n", Adapter);
	return m_d3d->GetAdapterDisplayMode(Adapter, pMode);
}
STDMETHODIMP WrapperDirect3D9::CheckDeviceType(THIS_ UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed) {
	Log::log("WrapperDirect3D9::CheckDeviceType() TODO! Adapter:%d, DeviceType:%d, Adapter Format:%d, BackBuffer Format:%d, windowed:%d\n",Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
	return m_d3d->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}
STDMETHODIMP WrapperDirect3D9::CheckDeviceFormat(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat) {
	Log::log("WrapperDirect3D9::CheckDeviceFormat() called! Adapter:%d, Device Type:%d, Adapter Format:%d, Usage:%d, Resource Type:%d, Check Format:%d\n", Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
	//Log::log("WrapperDirect3D9::CheckDeviceFormat() called\n");
	return m_d3d->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}
STDMETHODIMP WrapperDirect3D9::CheckDeviceMultiSampleType(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels) {
	Log::log("WrapperDirect3D9::CheckDeviceMultiSampleType() TODO! Adapter:%d, Device Type:%d, Surface Format:%d, windowed:%d, MultiSample Type:%d\n", Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType);
	return m_d3d->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}
STDMETHODIMP WrapperDirect3D9::CheckDepthStencilMatch(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat) {
	Log::log("WrapperDirect3D9::CheckDepthStencilMatch() TODO!\n");
	return m_d3d->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}
STDMETHODIMP WrapperDirect3D9::CheckDeviceFormatConversion(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat) {
	Log::log("WrapperDirect3D9::CheckDeviceFormatConversion() TODO! Adapter:%d, Device Type:%d, Source Format:%d, Target Format:%d\n", Adapter, DeviceType, SourceFormat, TargetFormat);
	return m_d3d->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

STDMETHODIMP WrapperDirect3D9::GetDeviceCaps(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps) {

	Log::log("WrapperDirect3D9::GetDeviceCaps called! adapetor:%d, device type:%d\n", Adapter, DeviceType);
	return m_d3d->GetDeviceCaps(Adapter, DeviceType, pCaps);
}
STDMETHODIMP_(HMONITOR) WrapperDirect3D9::GetAdapterMonitor(THIS_ UINT Adapter) {
	Log::log("WrapperDirect3D9::GetAdapterMonitor() TODO! Adapter:%d\n", Adapter);
	return m_d3d->GetAdapterMonitor(Adapter);
}

STDMETHODIMP WrapperDirect3D9::CreateDevice(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface) {

	Log::log("WrapperDirect3D9::CreateDevice() called\n");
	Log::log("Create Device start\n");
	Log::log("create device parameters: Adapter %d, DeviceType %d, bf %d dsf %d, backformat %d, backbuffer width:%d, backbuffer height:%d\n", Adapter, DeviceType, BehaviorFlags, pPresentationParameters->AutoDepthStencilFormat, pPresentationParameters->BackBufferFormat,pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

	IDirect3DDevice9* base_device = NULL;
	/*
	pPresentationParameters->Windowed = true;
	if(pPresentationParameters->BackBufferHeight < 600)
		pPresentationParameters->BackBufferHeight = 600;
	if(pPresentationParameters->BackBufferWidth < 800)
		pPresentationParameters->BackBufferWidth = 800;
		*/
	HRESULT hr = m_d3d->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &base_device);
	cur_d3ddevice = base_device;

	cs.begin_command(CreateDevice_Opcode, 0);
	cs.write_int(WrapperDirect3DDevice9::ins_count);
	cs.write_uint(Adapter);
	cs.write_uint(DeviceType);
	cs.write_uint(BehaviorFlags);
	cs.write_byte_arr((char*)(pPresentationParameters), sizeof(D3DPRESENT_PARAMETERS));
	cs.end_command();

	WrapperDirect3DDevice9::ins_count++;
	Log::log("Create Device End. With Device :%d ADDR:%d\n",WrapperDirect3DDevice9::ins_count,base_device);


	if(SUCCEEDED(hr)) {
		//MessageBox(NULL, "success", NULL, MB_OK);
		*ppReturnedDeviceInterface = static_cast<IDirect3DDevice9*>(new WrapperDirect3DDevice9(base_device, WrapperDirect3DDevice9::ins_count - 1));

		Log::log("WrapperDirect3D9::CreateDevice(), base_device=%d, device=%d\n", base_device, *ppReturnedDeviceInterface);
	}
	else {
		Log::log("Create Device Failed\n");
	}

	return hr;
}
