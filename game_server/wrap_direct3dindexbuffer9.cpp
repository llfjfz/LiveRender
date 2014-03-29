#include "game_server.h"
#include "wrap_direct3dindexbuffer9.h"
#include "wrap_direct3ddevice9.h"
#include "opcode.h"
static int max_ib = 0;
WrapperDirect3DIndexBuffer9::WrapperDirect3DIndexBuffer9(IDirect3DIndexBuffer9* ptr, int _id, int _length): m_ib(ptr), id(_id), length(_length) {
	Log::log("WrapperDirect3DIndexBuffer9 constructor, size:%d\n", length);
	cache_buffer = new char[_length];
	ram_buffer = new char[_length];
	isFirst = true;
	isSent= false;
	SetFirst = false;
	changed = false;

	//y_mesh_ = NULL;

	m_list.AddMember(ptr, this);
}

WrapperDirect3DIndexBuffer9::WrapperDirect3DIndexBuffer9(IDirect3DIndexBuffer9* ptr, int _id): m_ib(ptr), id(_id), length(0) {
	Log::log("WrapperDirect3DIndexBuffer9 constructor, size:0\n");
	isFirst = true;
	isSent= false;
	changed = false;

	//y_mesh_ = NULL;

	m_list.AddMember(ptr, this);
}

int mesh_arr[4];

YMesh* WrapperDirect3DIndexBuffer9::get_y_mesh(int vb_id, INT BaseVertexIndex, UINT startIndex) {

	memset(mesh_arr, 0, sizeof mesh_arr);
	mesh_arr[0] = vb_id;
	mesh_arr[1] = BaseVertexIndex;
	mesh_arr[2] = startIndex;

	YashNode* nd = mesh_table_.look_up( Slice((char*)mesh_arr, 12) );

	if(nd == NULL) return NULL;
	else return (YMesh*)(nd->val);
}

void WrapperDirect3DIndexBuffer9::put_y_mesh(int vb_id, INT BaseVertexIndex, UINT startIndex, YMesh* y_mesh) {
	memset(mesh_arr, 0, sizeof mesh_arr);
	mesh_arr[0] = vb_id;
	mesh_arr[1] = BaseVertexIndex;
	mesh_arr[2] = startIndex;

	mesh_table_.insert( Slice((char*)mesh_arr, 12), y_mesh );
}

IDirect3DIndexBuffer9* WrapperDirect3DIndexBuffer9::GetIB9() {
	return m_ib;
}

void WrapperDirect3DIndexBuffer9::SetID(int id) {
	this->id = id;
}

int WrapperDirect3DIndexBuffer9::GetID() {
	return id;
}

int WrapperDirect3DIndexBuffer9::GetLength() {
	return length;
}

WrapperDirect3DIndexBuffer9* WrapperDirect3DIndexBuffer9::GetWrapperIndexedBuffer9(IDirect3DIndexBuffer9* base_indexed_buffer) {
	WrapperDirect3DIndexBuffer9* ret = (WrapperDirect3DIndexBuffer9*)( m_list.GetDataPtr( (PVOID)base_indexed_buffer ) );

	if(ret == NULL) {
		Log::log("WrapperDirect3DIndexBuffer9::GetWrapperIndexedBuffer9() ret is NULL\n");
	}

	return ret;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DIndexBuffer9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3DIndexBuffer9::QueryInterface() called, base_index=%d, this=%d, ppvObj=%d\n", m_ib, this, *ppvObj);
	HRESULT hr = m_ib->QueryInterface(riid, ppvObj);
	*ppvObj  = this;
	Log::log("WrapperDirect3DIndexBuffer9::QueryInterface() end, base_index=%d, this=%d, ppvObj=%d\n", m_ib, this, *ppvObj);
	return hr;
}
STDMETHODIMP_(ULONG) WrapperDirect3DIndexBuffer9::AddRef(THIS) { return m_ib->AddRef(); }
STDMETHODIMP_(ULONG) WrapperDirect3DIndexBuffer9::Release(THIS) { 
	Log::log("WrapperDirect3DIndexBuffer9::Release() called! id:%d\n", this->id);
	return m_ib->Release(); }

/*** IDirect3DResource9 methods ***/
STDMETHODIMP WrapperDirect3DIndexBuffer9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) { 
	Log::log("WrapperDirect3DIndexBuffer9::GetDevice called!\n");
	IDirect3DDevice9* base = NULL;
	HRESULT hr = this->m_ib->GetDevice(&base);
	WrapperDirect3DDevice9 * ret = WrapperDirect3DDevice9::GetWrapperDevice9(base);
	if(ret == NULL){
		Log::log("WrapperDirect3DIndexBuffer9::GetDevice return NULL!\n");
	}
	*ppDevice = ret;
	return hr;
	//return m_ib->GetDevice(ppDevice); 
}

STDMETHODIMP WrapperDirect3DIndexBuffer9::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) {
	Log::log("WrapperDirect3DIndexBuffer9::SetPrivateData() TODO\n");
	return m_ib->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

STDMETHODIMP WrapperDirect3DIndexBuffer9::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) { return m_ib->GetPrivateData(refguid, pData, pSizeOfData); }
STDMETHODIMP WrapperDirect3DIndexBuffer9::FreePrivateData(THIS_ REFGUID refguid) { return m_ib->FreePrivateData(refguid); }

STDMETHODIMP_(DWORD) WrapperDirect3DIndexBuffer9::SetPriority(THIS_ DWORD PriorityNew) {
	Log::log("WrapperDirect3DIndexBuffer9::SetPriority() TODO\n");
	return m_ib->SetPriority(PriorityNew);
}

STDMETHODIMP_(DWORD) WrapperDirect3DIndexBuffer9::GetPriority(THIS) { return m_ib->GetPriority(); }

STDMETHODIMP_(void) WrapperDirect3DIndexBuffer9::PreLoad(THIS) {
	Log::log("WrapperDirect3DIndexBuffer9::PreLoad() TODO\n");
	return m_ib->PreLoad();
}

STDMETHODIMP_(D3DRESOURCETYPE) WrapperDirect3DIndexBuffer9::GetType(THIS) { return m_ib->GetType(); }

STDMETHODIMP WrapperDirect3DIndexBuffer9::Lock(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags) {
	Log::log("WrapperDirect3DIndexBuffer9::Lock(),id:%d, OffestToLock=%d, SizeToLock=%d, Flags=%d, size:%d\n",this->id, OffsetToLock, SizeToLock, Flags,this->length);

	void * tmp = NULL;

	HRESULT hr = m_ib->Lock(OffsetToLock, SizeToLock, &tmp, Flags);

	m_LockData.OffsetToLock = OffsetToLock;
	m_LockData.SizeToLock = SizeToLock;
	m_LockData.Flags = Flags;
	if(SizeToLock == 0) m_LockData.SizeToLock = length - OffsetToLock;
	m_LockData.pRAMBuffer = tmp;

	*ppbData = tmp;
	this->changed = true;

	return hr;
}

STDMETHODIMP WrapperDirect3DIndexBuffer9::Unlock(THIS) {
	Log::log("WrapperDirect3DIndexBuffer9::Unlock(),id:%d, UnlockSize=%d Bytes\n",id, m_LockData.SizeToLock);

	return m_ib->Unlock();
}

void WrapperDirect3DIndexBuffer9::read_data_from_buffer(char** ptr, int offest, int size) {
	if(size == 0) size = length - offest;
	if(!ram_buffer) {
		ram_buffer = (char*)(malloc(this->length));
		m_LockData.pRAMBuffer = ram_buffer;
	}

	void* p = NULL;
	Lock(offest, size, &p, 2048);
	memcpy(ram_buffer, p, size);
	Unlock();

	*ptr = ram_buffer;
}

void WrapperDirect3DIndexBuffer9::write_data_to_buffer(char* ptr, int offest, int size) {
	if(size == 0) size = length - offest;
	void* p = NULL;
	Lock(offest, size, &p, 2048);
	memcpy((char*)p, ptr, size);
	Unlock();
}

STDMETHODIMP WrapperDirect3DIndexBuffer9::GetDesc(THIS_ D3DINDEXBUFFER_DESC *pDesc) { return m_ib->GetDesc(pDesc); }

int WrapperDirect3DIndexBuffer9::PrepareIndexBuffer() {

	
	if(isFirst) {
		memset(cache_buffer, 0, length);
		isFirst = false;
	}

	if(!changed){
		Log::log("WrapperDirect3DIndexBuffer9 not changed!\n");
		return 1;
	}
	cs.begin_command(IndexBufferUnlock_Opcode, GetID());
	//void * p = NULL;
	double tick_s = 0.0, tick_e = 0.0, tick_a  = 0.0;
	tick_s = GetTickCount();
	//Lock(0,0,&p,D3DLOCK_DISCARD);
	//Lock(0,0,&p,D3DLOCK_NOSYSLOCK);

	char* p = NULL;
	read_data_from_buffer(&p, 0, 0);
	
	cs.write_uint(m_LockData.OffsetToLock);
	cs.write_uint(m_LockData.SizeToLock);
	cs.write_uint(m_LockData.Flags);

	cs.write_int(CACHE_MODE_DIFF);
	
	int stride = (Format == D3DFMT_INDEX16) ? 2 : 4;
	cs.write_char(stride);

	int last = 0, cnt = 0, c_len = 0, size = 0;
	int base = m_LockData.OffsetToLock;
	tick_e = GetTickCount();
	for(int i=0; i<m_LockData.SizeToLock; ++i) {
		
		if( cache_buffer[base + i] ^ *((char*)(m_LockData.pRAMBuffer) + i) ) {
			int d = i - last;
			cs.write_int(d);
			last = i;
			cs.write_char( *((char*)(m_LockData.pRAMBuffer) + i) );
			cnt++;
			cache_buffer[base + i] = *((char*)(m_LockData.pRAMBuffer) + i);
			//Log::log("WrapperDirect3DVertexBuffer9::PrepareVertexBuffer, last=%d, d=%d\n", last, d);
		}
	}
	int neg = (1<<28)-1;
	cs.write_int(neg);
	tick_a = GetTickCount();
	Log::log("\tLock index buffer:%f, cache time:%f\n", tick_e - tick_s, tick_a- tick_s);
	//获取当前这条指令的长度
	c_len = cs.get_command_length();

	
	if(c_len > m_LockData.SizeToLock) {
		Log::log("WrapperDirect3DIndexBuffer9::PrepareIndexBuffer(), cancel command, id=%d\n", id);
		cs.cancel_command();

		c_len = m_LockData.SizeToLock;

		Log::log("offest=%d, c_len=%d\n", m_LockData.OffsetToLock, c_len);

		cs.begin_command(IndexBufferUnlock_Opcode, GetID());
		cs.write_uint(m_LockData.OffsetToLock);
		cs.write_uint(m_LockData.SizeToLock);
		cs.write_uint(m_LockData.Flags);

		cs.write_int(CACHE_MODE_COPY);
		cs.write_char(stride);
		cs.write_byte_arr((char*)(m_LockData.pRAMBuffer), c_len);

		//Log::slog("ib: id:%d, size:%d\n", this->id, this->length);
		if(c_len > max_ib){
			max_ib = c_len;
			Log::log("max ib:%d\n", max_ib);
		}
		Log::log("ib  id:%d, changed %d, org:%d\n",this->id, c_len,this->length);
		cs.end_command();
	}
	else {
		if(cnt > 0) {
			Log::log("ib  id:%d, changed %d, org:%d\n",this->id, c_len,this->length);
			cs.end_command();
		}
		else cs.cancel_command();
	}
	
	this->changed = false;
	return (cnt > 0);
}
