#include "wrap_direct3dvolume9.h"
#include "wrap_direct3ddevice9.h"
#include "opcode.h"
int WrapperDirect3DVolume9::ins_count = 0;
HashSet WrapperDirect3DVolume9::m_list;

WrapperDirect3DVolume9::WrapperDirect3DVolume9(IDirect3DVolume9 *ptr, int _id):m_d3dvolume(ptr),id(_id){
	Log::log("WrapperDirect3DVolume9 constructor called!\n");
	m_list.AddMember(ptr,this);
}

int WrapperDirect3DVolume9::GetID(){
	return this->id;
}
void WrapperDirect3DVolume9::SetID(int id){
	this->id =id;
}

WrapperDirect3DVolume9 * WrapperDirect3DVolume9::GetWrapperDirect3DVolume9(IDirect3DVolume9 *ptr){
	Log::log("WrapperDirect3DVolume9:: GetWrapperDirect3DVolume9 called!\n");
	WrapperDirect3DVolume9 * ret = NULL;
	ret = (WrapperDirect3DVolume9 *)m_list.GetDataPtr(ptr);
	if(ret == NULL){
		Log::log("ERROR:WrapperDirect3DVolume9:: GetWrapperDirect3DVolume9 return NULL!\n");
	}
	return ret;
}
IDirect3DVolume9 * WrapperDirect3DVolume9::GetIDirect3DVolume9(){
	return this->m_d3dvolume;
}

/********************** IUnknown Methods *************/
STDMETHODIMP WrapperDirect3DVolume9::QueryInterface(THIS_ REFIID riid, void **ppvObj){
	Log::log("WrapperDirect3DVolume9::QueryInterface called!\n");
	HRESULT hr =this->m_d3dvolume->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	return hr;
}
STDMETHODIMP_(ULONG) WrapperDirect3DVolume9::AddRef(THIS){
	Log::log("WrapperDirect3DVolume9::AddRef called!\n");
	return this->m_d3dvolume->AddRef();
}

STDMETHODIMP_(ULONG) WrapperDirect3DVolume9::Release(THIS){
	Log::log("WrapperDirect3DVolume9::Release called!\n");
	return this->m_d3dvolume->Release();
}

/*************** Methods *************************/
STDMETHODIMP WrapperDirect3DVolume9::GetDevice(THIS_ IDirect3DDevice9** ppDevice)
{
	Log::log("WrapperDirect3DVolume9::GetDevice  called!\n");
	IDirect3DDevice9* base_device = NULL;
	HRESULT hr =this->m_d3dvolume->GetDevice(&base_device);
	WrapperDirect3DDevice9* ret =  WrapperDirect3DDevice9::GetWrapperDevice9(base_device);
	*ppDevice = ret;
	return hr;
}
STDMETHODIMP WrapperDirect3DVolume9::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
	Log::log("WrapperDirect3DVolume9::SetPrivateData  called!\n");
	HRESULT hr =this->m_d3dvolume->SetPrivateData(refguid, pData, SizeOfData, Flags);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolume9::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
	Log::log("WrapperDirect3DVolume9::GetPrivateData  called!\n");
	HRESULT hr =this->m_d3dvolume->GetPrivateData(refguid, pData, pSizeOfData);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolume9::FreePrivateData(THIS_ REFGUID refguid)
{
	Log::log("WrapperDirect3DVolume9::FreePrivateData  called!\n");
	HRESULT hr =this->m_d3dvolume->FreePrivateData(refguid);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolume9::GetContainer(THIS_ REFIID riid,void** ppContainer)
{
	Log::log("WrapperDirect3DVolume9::GetContainer  called!\n");
	HRESULT hr =this->m_d3dvolume->GetContainer(riid, ppContainer);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolume9::GetDesc(THIS_ D3DVOLUME_DESC *pDesc)
{
	Log::log("WrapperDirect3DVolume9::GetDesc  called!\n");
	HRESULT hr =this->m_d3dvolume->GetDesc(pDesc);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolume9::LockBox(THIS_ D3DLOCKED_BOX * pLockedVolume,CONST D3DBOX* pBox,DWORD Flags)
{
	Log::log("WrapperDirect3DVolume9::LockBox  called!\n");
	HRESULT hr =this->m_d3dvolume->LockBox(pLockedVolume, pBox, Flags);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolume9::UnlockBox(THIS)
{
	Log::log("WrapperDirect3DVolume9::UnlockBox  called!\n");
	HRESULT hr =this->m_d3dvolume->UnlockBox();
	return hr;
}
