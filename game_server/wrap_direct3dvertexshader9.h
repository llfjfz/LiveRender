#ifndef __WRAP_DIRECT3DVERTEXSHADER9__
#define __WRAP_DIRECT3DVERTEXSHADER9__

#include "game_server.h"

class WrapperDirect3DVertexShader9: public IDirect3DVertexShader9 {
private:
	IDirect3DVertexShader9* m_vs;
	int id;

	static HashSet m_list;

public:
	static int ins_count;
	WrapperDirect3DVertexShader9(IDirect3DVertexShader9* ptr, int _id);
	int GetID();
	void SetID(int id);
	IDirect3DVertexShader9* GetVS9();

	static WrapperDirect3DVertexShader9* GetWrapperVertexShader(IDirect3DVertexShader9* ptr);

public:

	/*** IUnknown methods ***/
	COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	COM_METHOD(ULONG,AddRef)(THIS);
	COM_METHOD(ULONG,Release)(THIS);

	/*** IDirect3DVertexShader9 methods ***/
	COM_METHOD(HRESULT,GetDevice)(THIS_ IDirect3DDevice9** ppDevice);
	COM_METHOD(HRESULT,GetFunction)(THIS_ void*,UINT* pSizeOfData);
};

#endif
