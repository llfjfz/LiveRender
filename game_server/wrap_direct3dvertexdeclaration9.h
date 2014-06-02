#ifndef __WRAP_DIRECT3DVERTEXDECLARATION9__
#define __WRAP_DIRECT3DVERTEXDECLARATION9__

#include "game_server.h"
/**
FVF(Flexible Vertex Format), will transform to vertex declaration. The IDirect3DVertexDeclaration9
is usually used in shader programming 
**/

class WrapperDirect3DVertexDeclaration9: public IDirect3DVertexDeclaration9 {
private:
	IDirect3DVertexDeclaration9* m_vd;
	int id;
public:
	UINT numElements;
	D3DVERTEXELEMENT9 * pDecl;

	static int ins_count;
	WrapperDirect3DVertexDeclaration9(IDirect3DVertexDeclaration9* ptr, int _id);
	IDirect3DVertexDeclaration9* GetVD9();
	void SetID(int id);
	int GetID();

public:
	/*** IUnknown methods ***/
	COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	COM_METHOD(ULONG,AddRef)(THIS);
	COM_METHOD(ULONG,Release)(THIS);

	/*** IDirect3DVertexDeclaration9 methods ***/
	COM_METHOD(HRESULT,GetDevice)(THIS_ IDirect3DDevice9** ppDevice);
	COM_METHOD(HRESULT,GetDeclaration)(THIS_ D3DVERTEXELEMENT9* pElement,UINT* pNumElements);
};

#endif
