#include "game_server.h"
#include "wrap_direct3dvertexbuffer9.h"
#include "opcode.h"
//#define FLOAT_COMPRESS
#define INT_COMPRESS
#define COMPRESS_TO_DWORD

WrapperDirect3DVertexBuffer9::WrapperDirect3DVertexBuffer9(IDirect3DVertexBuffer9* ptr, int _id, int _length): m_vb(ptr), isLock(false), id(_id), length(_length) {
	Log::log("WrapperDirect3DVertexBuffer9 constructor called, id=%d, length=%d\n", _id, _length);
	cache_buffer = new char[_length];
	ram_buffer = new char[_length];
	m_LockData.pRAMBuffer = ram_buffer;
	isFirst = true;
	isSent = false;
	SetFirst = false;
	changed = false;
	decl = NULL;
	this->maxFlag = false;
	stride = -1;

	readed_ = false;

	y_mesh_ = NULL;
	owner_mesh_ = NULL;

	m_list.AddMember(ptr, this);
}

WrapperDirect3DVertexBuffer9::WrapperDirect3DVertexBuffer9(IDirect3DVertexBuffer9* ptr, int _id): m_vb(ptr), isLock(false), id(_id), length(0) {
	//cache_buffer = new char[_length];
	isFirst = true;
	isSent = false;
	this->maxFlag = false;
	changed = false;
	decl = NULL;
	stride = -1;

	y_mesh_ = NULL;

	m_list.AddMember(ptr, this);
}

LPDIRECT3DVERTEXBUFFER9 WrapperDirect3DVertexBuffer9::GetVB9() {
	return m_vb;
}

void WrapperDirect3DVertexBuffer9::SetId(int id) {
	this->id = id;
}

int WrapperDirect3DVertexBuffer9::GetId() {
	return this->id;
}

int WrapperDirect3DVertexBuffer9::GetLength() {
	return this->length;
}

WrapperDirect3DVertexBuffer9* WrapperDirect3DVertexBuffer9::GetWrapperVertexBuffer9(IDirect3DVertexBuffer9* ptr) {
	WrapperDirect3DVertexBuffer9* ret = (WrapperDirect3DVertexBuffer9*)( m_list.GetDataPtr( ptr ) );
	if(ret == NULL) {
		Log::log("WrapperDirect3DVertexBuffer9::GetWrapperVertexBuffer9(), ret is NULL\n");
	}
	return ret;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DVertexBuffer9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3DVertexBuffer9::QueryInterface() called, base_vb=%d, this=%d, ppvObj=%d\n", m_vb, this, *ppvObj);
	HRESULT hr = m_vb->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	Log::log("WrapperDirect3DVertexBuffer9::QueryInterface() called, base_vb=%d, this=%d, ppvObj=%d\n", m_vb, this, *ppvObj);
	return hr;
};
STDMETHODIMP_(ULONG) WrapperDirect3DVertexBuffer9::AddRef(THIS) {
	return m_vb->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3DVertexBuffer9::Release(THIS) {
	Log::log("WrapperDirect3DVertexBuffer9::Release() called! id:%d\n", this->id);
	return m_vb->Release();
}

/*** IDirect3DResource9 methods ***/
STDMETHODIMP WrapperDirect3DVertexBuffer9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	Log::log("WrapperDirect3DVertexBuffer9::GetDevice() TODO\n");
	return m_vb->GetDevice(ppDevice);
}
STDMETHODIMP WrapperDirect3DVertexBuffer9::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) {
	Log::log("WrapperDirect3DVertexBuffer9::SetPrivateData() TODO\n");
	return m_vb->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
STDMETHODIMP WrapperDirect3DVertexBuffer9::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) {
	return m_vb->GetPrivateData(refguid, pData, pSizeOfData);
}
STDMETHODIMP WrapperDirect3DVertexBuffer9::FreePrivateData(THIS_ REFGUID refguid) {
	return m_vb->FreePrivateData(refguid);
}
STDMETHODIMP_(DWORD) WrapperDirect3DVertexBuffer9::SetPriority(THIS_ DWORD PriorityNew) {
	Log::log("WrapperDirect3DVertexBuffer9::SetPriority() TODO\n");
	return m_vb->SetPriority(PriorityNew);
}
STDMETHODIMP_(DWORD) WrapperDirect3DVertexBuffer9::GetPriority(THIS) {
	return m_vb->GetPriority();
}
STDMETHODIMP_(void) WrapperDirect3DVertexBuffer9::PreLoad(THIS) {
	Log::log("WrapperDirect3DVertexBuffer9::PreLoad() TODO\n");
	return m_vb->PreLoad();
}
STDMETHODIMP_(D3DRESOURCETYPE) WrapperDirect3DVertexBuffer9::GetType(THIS) {
	return m_vb->GetType();
}

STDMETHODIMP WrapperDirect3DVertexBuffer9::Lock(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags) {
	Log::log("WrapperDirect3DVertexBuffer9::Lock(), id=%d, length=%d, offest=%d, size_to_lock=%d, flag=%d\n",this->id, length, OffsetToLock, SizeToLock, Flags);

	void * tmp = NULL;

	HRESULT hr = m_vb->Lock(OffsetToLock, SizeToLock, &tmp, Flags);

	Log::log("WrapperDirect3DVertexBuffer9::Lock() end\n");
	
	m_LockData.OffsetToLock = OffsetToLock;
	m_LockData.SizeToLock = SizeToLock;
	m_LockData.Flags = Flags;

	if(SizeToLock == 0) m_LockData.SizeToLock = length - OffsetToLock;

	*ppbData = tmp;
	changed = true;
	readed_ = false;
	return hr;
}

STDMETHODIMP WrapperDirect3DVertexBuffer9::Unlock(THIS) {
	Log::log("WrapperDirect3DVertexBuffer9::Unlock(), id:%d, UnlockSize=%d Bytes\n", this->id,m_LockData.SizeToLock);
	return m_vb->Unlock();
}

void WrapperDirect3DVertexBuffer9::read_data_from_buffer(char** ptr, int offest, int size) {
	Log::log("WrapperDirect3DVertexBuffer9::read_data_from_buffer() called\n");
	if(size == 0) size = length - offest;
	if(!ram_buffer) {
		ram_buffer = (char*)(malloc(this->length));
		m_LockData.pRAMBuffer = ram_buffer;
	}

	if(readed_) {
		*ptr = ram_buffer;
		return;
	}

	void* p = NULL;
	int sv_change = changed;
	Lock(offest, size, &p, 2048);
	memcpy(ram_buffer, p, size);
	Unlock();

	changed = sv_change;

	readed_ = true;

	*ptr = ram_buffer;
}

void WrapperDirect3DVertexBuffer9::write_data_to_buffer(char* ptr, int offest, int size) {
	Log::log("WrapperDirect3DVertexBuffer9::write_data_to_buffer() called\n");
	if(size == 0) size = length - offest;
	void* p = NULL;
	Lock(offest, size, &p, 2048);
	memcpy((char*)p, ptr, size);
	Unlock();
}

STDMETHODIMP WrapperDirect3DVertexBuffer9::GetDesc(THIS_ D3DVERTEXBUFFER_DESC *pDesc) {
	//Log::log("WrapperDirect3DVertexBuffer9::GetDesc() TODO\n");
	return m_vb->GetDesc(pDesc);
}
