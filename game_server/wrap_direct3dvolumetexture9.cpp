#include "wrap_direct3dvolumetexture9.h"
#include "wrap_direct3ddevice9.h"
#include "wrap_direct3dvolume9.h"
#include "opcode.h"

int WrapperDirect3DVolumeTexture9::ins_count = 0;
HashSet WrapperDirect3DVolumeTexture9::m_list;
WrapperDirect3DVolumeTexture9::WrapperDirect3DVolumeTexture9(IDirect3DVolumeTexture9 * ptr, int _id): m_tex(ptr), id(_id){
	Log::log("WrapperDirect3DVolumeTexture9 constructor called!\n");
	m_list.AddMember(ptr,this);
}
int WrapperDirect3DVolumeTexture9::GetID(){
	return this->id;
}
void WrapperDirect3DVolumeTexture9::SetID(int id){
	this->id = id;
}

WrapperDirect3DVolumeTexture9 * WrapperDirect3DVolumeTexture9::GetWrapperTexture9(IDirect3DVolumeTexture9 * ptr){
	Log::log("WrapperDirect3DVolumeTexture9:: GetWrapperDirect3DVolumeTexture9 called!\n");
	WrapperDirect3DVolumeTexture9 * ret = NULL;
	ret = (WrapperDirect3DVolumeTexture9 *)m_list.GetDataPtr(ptr);
	if(ret == NULL){
		Log::log("ERROR:GetWrapperDirect3DVolumeTexture9 return NULL\n");
	}
	return ret;
}
IDirect3DVolumeTexture9 * WrapperDirect3DVolumeTexture9::GetVolumeTex9(){
	return this->m_tex;
}

/******* IUnknown Mehtods ***********/
STDMETHODIMP WrapperDirect3DVolumeTexture9::QueryInterface(THIS_ REFIID riid, void **ppvObj){
	Log::log("WrapperDirect3DVolumeTexture9::QueryInterface called!\n");
	HRESULT hr = this->m_tex->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	return hr;
}
STDMETHODIMP_(ULONG) WrapperDirect3DVolumeTexture9::AddRef(THIS){
	Log::log("WrapperDirect3DVolumeTexture9::AddRef called!\n");
	return this->m_tex->AddRef();
}

STDMETHODIMP_(ULONG) WrapperDirect3DVolumeTexture9::Release(THIS){
	Log::log("WrapperDirect3DVolumeTexture9::Release() called! id:%d\n",id);
	return this->m_tex->Release();
}

/*************** Methods **************/
STDMETHODIMP WrapperDirect3DVolumeTexture9::GetDevice(THIS_ IDirect3DDevice9** ppDevice)
{
	Log::log("WrapperDirect3DVolumeTexture9:: GetDevice called!\n");
	IDirect3DDevice9 * base_device = NULL;
	HRESULT hr = this->m_tex->GetDevice(ppDevice);
	//WrapperDirect3DVolumeTexture9 * ret = GetWrapperDirect3DVolumeTexture9(base_device);
	
	//*ppDevice = device;
	return hr;
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
	Log::log("WrapperDirect3DVolumeTexture9:: SetPrivateData()  TODO!!\n");
	HRESULT hr = this->m_tex->SetPrivateData(refguid, pData, SizeOfData, Flags);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
	Log::log("WrapperDirect3DVolumeTexture9::GetPrivateData()  TODO!!\n");
	HRESULT hr = this->m_tex->GetPrivateData(refguid, pData, pSizeOfData);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::FreePrivateData(THIS_ REFGUID refguid)
{
	Log::log("WrapperDirect3DVolumeTexture9::FreePrivateData()  TODO!!\n");
	HRESULT hr = this->m_tex->FreePrivateData(refguid);
	return hr;
}
STDMETHODIMP_(DWORD) WrapperDirect3DVolumeTexture9::SetPriority(THIS_ DWORD PriorityNew)
{
	Log::log("WrapperDirect3DVolumeTexture9::SetPriority()  TODO!!\n");
	DWORD hr = this->m_tex->SetPriority(PriorityNew);
	return hr;
}
STDMETHODIMP_(DWORD) WrapperDirect3DVolumeTexture9::GetPriority(THIS)
{
	Log::log("WrapperDirect3DVolumeTexture9:: GetPriority()  TODO!!\n");
	DWORD hr = this->m_tex->GetPriority();
	return hr;
}
STDMETHODIMP_(void) WrapperDirect3DVolumeTexture9::PreLoad(THIS)
{
	Log::log("WrapperDirect3DVolumeTexture9:: PreLoad()  TODO!!\n");
	this->m_tex->PreLoad();

}
STDMETHODIMP_(D3DRESOURCETYPE) WrapperDirect3DVolumeTexture9::GetType(THIS)
{
	Log::log("WrapperDirect3DVolumeTexture9:: GetType()  TODO!!\n");
	return this->m_tex->GetType();
}
STDMETHODIMP_(DWORD) WrapperDirect3DVolumeTexture9::SetLOD(THIS_ DWORD LODNew)
{
	Log::log("WrapperDirect3DVolumeTexture9::SetLOD()  TODO!\n");
	DWORD hr = this->m_tex->SetLOD(LODNew);
	return hr;
}
STDMETHODIMP_(DWORD) WrapperDirect3DVolumeTexture9::GetLOD(THIS)
{
	Log::log("WrapperDirect3DVolumeTexture9::GetLOD()  TODO!\n");
	DWORD hr = this->m_tex->GetLOD();
	return hr;
}
STDMETHODIMP_(DWORD) WrapperDirect3DVolumeTexture9::GetLevelCount(THIS)
{
	Log::log("WrapperDirect3DVolumeTexture9:: GetLevelCount()  TODO!\n");
	DWORD hr = this->m_tex->GetLevelCount();
	return hr;
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::SetAutoGenFilterType(THIS_ D3DTEXTUREFILTERTYPE FilterType)
{
	Log::log("WrapperDirect3DVolumeTexture9::SetAutoGenFilterType()  TODO!\n");
	HRESULT hr = this->m_tex->SetAutoGenFilterType(FilterType);
	return hr;
}
STDMETHODIMP_(D3DTEXTUREFILTERTYPE) WrapperDirect3DVolumeTexture9::GetAutoGenFilterType(THIS)
{
	Log::log("WrapperDirect3DVolumeTexture9::GetAutoGenFilterType()  TODO!!\n");
	return this->m_tex->GetAutoGenFilterType();
}
STDMETHODIMP_(void) WrapperDirect3DVolumeTexture9::GenerateMipSubLevels(THIS)
{
	Log::log("WrapperDirect3DVolumeTexture9::GenerateMipSubLevels()  TODO!!\n");
	this->m_tex->GenerateMipSubLevels();
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::GetLevelDesc(THIS_ UINT Level,D3DVOLUME_DESC *pDesc)
{
	Log::log("WrapperDirect3DVolumeTexture9::GetLevelDesc()  TODO!!\n");
	HRESULT hr = this->m_tex->GetLevelDesc(Level, pDesc);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::GetVolumeLevel(THIS_ UINT Level,IDirect3DVolume9** ppVolumeLevel)
{
	Log::log("WrapperDirect3DVolumeTexture9::GetVolumeLevel()  TODO!!\n");
	IDirect3DVolume9 * tpVolumeLevel = NULL;
	HRESULT hr = this->m_tex->GetVolumeLevel(Level, &tpVolumeLevel);
	WrapperDirect3DVolume9 *ret = WrapperDirect3DVolume9::GetWrapperDirect3DVolume9(tpVolumeLevel);
	if(ret ==NULL){
		Log::log("WrapperDirect3DVolumeTexture9::GetVolumeLevel  get NULL!\n");
	}
	*ppVolumeLevel =dynamic_cast<IDirect3DVolume9*>(ret);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::LockBox(THIS_ UINT Level,D3DLOCKED_BOX* pLockedVolume,CONST D3DBOX* pBox,DWORD Flags)
{
	Log::log("WrapperDirect3DVolumeTexture9::LockBox()  TODO!!\n");
	HRESULT hr = this->m_tex->LockBox(Level, pLockedVolume, pBox, Flags);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::UnlockBox(THIS_ UINT Level)
{
	Log::log("WrapperDirect3DVolumeTexture9::UnlockBox()  TODO!!\n");
	HRESULT hr = this->m_tex->UnlockBox(Level);
	return hr;
}
STDMETHODIMP WrapperDirect3DVolumeTexture9::AddDirtyBox(THIS_ CONST D3DBOX* pDirtyBox)
{
	Log::log("WrapperDirect3DVolumeTexture9::AddDirtyBox()  TODO!!\n");
	HRESULT hr = this->m_tex->AddDirtyBox(pDirtyBox);
	return hr;
}