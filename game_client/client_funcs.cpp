#include "game_client.h"

IDirect3D9* g_d3d = NULL;
HWND window_handle = NULL;
IDirect3DDevice9* cur_device = NULL;

extern D3DDISPLAYMODE displayMode;
extern D3DPRESENT_PARAMETERS d3dpp;

void* device_list[Max_Obj_Cnt];
void* vb_list[Max_Obj_Cnt];
void* ib_list[Max_Obj_Cnt];
void* vd_list[Max_Obj_Cnt];
void* vs_list[Max_Obj_Cnt];
void* ps_list[Max_Obj_Cnt];
void* tex_list[Max_Obj_Cnt];
void* sb_list[Max_Obj_Cnt];
void* ctex_list[Max_Obj_Cnt];
void* chain_list[Max_Obj_Cnt];
void* surface_list[Max_Obj_Cnt];


#define GET_DEVICE() \
	cur_device = (LPDIRECT3DDEVICE9)(device_list[obj_id]);

HRESULT FakeDCreateWindow() {
	Log::log("FakedCreateWindow() called\n");
	TCHAR szAppName[]= TEXT("HelloWin");
	TCHAR szClassName[]= TEXT("HelloWinClass");

	DWORD dwExStyle = cc.read_uint();
	DWORD dwStyle = cc.read_uint();
	int x= cc.read_int();
	int y = cc.read_int();
	int nWidth = cc.read_int();
	int nHeight = cc.read_int();

	static bool window_created = false;

	if(!window_created)
		init_window(nWidth, nHeight, dwExStyle, dwStyle);
	window_created = true;
	return D3D_OK;
}

HRESULT FakeDDirectCreate(){
	Log::log("server Direct3DCreate9 called\n");

	g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_d3d)
		return D3D_OK;
	else{
		Log::log("Direct3DCreate9 failed!\n");
		return D3D_OK;
	}
}

HRESULT FakedCreateDevice() {
	//printf("FakedCreateDevice called\n");
	Log::log("FakedCreateDevice called\n");

	return client_init();
}

HRESULT FakedBeginScene() {
	//printf("FakedBeginScene called\n");
	Log::log("FakedBeginScene called\n");

	GET_DEVICE();
	return cur_device->BeginScene();
}

HRESULT FakedEndScene() {
	//printf("FakedEndScene called\n");
	Log::log("FakedEndScene called\n");
	GET_DEVICE();
	return cur_device->EndScene();
}

HRESULT FakedClear() {
	//printf("Faked Clear called\n");
	DWORD count = cc.read_uint();
	D3DRECT pRects;

	bool is_null = cc.read_char();
	if(!is_null) {
		cc.read_byte_arr((char*)(&pRects), sizeof(pRects));
	}

	DWORD Flags = cc.read_uint();
	D3DCOLOR color = cc.read_uint();
	float Z = cc.read_float();
	DWORD stencil = cc.read_uint();

	Log::log("Faked Clear called(), Color=0x%08x\n", color);

	GET_DEVICE();
	if(!is_null)
		return cur_device->Clear(count, &pRects, Flags, color, Z, stencil);
	else
		return cur_device->Clear(count, NULL, Flags, color, Z, stencil);
}

HRESULT FakedPresent() {
	Log::log("Faked Present called\n");

	static float last_present = 0;
	float now_present = timeGetTime();

	Log::slog("FakedPresent(), present gap=%.4f\n", now_present - last_present);
	last_present = now_present;

	float t1 = timeGetTime();

	const RECT* pSourceRect = (RECT*)(cc.read_int());
	const RECT* pDestRect = (RECT*)(cc.read_int());
	HWND hDestWindowOverride = (HWND)(cc.read_int());
	const RGNDATA* pDirtyRegion = (RGNDATA*)(cc.read_int());

	//return cur_device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	GET_DEVICE();

	assert(cur_device);
	HRESULT hr = cur_device->Present(NULL, NULL, NULL, NULL);

	float t2 = timeGetTime();
	Log::log("FakedPresent(), del_time=%.4f\n", t2 - t1);

	return hr;
}

HRESULT FakedSetTransform() {
	//D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix
	Log::log("FakedSetTransform called\n");

	short st = cc.read_short();
	D3DTRANSFORMSTATETYPE state = (D3DTRANSFORMSTATETYPE)st;
	unsigned short mask = cc.read_ushort();

	D3DMATRIX mat;
	int out_len = 0;
	for(int i=0; i<4; ++i) {
		for(int j=0; j<4; ++j) {
			if(mask & (1 << (i * 4 + j))) {
				mat.m[i][j] = cc.read_float();
			}
			else
				mat.m[i][j] = 0.0f;
		}
	}
	GET_DEVICE();

	return cur_device->SetTransform(state, &mat);
}

HRESULT FakedSetRenderState() {
	Log::log("FakedSetRenderState called\n");
	//D3DRENDERSTATETYPE State,DWORD Value

	D3DRENDERSTATETYPE State = (D3DRENDERSTATETYPE)(cc.read_uint());
	DWORD Value = cc.read_uint();

	GET_DEVICE();
	return cur_device->SetRenderState(State, Value);
}

HRESULT FakedSetStreamSource() {
	Log::log("FakedSetStreamSource called\n");
	//UINT StreamNumber,UINT vb_id,UINT OffsetInBytes,UINT Stride

	int StreamNumber, vb_id, OffestInBytes, Stride;


	StreamNumber = cc.read_uint();
	vb_id = cc.read_int();
	OffestInBytes = cc.read_uint();
	Stride = cc.read_uint();

	//Log::slog("FakedSetStreamSource(), hit_cnt=%d, set_cnt=%d, ratio=%.3f\n", sss_cache->hit_cnt, sss_cache->set_cnt, sss_cache->hit_cnt * 1.0 / (sss_cache->hit_cnt + sss_cache->set_cnt));

	if(vb_id == -1) {
		return cur_device->SetStreamSource(StreamNumber, NULL, OffestInBytes, Stride);
	}
	else{
		Log::log("FakedSetStreamSource: StreamNum:%d, vb_id:%d, Offset:%d, Stride:%d\n",StreamNumber,vb_id, OffestInBytes , Stride);
	}

	ClientVertexBuffer9* svb = NULL;
	svb = (ClientVertexBuffer9*)(vb_list[vb_id]);
	IDirect3DVertexBuffer9 * vb = NULL;
	if(svb == NULL){
		Log::log("FakedSetStreamSource: vertexbuffer NULL!\n");
	}
	else{
		vb = svb->GetVB();
		svb->stride = Stride;
	}

	return cur_device->SetStreamSource(StreamNumber, vb, OffestInBytes, Stride);
}

HRESULT FakedSetFVF() {
	//DWORD FVF
	Log::log("FakedSetFVF called\n");

	DWORD FVF = cc.read_uint();

	GET_DEVICE();
	return cur_device->SetFVF(FVF);
}

HRESULT FakedDrawPrimitive() {
	Log::log("FakedDrawPrimitive called\n");
	//D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount

	char type = cc.read_char();
	int StartVertex = 0, PrimitiveCount = 0;
	D3DPRIMITIVETYPE PrimitiveType = (D3DPRIMITIVETYPE)type;
	StartVertex = cc.read_int();
	PrimitiveCount = cc.read_int();


	GET_DEVICE();
	return cur_device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT FakedDrawIndexedPrimitive() {
	Log::log("FakedDrawIndexedPrimitive called\n");
	//D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount
	GET_DEVICE();


	D3DPRIMITIVETYPE Type = (D3DPRIMITIVETYPE)(cc.read_char());
	int BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount;
	BaseVertexIndex = cc.read_int();
	MinVertexIndex = cc.read_int();
	NumVertices = cc.read_int();
	startIndex = cc.read_int();
	primCount = cc.read_int();

	Log::log("FakedDrawIndexedPrimitive(), BaseVertexIndex=%d, MinVertexIndex=%d, NumVertices=%d, startIndex=%d, primCount=%d\n", BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

	return cur_device->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT FakedCreateVertexBuffer() {
	Log::log("FakedCreateVertexBuffer called\n");

	UINT id = cc.read_uint();
	UINT Length = cc.read_uint();
	DWORD Usage = cc.read_uint();
	DWORD FVF = cc.read_uint();
	D3DPOOL Pool = (D3DPOOL)(cc.read_uint());

	LPDIRECT3DVERTEXBUFFER9 vb = NULL;

	Log::log("FakedCreateVertexBuffer before create! Length:%d, Usage:%x, FVF:%x, Pool:%d, id:%d\n",Length, Usage, FVF, Pool,id);
	GET_DEVICE();
	HRESULT hr = cur_device->CreateVertexBuffer(Length, Usage, FVF, Pool, &vb, NULL);
	Log::log("FakedCreateVertexBuffer created. \n");
	vb_list[id] = new ClientVertexBuffer9(vb, Length);

	Log::log("FakedCreateVertexBuffer End. id:%d\n", id);
	return hr;
}

HRESULT FakedVertexBufferLock() {
	Log::log("FakedVertexBufferLock called\n");

	int id = obj_id;

	UINT OffestToLock = cc.read_uint();
	UINT SizeToLock = cc.read_uint();
	DWORD Flags = cc.read_uint();

	ClientVertexBuffer9* svb = NULL;
	svb = (ClientVertexBuffer9*)(vb_list[id]);
	Log::log("FakedVertexBufferLock id:%d\n",id);
	return svb->Lock(OffestToLock, SizeToLock, Flags);
}

HRESULT FakedVertexBufferUnlock() {
	Log::log("FakedVertexBufferUnlock called\n");

	int id = obj_id;

	ClientVertexBuffer9* svb = NULL;
	svb = (ClientVertexBuffer9*)(vb_list[id]);
	if(svb == NULL){
		Log::log("FakedVertexBufferUnlock is NULL id:%d\n",id);
	}else{
		Log::log("FakedVertexBufferUnlock id:%d\n",id);
	}

	return svb->Unlock();
}

HRESULT FakedSetIndices() {
	Log::log("FakedSetIndices called!!\n");
	
	int ib_id;
	
	ib_id = cc.read_short();

	GET_DEVICE();
	IDirect3DIndexBuffer9 * ib = NULL;

	Log::log("FakedSetIndices called, ib_id=%d\n", ib_id);

	if(ib_id == -1) return cur_device->SetIndices(NULL);

	ClientIndexBuffer9* sib = NULL;
	sib = (ClientIndexBuffer9*)(ib_list[ib_id]);

	if(sib == NULL) {
		Log::log("FakedSetIndices, sib is NULL\n");
	}
	
	ib = NULL;
	if(sib == NULL){
		Log::log("FakedSetIndices, sib is NULL\n");
	}else{
		ib = sib->GetIB();
	}
	
	return cur_device->SetIndices(ib);
}

HRESULT FakedCreateIndexBuffer() {
	Log::log("FakedCreateIndexBuffer called\n");
	//UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle

	UINT id = cc.read_uint();
	UINT Length = cc.read_uint();
	DWORD Usage = cc.read_uint();
	D3DFORMAT Format = (D3DFORMAT)(cc.read_uint());
	D3DPOOL Pool = (D3DPOOL)(cc.read_uint());

	LPDIRECT3DINDEXBUFFER9 ib = NULL;

	GET_DEVICE();
	HRESULT hr = cur_device->CreateIndexBuffer(Length, Usage, Format, Pool, &ib, NULL);

	ib_list[id] = new ClientIndexBuffer9(ib, Length);
	Log::log("FakedCreateIndexBuffer End. id:%d\n", id);
	return hr;
}

HRESULT FakedIndexBufferLock() {
	Log::log("FakedIndexBufferLock called\n");

	int id = obj_id;
	UINT OffestToLock = cc.read_uint();
	UINT SizeToLock = cc.read_uint();
	DWORD Flags = cc.read_uint();

	Log::log("FakedIndexBufferLock called,id:%d, OffestToLock=%d, SizeToLock=%d, Flags=%d\n",id, OffestToLock, SizeToLock, Flags);

	ClientIndexBuffer9* sib = NULL;
	sib = (ClientIndexBuffer9*)(ib_list[id]);
	return sib->Lock(OffestToLock, SizeToLock, Flags);

}

HRESULT FakedIndexBufferUnlock() {
	Log::log("FakedIndexBufferUnlock called\n");

	int id = obj_id;

	ClientIndexBuffer9* sib = NULL;
	sib = (ClientIndexBuffer9*)(ib_list[id]);
	Log::log("FakedIndexBufferUnlock IB id:%d\n",id);
	if(sib == NULL){
		Log::log("FakedIndexBufferUnlock ClientIndexBuffer is NULL id=%d\n", id);
		return S_OK;
	}

	return sib->Unlock();
}

HRESULT FakedSetSamplerState() {
	Log::log("FakedSetSamplerState called\n");
	//DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value

	int Sampler, Value;
	Sampler = cc.read_int();
	char type = cc.read_char();
	D3DSAMPLERSTATETYPE Type = (D3DSAMPLERSTATETYPE)type;
	Value = cc.read_int();

	GET_DEVICE();
	return cur_device->SetSamplerState(Sampler, Type, Value);
}

HRESULT FakedCreateVertexDeclaration() {

	int id = cc.read_int();
	int cnt = cc.read_int();
	D3DVERTEXELEMENT9 dt[100];
	cc.read_byte_arr((char*)dt, cnt * sizeof(D3DVERTEXELEMENT9));

	Log::log("FakedCreateVertexDeclaration(), id=%d, cnt=%d\n", id, cnt);

	LPDIRECT3DVERTEXDECLARATION9 vd = NULL;

	GET_DEVICE();
	HRESULT hr = cur_device->CreateVertexDeclaration(dt, &vd);

	vd_list[id] = vd;

	if(SUCCEEDED(hr)) {
		Log::log("FakedCreateVertexDeclaration() succeeded\n");
	}
	else {
		Log::log("FakedCreateVertexDeclaration() failed\n");
	}

	return hr;

	//return D3D_OK;
}

HRESULT FakedSetVertexDeclaration() {

	short id = cc.read_short();

	Log::log("FakedSetVertexDeclaration(), id=%d\n", id);

	if(id == -1) return cur_device->SetVertexDeclaration(NULL);

	LPDIRECT3DVERTEXDECLARATION9 vd = NULL;
	vd = (LPDIRECT3DVERTEXDECLARATION9)(vd_list[id]);

	GET_DEVICE();
	HRESULT hr = cur_device->SetVertexDeclaration(vd);
	if(SUCCEEDED(hr)) {
		Log::log("FakedSetVertexDeclaration() succeeded\n");
	}
	else {
		Log::log("FakedSetVertexDeclaration() failed\n");
	}
	return hr;

	//return cur_device->SetVertexDeclaration(_decl);
}

HRESULT FakedSetSoftwareVertexProcessing() {
	BOOL bSoftware = cc.read_int();

	Log::log("FakedSetSoftwareVertexProcessing(), bSoftware=%d\n", bSoftware);

	GET_DEVICE();
	return cur_device->SetSoftwareVertexProcessing(bSoftware);
}

HRESULT FakedSetLight() {
	Log::log("FakedSetLight called\n");
	//DWORD Index,CONST D3DLIGHT9* pD3DLight9

	DWORD Index = cc.read_uint();
	D3DLIGHT9 light;
	cc.read_byte_arr((char*)(&light), sizeof(D3DLIGHT9));

	GET_DEVICE();
	return cur_device->SetLight(Index, &light);
}

HRESULT FakedLightEnable() {
	Log::log("FakedLightEnable called\n");
	//DWORD Index,BOOL Enable

	DWORD Index = cc.read_uint();
	BOOL Enable = cc.read_int();

	GET_DEVICE();
	return cur_device->LightEnable(Index, Enable);
}

HRESULT FakedCreateVertexShader() {
	//DWORD* pFunction,IDirect3DVertexShader9** ppShader
	Log::log("FakedCreateVertexShader called\n");

	int id = cc.read_int();
	int cnt = cc.read_int();
	DWORD* ptr = new DWORD[cnt];
	cc.read_byte_arr((char*)ptr, cnt * sizeof(DWORD));

	LPDIRECT3DVERTEXSHADER9 base_vs = NULL;

	Log::log("FakedCreateVertexShader(), id=%d, cnt=%d\n", id, cnt);

	GET_DEVICE();
	HRESULT hr = cur_device->CreateVertexShader(ptr, &base_vs);

	vs_list[id] = base_vs;

	if(SUCCEEDED(hr)) {
		Log::log("FakedCreateVertexShader() succeeded\n");
	}
	else {
		Log::log("FakedCreateVertexShader() failed\n");
	}

	delete[] ptr;
	ptr = NULL;

	return hr;
}

HRESULT FakedSetVertexShader() {
	Log::log("FakedSetVertexShader called\n");

	int id = cc.read_int();

	Log::log("FakedSetVertexShader(), id=%d\n", id);

	if(id == -1) return cur_device->SetVertexShader(NULL);

	LPDIRECT3DVERTEXSHADER9 base_vs = NULL;
	base_vs = (LPDIRECT3DVERTEXSHADER9)(vs_list[id]);

	Log::log("FakedSetVertexShader(), vs=%d\n", vs_list[id]);

	GET_DEVICE();
	return cur_device->SetVertexShader(base_vs);
}

float vs_data[10000];

HRESULT FakedSetVertexShaderConstantF() {
	//UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount
	Log::log("FakedSetVertexShaderConstantF called\n");


	UINT StartRegister = cc.read_ushort();
	UINT Vector4fcount = cc.read_ushort();

	int i;
	for(i=0; i<Vector4fcount/2; ++i) {
		cc.read_vec(vs_data + (i * 8), 32);
	}

	if(Vector4fcount & 1) {
		cc.read_vec(vs_data + (i * 8), 16);
	}

	GET_DEVICE();
	//HRESULT hr = cur_device->SetVertexShaderConstantF(StartRegister, (float*)(cc.get_cur_ptr((Vector4fcount * 4) * sizeof(float))), Vector4fcount);
	HRESULT hr = cur_device->SetVertexShaderConstantF(StartRegister, vs_data, Vector4fcount);

	return hr;
}

HRESULT FakedCreatePixelShader() {
	Log::log("FakedCreatePixelShader called\n");


	int id = cc.read_int();
	int cnt = cc.read_int();
	DWORD* ptr = new DWORD[cnt];
	cc.read_byte_arr((char*)ptr, cnt * sizeof(DWORD));

	LPDIRECT3DPIXELSHADER9 base_ps = NULL;

	Log::log("FakedCreatePixelShader(), id=%d, cnt=%d\n", id, cnt);

	GET_DEVICE();
	HRESULT hr = cur_device->CreatePixelShader(ptr, &base_ps);

	ps_list[id] = base_ps;

	if(SUCCEEDED(hr)) {
		Log::log("FakedCreatePixelShader() succeeded\n");
	}
	else {
		Log::log("FakedCreatePixelShader() failed\n");
	}

	delete[] ptr;
	ptr = NULL;

	return hr;
}

HRESULT FakedSetPixelShader() {
	Log::log("FakedSetPixelShader called\n");

	int id = cc.read_int();

	if(id == -1) return cur_device->SetPixelShader(NULL);

	LPDIRECT3DPIXELSHADER9 base_ps = NULL;
	base_ps = (LPDIRECT3DPIXELSHADER9)(ps_list[id]);

	GET_DEVICE();
	return cur_device->SetPixelShader(base_ps);
}

HRESULT FakedSetPixelShaderConstantF() {
	Log::log("FakedSetPixelShaderConstantF called\n");

	UINT StartRegister = cc.read_uint();
	UINT Vector4fcount = cc.read_uint();

	for(int i=0; i<Vector4fcount; ++i) {
		cc.read_vec(vs_data + (i * 4));
	}

	GET_DEVICE();
	//HRESULT hr = cur_device->SetPixelShaderConstantF(StartRegister, (float*)(cc.get_cur_ptr((Vector4fcount * 4) * sizeof(float))), Vector4fcount);
	HRESULT hr = cur_device->SetPixelShaderConstantF(StartRegister, vs_data, Vector4fcount);

	return hr;
}

UINT arr[4];

char up_buf[1000];

HRESULT FakedDrawPrimitiveUP() {
	Log::log("FakedDrawPrimitiveUP called\n");
	//D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride

	/*
	D3DPRIMITIVETYPE PrimitiveType = (D3DPRIMITIVETYPE)(cc.read_uint());
	UINT PrimitiveCount = cc.read_uint();
	UINT VertexCount = cc.read_uint();
	UINT VertexStreamZeroStride = cc.read_uint();
	*/
	
	cc.read_vec((float*)&arr);
	D3DPRIMITIVETYPE PrimitiveType = (D3DPRIMITIVETYPE)arr[0];
	UINT PrimitiveCount = arr[1];
	UINT VertexCount = arr[2];
	UINT VertexStreamZeroStride = arr[3];
	
	cc.read_vec((float*)up_buf, VertexCount * VertexStreamZeroStride);

	GET_DEVICE();

	//return cur_device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, (void*)(cc.get_cur_ptr(VertexCount * VertexStreamZeroStride)), VertexStreamZeroStride);

	return cur_device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, (void*)up_buf, VertexStreamZeroStride);
}

HRESULT FakedDrawIndexedPrimitiveUP() {
	Log::log("FakedDrawIndexedPrimitiveUP called\n");
	//D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride

	D3DPRIMITIVETYPE PrimitiveType = (D3DPRIMITIVETYPE)(cc.read_uint());
	UINT MinVertexIndex = cc.read_uint();
	UINT NumVertices = cc.read_uint();
	UINT PrimitiveCount = cc.read_uint();
	D3DFORMAT IndexDataFormat = (D3DFORMAT)(cc.read_uint());
	UINT VertexStreamZeroStride = cc.read_uint();

	int indexSize = NumVertices * 2;
	if(IndexDataFormat == D3DFMT_INDEX32) indexSize = NumVertices * 4;
	char* indexData = new char[indexSize];
	cc.read_byte_arr(indexData, indexSize);

	GET_DEVICE();
	HRESULT hr = cur_device->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, (void*)indexData, IndexDataFormat, (void*)(cc.get_cur_ptr(NumVertices * VertexStreamZeroStride)), VertexStreamZeroStride);

	delete[] indexData;
	indexData = NULL;

	return hr;
}

HRESULT FakedSetVertexShaderConstantI() {
	Log::log("FakedSetVertexShaderConstantI called\n");
	//UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount


	UINT StartRegister = cc.read_uint();
	UINT Vector4icount = cc.read_uint();
	int* ptr = new int[Vector4icount * 4];
	cc.read_byte_arr((char*)ptr, (Vector4icount * 4) * sizeof(int));

	GET_DEVICE();
	HRESULT hr = cur_device->SetVertexShaderConstantI(StartRegister, ptr, Vector4icount);

	delete[] ptr;
	ptr = NULL;

	return hr;
}

HRESULT FakedSetVertexShaderConstantB() {
	Log::log("FakedSetVertexShaderConstantB called\n");
	//UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount

	UINT StartRegister = cc.read_uint();
	UINT BoolCount = cc.read_uint();

	GET_DEVICE();

	return cur_device->SetVertexShaderConstantB(StartRegister, (BOOL*)(cc.get_cur_ptr(sizeof(BOOL) * BoolCount)), BoolCount);
}

HRESULT FakedSetPixelShaderConstantI() {
	Log::log("FakedSetPixelShaderConstantI called\n");
	//UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount

	UINT StartRegister = cc.read_uint();
	UINT Vector4iCount = cc.read_uint();

	GET_DEVICE();


	return cur_device->SetPixelShaderConstantI(StartRegister, (int*)(cc.get_cur_ptr(sizeof(int) * (Vector4iCount * 4))), Vector4iCount);
}

HRESULT FakedSetPixelShaderConstantB() {
	Log::log("FakedSetPixelShaderConstantB called\n");
	//UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount

	UINT StartRegister = cc.read_uint();
	UINT BoolCount = cc.read_uint();

	GET_DEVICE();

	return cur_device->SetPixelShaderConstantB(StartRegister, (BOOL*)(cc.get_cur_ptr(sizeof(BOOL) * BoolCount)), BoolCount);
}

HRESULT FakedReset() {
	Log::log("FakedReset called\n");


	D3DPRESENT_PARAMETERS t_d3dpp;
	cc.read_byte_arr((char*)(&t_d3dpp), sizeof(t_d3dpp));

	GET_DEVICE();
	return cur_device->Reset(&d3dpp);
}

HRESULT FakedSetMaterial() {
	Log::log("FakedSetMaterial called\n");

	D3DMATERIAL9 material;
	cc.read_byte_arr((char*)(&material), sizeof(D3DMATERIAL9));

	GET_DEVICE();
	return cur_device->SetMaterial(&material);
}

extern LPDIRECT3D9 g_d3d;
extern D3DDISPLAYMODE displayMode;
BOOL IsTextureFormatOk( D3DFORMAT TextureFormat, D3DFORMAT AdapterFormat)
{

	HRESULT hr = g_d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		AdapterFormat,
		0,
		D3DRTYPE_TEXTURE,
		TextureFormat);

	return SUCCEEDED( hr );
}


HRESULT FakedCreateTexture()
{
	//UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle
	Log::log("FakedCreateTexture called\n");

	int id = cc.read_int();
	UINT Width = cc.read_uint();
	UINT Height = cc.read_uint();
	UINT Levels = cc.read_uint();
	DWORD Usage = cc.read_uint();
	D3DFORMAT Format = (D3DFORMAT)(cc.read_uint());
	D3DPOOL Pool = (D3DPOOL)(cc.read_uint());

	LPDIRECT3DTEXTURE9 base_tex = NULL;

	GET_DEVICE();

	if(IsTextureFormatOk(Format, displayMode.Format)){
		// format is ok
	}
	else{
		// form at is not ok.
		switch(Format){
		case D3DFMT_MULTI2_ARGB8:
			Log::log("Invalid Format:%d D3DFMT_MULTI2_ARGB8\n", Format);
			break;
		case D3DFMT_G8R8_G8B8:
			Log::log("Invalid Format:%d D3DFMT_G8R8_G8B8\n", Format);
			break;
		case D3DFMT_R8G8_B8G8:
			Log::log("Invalid Format:%d D3DFMT_R8G8_B8G8\n", Format);
			break;
		case D3DFMT_UYVY:
			Log::log("Invalid Format:%d D3DFMT_UYVY\n", Format);
			break;
		case D3DFMT_YUY2:
			Log::log("Invalid Format:%d D3DFMT_YUY2\n", Format);
			break;
		case D3DFMT_DXT1:
			Log::log("Invalid Format:%d D3DFMT_DXT1\n", Format);
			break;
		case D3DFMT_DXT2:
			Log::log("Invalid Format:%d D3DFMT_DXT2\n", Format);
			break;
		case D3DFMT_DXT3:
			Log::log("Invalid Format:%d D3DFMT_DXT3\n", Format);
			break;
		case D3DFMT_DXT4:
			Log::log("Invalid Format:%d D3DFMT_DXT4\n", Format);
			break;

		}
		Log::log("Invalid Format:%d \n", Format);
		if(Format == 1515474505)
			Format = D3DFMT_D32;
	}
	HRESULT hr = cur_device->CreateTexture(Width, Height, Levels, Usage, Format, Pool, &base_tex, NULL);

	switch(hr){
	case D3D_OK:
		Log::log("FakeCreateTexture(), return D3D_OK\n");
		break;
	case D3DERR_INVALIDCALL:
		Log::log("FakeCreateTexture(), return D3DERR_INVALIDCALL, height:%d, width:%d, level:%d, usage:%d, format:%d, pool:%d\n",Height, Width, Levels, Usage, Format,Pool);
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		Log::log("FakeCreateTexture(), return D3DERR_OUTOFVIDEOMEMORY\n");
		break;
	case E_OUTOFMEMORY:
		Log::log("FakeCreateTexture(), return E_OUTOFMEMORY\n");
		break;
	default:
		break;
	}
	if(base_tex == NULL) {
		Log::log("FakedCreateTexture(), CreateTexture failed, id:%d\n",id);
	}
	else{
		Log::log("FakedCreateTexture created, id:%d\n",id);
	}
	tex_list[id] = new ClientTexture9(base_tex);

	return hr;
}

HRESULT FakedSetTexture() {
	//DWORD Stage,IDirect3DBaseTexture9* pTexture

	int Stage, tex_id;

	Stage = cc.read_uint();
	tex_id = cc.read_int();

	GET_DEVICE();
	if(tex_id == -1) {
		return cur_device->SetTexture(Stage, NULL);
	}

	ClientTexture9* tex = NULL;
	tex = (ClientTexture9*)(tex_list[tex_id]);

	if(tex == NULL) {
		Log::log("FakedSetTexture(), Texture is NULL, tex id:%d\n",tex_id);
	}

	
	return cur_device->SetTexture(Stage, tex->GetTex9());
}

HRESULT FakedSetTextureStageState() {
	//DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value
	Log::log("FakedSetTextureStageState called\n");

	DWORD Stage = cc.read_uint();
	D3DTEXTURESTAGESTATETYPE Type = (D3DTEXTURESTAGESTATETYPE)(cc.read_uint());
	DWORD Value = cc.read_uint();

	GET_DEVICE();
	return cur_device->SetTextureStageState(Stage, Type, Value);
}

HRESULT FakedTransmitTextureData() {
	Log::log("FakedTransmitTextureData called\n");

#ifndef LOCAL_IMG
	
	int id = obj_id;
	//int levelCount = cc.read_int();
	int level = cc.read_int();
	
#ifdef MEM_FILE_TEX
	int Pitch = cc.read_uint();
#endif
	
	int size = cc.read_int();

	ClientTexture9* tex = NULL;
	tex = (ClientTexture9*)(tex_list[id]);
	
	Log::log("server TransmitTextureData get texture id:%d tex:%d\n",id,tex);
#ifndef MEM_FILE_TEX
	char  fname[50];
	sprintf(fname,"surface\\face_%d_leve_%d.png",id,level);
	LPDIRECT3DSURFACE9 des = NULL;
	tex->GetTex9()->GetSurfaceLevel(level,&des);
	//D3DXLoadSurfaceFromFile(des,NULL,NULL,fname,NULL,D3DX_FILTER_LINEAR,0xff000000,NULL);
	D3DXLoadSurfaceFromFileInMemory(des,NULL,NULL,cc.get_cur_ptr(size),size,NULL,D3DX_FILTER_LINEAR,0xff000000,NULL);
	return D3D_OK;
#else
	return tex->FillData(level, Pitch, size, (void*)cc.get_cur_ptr(size));

#endif
#else
	
	int id = obj_id;
	int level = cc.read_int();

	ClientTexture9* tex = NULL;
	tex = (ClientTexture9*)(tex_list[id]);
	
	Log::log("client TransmitTextureData get texture id:%d tex:%d\n",id,tex);

	char  fname[50];
	sprintf(fname,"surface\\face_%d_leve_%d.png",id,level);
	LPDIRECT3DSURFACE9 des = NULL;
	tex->GetTex9()->GetSurfaceLevel(level,&des);
	D3DXLoadSurfaceFromFile(des,NULL,NULL,fname,NULL,D3DX_FILTER_LINEAR,0xff000000,NULL);
	return D3D_OK;//tex->FillData(level, Pitch, size, (void*)cur_ptr);
#endif
}

HRESULT FakedCreateStateBlock() {
	Log::log("FakedCreateStateBlock() called\n");
	
	D3DSTATEBLOCKTYPE Type = (D3DSTATEBLOCKTYPE)(cc.read_uint());
	int id = cc.read_int();

	IDirect3DStateBlock9* base_sb = NULL;

	GET_DEVICE();
	HRESULT hr = cur_device->CreateStateBlock(Type, &base_sb);

	sb_list[id] = new ClientStateBlock9(base_sb);
	return hr;
}

HRESULT FakedBeginStateBlock() {
	Log::log("FakedBeginStateBlock() called\n");
	
	GET_DEVICE();
	return cur_device->BeginStateBlock();
}

HRESULT FakedEndStateBlock() {
	Log::log("FakedEndStateBlock() called\n");
	
	int id = cc.read_int();
	IDirect3DStateBlock9* base_sb = NULL;

	GET_DEVICE();
	HRESULT hr = cur_device->EndStateBlock(&base_sb);
	sb_list[id] = new ClientStateBlock9(base_sb);

	return hr;
}

HRESULT FakedStateBlockCapture() {
	Log::log("FakedStateBlockCapture() called\n");
	int id = obj_id;
	ClientStateBlock9* sb = NULL;
	sb = (ClientStateBlock9*)(sb_list[id]);
	return sb->Capture();
}

HRESULT FakedStateBlockApply() {
	Log::log("FakedStateBlockApply() called\n");
	int id = obj_id;
	ClientStateBlock9* sb = NULL;
	sb = (ClientStateBlock9*)(sb_list[id]);
	return sb->Apply();
}

HRESULT FakedDeviceAddRef() {
	Log::log("FakedDeviceAddRef() called\n");
	
	GET_DEVICE();
	return D3D_OK;
	return cur_device->AddRef();
}

HRESULT FakedDeviceRelease() {
	Log::log("FakedDeviceRelease() called\n");
	
	GET_DEVICE();
	return D3D_OK;
	return cur_device->Release();
}

HRESULT FakedSetViewport() {
	Log::log("FakedSetViewport() called\n");
	
	GET_DEVICE();
	D3DVIEWPORT9 viewport;
	cc.read_byte_arr((char*)(&viewport), sizeof(viewport));
	Log::log("FakeSetViewport(), viewport v.x:%d, v.y:%d, y.width:%d, v.Height:%d, v.Maxz:%f, v.Minz:%f\n", viewport.X,viewport.Y,viewport.Width,viewport.Height,viewport.MaxZ,viewport.MinZ);
	return cur_device->SetViewport(&viewport);
}

HRESULT FakedSetNPatchMode() {
	Log::log("FakedSetNPatchMode() called\n");
	
	GET_DEVICE();
	float nSegments = cc.read_float();
	return cur_device->SetNPatchMode(nSegments);
}

HRESULT FakedCreateCubeTexture() {
	//int id, UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool
	Log::log("FakedCreateCubeTexture() called\n");
	
	GET_DEVICE();

	int id = cc.read_int();
	UINT EdgeLength = cc.read_uint();
	UINT Levels = cc.read_uint();
	DWORD Usage = cc.read_uint();
	D3DFORMAT Format = (D3DFORMAT)(cc.read_uint());
	D3DPOOL Pool = (D3DPOOL)(cc.read_uint());

	IDirect3DCubeTexture9* base_cube_tex = NULL;
	HRESULT hr = cur_device->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, &base_cube_tex, NULL);

	if(SUCCEEDED(hr)) {
		ctex_list[id] = new ClientCubeTexture9(base_cube_tex);
	}
	else {
		Log::log("FakedCreateCubeTexture() failed\n");
	}
	return hr;
}

HRESULT FakedSetCubeTexture() {
	Log::log("FakedSetCubeTexture() called\n");
	
	GET_DEVICE();

	DWORD Stage = cc.read_uint();
	int id = cc.read_int();

	ClientCubeTexture9* cube_tex = NULL;
	cube_tex = (ClientCubeTexture9*)(ctex_list[id]);

	if(cube_tex == NULL) {
		Log::log("FakedSetCubeTexture(), cube_tex is NULL\n");
	}

	return cur_device->SetTexture(Stage, cube_tex->GetCubeTex9());
}

HRESULT FakedGetSwapChain() {
	Log::log("FakedGetSwapChain() called\n");
	
	GET_DEVICE();

	int id = cc.read_int();
	UINT iSwapChain = cc.read_uint();
	
	IDirect3DSwapChain9* base_chain = NULL;
	HRESULT hr = cur_device->GetSwapChain(iSwapChain, &base_chain);

	if(base_chain == NULL) {
		Log::log("FakedGetSwapChain(), base_chain is NULL\n");
	}

	ClientSwapChain9* swap_chain = NULL;
	swap_chain = (ClientSwapChain9*)(chain_list[id]);

	if(swap_chain == NULL) {
		swap_chain = new ClientSwapChain9(base_chain);
		chain_list[id] = swap_chain;
	}

	return hr;
}

HRESULT FakedSwapChainPresent() {
	Log::log("FakedSwapChainPresent() called\n");
	
	//RECT* pSourceRect,CONST RECT* pDestRect,CONST RGNDATA* pDirtyRegion,DWORD dwFlags
	
	/*
	RECT SourceRect, DestRect;
	RGNDATA DirtyRegion;

	
	int id = obj_id;

	ClientSwapChain9* swap_chain = NULL;
	swap_chain = (ClientSwapChain9*)(chain_list[id]);

	if(swap_chain == NULL) {
		Log::log("FakedSwapChainPresent(), swap_chain is NULL\n");
	}

	extern HWND hWnd;
	*/
	//擦，这里有点问题啊，mark先!
	return cur_device->Present(NULL, NULL, NULL, NULL);
	//return swap_chain->Present(pSourceRect, pDestRect, hWnd, pDirtyRegion, dwFlags);
}

HRESULT FakedSetAutoGenFilterType() {
	Log::log("FakedSetAutoGenFilterType() called\n");
	
	int id = obj_id;
	D3DTEXTUREFILTERTYPE FilterType = (D3DTEXTUREFILTERTYPE)(cc.read_uint());

	ClientTexture9* tex = NULL;
	tex = (ClientTexture9*)(tex_list[id]);

	if(tex == NULL) {
		Log::log("FakedSetAutoGenFilterType(), tex is NULL\n");
	}

	return tex->SetAutoGenFilterType(FilterType);
}

void FakedGenerateMipSubLevels() {
	Log::log("FakedGenerateMipSubLevels() called\n");
	
	int id = obj_id;

	ClientTexture9* tex = NULL;
	tex = (ClientTexture9*)(tex_list[id]);

	return tex->GenerateMipSubLevels();
}

HRESULT FakedSetRenderTarget() {
	Log::log("FakedSetRenderTarget() called\n");
	//DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget
	
	GET_DEVICE();
	DWORD RenderTargetIndex = cc.read_uint();
	int sfid = cc.read_int();
	int tex_id = cc.read_int();
	int level = cc.read_int();

	if(sfid == -1) return cur_device->SetRenderTarget(RenderTargetIndex, NULL);

	ClientSurface9* surface = (ClientSurface9*)(surface_list[sfid]);
	ClientTexture9* texture = NULL;
	if(tex_id !=-1 && tex_id <10000)
		texture = (ClientTexture9*)(tex_list[tex_id]);
	if(texture!=NULL){
		IDirect3DSurface9 * real_sur=NULL;
		texture->GetTex9()->GetSurfaceLevel(level, &real_sur);
		surface->ReplaceSurface(real_sur);
		/*char  fname[50];
		sprintf(fname,"surface\\Target%d_NOtex_%d.png",sfid,tex_id);
		D3DXSaveSurfaceToFile(fname,D3DXIFF_PNG,surface->GetSurface9(), NULL, NULL);*/
		return cur_device->SetRenderTarget(RenderTargetIndex,  real_sur);
		//MessageBoxA(NULL,"surface from tex!","WARNING",MB_OK);
	}
	else if(surface){
		/*char  fname[50];
		sprintf(fname,"surface\\Target%d_tex_%d.png",sfid,tex_id);
		D3DXSaveSurfaceToFile(fname,D3DXIFF_PNG,surface->GetSurface9(), NULL, NULL);*/
		Log::log("surface:%d,texture id:%d, surface id:%d, level:%d\n", surface,tex_id,sfid, level);
		return cur_device->SetRenderTarget(RenderTargetIndex, surface->GetSurface9());
		//MessageBoxA(NULL,"surface NO!","WARNING",MB_OK);
	}
	else{
		Log::log("surface is NULL, texture id: %d, surface id: %d, level:%d\n", tex_id, sfid, level);
		return cur_device->SetRenderTarget(RenderTargetIndex, NULL);
	}
}

HRESULT FakedSetDepthStencilSurface() {
	Log::log("FakedSetDepthStencilSurface() called\n");

	
	GET_DEVICE();
	int sfid = cc.read_int();
	Log::log("surface id:%d\n", sfid);
	if(sfid == -1) return cur_device->SetDepthStencilSurface(NULL);

	ClientSurface9* surface = (ClientSurface9*)(surface_list[sfid]);

	return cur_device->SetDepthStencilSurface(surface->GetSurface9());
}

HRESULT FakedTextureGetSurfaceLevel() {
	Log::log("FakedTextureGetSurfaceLevel() called\n");
	
	int id = obj_id;
	int t_id = cc.read_int();
	int sfid = cc.read_int();
	UINT Level = cc.read_int();

	ClientTexture9* tex = (ClientTexture9*)(tex_list[t_id]);
	if(!tex){
		Log::log("client texture null, id:%d\n", t_id);
	}

	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = tex->GetSurfaceLevel(Level, &base_surface);

	surface_list[sfid] = new ClientSurface9(base_surface);

	return hr;
}

HRESULT FakedSwapChainGetBackBuffer() {
	//UINT iBackBuffer,D3DBACKBUFFER_TYPE Type
	Log::log("FakedSwapChainGetBackBuffer() called\n");
	
	int chain_id = obj_id;
	int surface_id = cc.read_int();
	UINT iBackBuffer = cc.read_uint();
	D3DBACKBUFFER_TYPE Type = (D3DBACKBUFFER_TYPE)(cc.read_uint());

	ClientSwapChain9* chain = (ClientSwapChain9*)(chain_list[chain_id]);

	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = chain->GetSwapChain9()->GetBackBuffer(iBackBuffer, Type, &base_surface);

	surface_list[surface_id] = new ClientSurface9(base_surface);

	return hr;
}

HRESULT FakedGetDepthStencilSurface() {
	Log::log("FakedGetDepthStencilSurface() called\n");
	
	GET_DEVICE();
	int sfid = cc.read_int();
	Log::log("surface id:%d\n", sfid);
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = cur_device->GetDepthStencilSurface(&base_surface);
	if(hr == D3D_OK){
		Log::log("GetDepthStencilSurface is OK!\n");
		surface_list[sfid] = new ClientSurface9(base_surface);
	}else{
		Log::log("ERROR! GetDepthStencilSurface failed!\n");

	}

	return hr;
}

HRESULT FakedCreateDepthStencilSurface() {
	Log::log("FakedCreateDepthStencilSurface() called\n");
	//UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle
	
	GET_DEVICE();
	int id = cc.read_int();
	UINT Width = cc.read_uint();
	UINT Height = cc.read_uint();
	D3DFORMAT Format = (D3DFORMAT)(cc.read_uint());
	D3DMULTISAMPLE_TYPE MultiSample = (D3DMULTISAMPLE_TYPE)(cc.read_uint());
	DWORD MultisampleQuality = cc.read_uint();
	BOOL Discard = cc.read_int();
	
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = cur_device->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, &base_surface, NULL);

	surface_list[id] = new ClientSurface9(base_surface);

	return hr;
}

HRESULT FakedCubeGetCubeMapSurface() {
	Log::log("FakedCubeGetCubeMapSurface() called\n");
	//D3DCUBEMAP_FACES FaceType,UINT Level
	
	int cube_id = obj_id;
	int surface_id = cc.read_int();
	D3DCUBEMAP_FACES FaceType = (D3DCUBEMAP_FACES)(cc.read_uint());
	UINT Level = cc.read_uint();

	ClientCubeTexture9* cube = (ClientCubeTexture9*)(ctex_list[cube_id]);
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = cube->GetCubeTex9()->GetCubeMapSurface(FaceType, Level, &base_surface);

	surface_list[surface_id] = new ClientSurface9(base_surface);

	return hr;
}

HRESULT FakeTransmitSurface(){
	Log::log("FakeTransmitSurface() called!\n");
	
	int id = obj_id;
	int size = cc.read_int();
	LPDIRECT3DSURFACE9 surface = NULL;
	//cur_device->CreateDepthStencilSurface(
	//D3DXLoadSurfaceFromFileInMemory(

	return D3D_OK;
}


//newly added
HRESULT FakeD3DDeviceGetBackBuffer(){
	Log::log("FakeDeviceGetBackBuffer() called!\n");
	int id = obj_id;   // device id

	int surface_id = cc.read_int();
	UINT iSwapChain = cc.read_uint();
	UINT iBackBuffer = cc.read_uint();
	UINT Type = cc.read_uint();
	D3DBACKBUFFER_TYPE type = (D3DBACKBUFFER_TYPE)Type;
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = cur_device->GetBackBuffer(iSwapChain, iBackBuffer, type, &base_surface);

	if(hr== D3D_OK){
		surface_list[surface_id] = new ClientSurface9(base_surface);
	}else{
		Log::log("ERROR! D3DDeviceGetBackBuffer() failed!\n");
	}
	return hr;
}

HRESULT FakeD3DGetDeviceCaps(){
	HRESULT hr;
	Log::log("FakeD3DGetDeviceCaps called!\n");
	D3DDEVTYPE type = (D3DDEVTYPE)cc.read_int();
	D3DCAPS9 d3d_caps;
	hr = g_d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, type, &d3d_caps);
	if(SUCCEEDED(hr)){
		// send back the parameters of device
		//cc.send_raw_buffer((char *)&d3d_caps, sizeof(D3DCAPS9));
		//cc.write_byte_arr((char*) &d3d_caps, sizeof(D3DCAPS9));
	}
	else{
		Log::log("FakeD3DGetDeviceCaps failed!\n");
	}


	return hr;
}
HRESULT FakeD3DDGetRenderTarget(){
	HRESULT hr;
	Log::log("FakeD3DDGetRenderTarget() called!\n");
	int sid = -1;
	sid = cc.read_int();
	DWORD RenderTargetIndex = (DWORD)cc.read_uint();
	IDirect3DSurface9 * target = NULL;
	hr= cur_device->GetRenderTarget(RenderTargetIndex, &target);

	if(hr== D3D_OK){
		surface_list[sid] = new ClientSurface9(target);
	}else{
		Log::log("ERROR! D3DDeviceGetBackBuffer() failed!\n");
	}


	return hr;
}
HRESULT FakeD3DDSetScissorRect(){
	HRESULT hr;
	Log::log("FakeD3DDSetScissorRect() calle!\n");
	int left = cc.read_int();
	int right = cc.read_int();
	int top = cc.read_int();
	int bottom = cc.read_int();

	GET_DEVICE();

	RECT re;
	re.left = left;
	re.right= right;
	re.bottom = bottom;
	re.top = top;
	hr = cur_device->SetScissorRect(&re);

	return hr;
}

HRESULT FakedSetVertexBufferFormat() {
	Log::log("FakedSetVertexBufferFormat() called\n");

	int id = obj_id;

	ClientVertexBuffer9* svb = NULL;
	svb = (ClientVertexBuffer9*)(vb_list[id]);
	if(svb == NULL){
		Log::log("FakedSetVertexBufferFormat is NULL id:%d\n",id);
	}else{
		Log::log("FakedSetVertexBufferFormat id:%d\n",id);
	}

	return svb->SetVertexBufferFormat();
}

HRESULT FakedSetDecimateResult() {
	Log::log("FakedSetDecimateResult() called\n");

	int id = obj_id;

	ClientVertexBuffer9* svb = NULL;
	svb = (ClientVertexBuffer9*)(vb_list[id]);
	if(svb == NULL){
		Log::log("FakedSetDecimateResult is NULL id:%d\n",id);
	}else{
		Log::log("FakedSetDecimateResult id:%d\n",id);
	}

	return svb->SetDecimateResult();
}

HRESULT FakedSetGammaRamp() {
	Log::log("FakedSetGammaRamp() called\n");

	UINT iSwapChain = cc.read_uint();
	DWORD Flags = cc.read_uint();
	D3DGAMMARAMP pRamp;
	cc.read_byte_arr((char*)&pRamp, sizeof(D3DGAMMARAMP));

	GET_DEVICE();

	cur_device->SetGammaRamp(iSwapChain, Flags, &pRamp);
	return D3D_OK;
}

extern int inputTickEnd;
extern int inputTickStart;

HRESULT FakeNullInstruct(){
	SYSTEMTIME sys, now;
	GetLocalTime(&now);
	inputTickEnd = GetTickCount();
	Log::slog("%d\n", inputTickEnd - inputTickStart);
	return D3D_OK;
}

