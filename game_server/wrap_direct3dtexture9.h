#ifndef __WRAP_DIRECT3DTEXTURE9__
#define __WRAP_DIRECT3DTEXTURE9__

#include "game_server.h"

class WrapperDirect3DTexture9: public IDirect3DTexture9 {
private:
	IDirect3DTexture9* m_tex;
	int id;

	static HashSet m_list;

public:
	bool isSent; // true for send already while false for send the texture right
	D3DFORMAT Format;
	D3DPOOL Pool;
	UINT usage;
	UINT Height, Width;
	UINT Levels;
	DWORD Filter;
	DWORD MipFilter;
	D3DCOLOR ColorKey;

	static int ins_count;
	WrapperDirect3DTexture9(IDirect3DTexture9* ptr, int _id);
	int GetID();
	void SetID(int id);
	IDirect3DTexture9* GetTex9();
	void SayHi(char* str);
	HRESULT SendTextureData();
	static WrapperDirect3DTexture9* GetWrapperTexture9(IDirect3DTexture9* ptr);

	/*** IUnknown methods ***/
	COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	COM_METHOD(ULONG,AddRef)(THIS);
	COM_METHOD(ULONG,Release)(THIS);

	/*** IDirect3DBaseTexture9 methods ***/
	COM_METHOD(HRESULT,GetDevice)(THIS_ IDirect3DDevice9** ppDevice);
	COM_METHOD(HRESULT,SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags);
	COM_METHOD(HRESULT,GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData);
	COM_METHOD(HRESULT,FreePrivateData)(THIS_ REFGUID refguid);
	COM_METHOD(DWORD, SetPriority)(THIS_ DWORD PriorityNew);
	COM_METHOD(DWORD, GetPriority)(THIS);
	COM_METHOD(void, PreLoad)(THIS);
	COM_METHOD(D3DRESOURCETYPE, GetType)(THIS);
	COM_METHOD(DWORD, SetLOD)(THIS_ DWORD LODNew);
	COM_METHOD(DWORD, GetLOD)(THIS);
	COM_METHOD(DWORD, GetLevelCount)(THIS);
	COM_METHOD(HRESULT,SetAutoGenFilterType)(THIS_ D3DTEXTUREFILTERTYPE FilterType);
	COM_METHOD(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)(THIS);
	COM_METHOD(void, GenerateMipSubLevels)(THIS);
	COM_METHOD(HRESULT,GetLevelDesc)(THIS_ UINT Level,D3DSURFACE_DESC *pDesc);
	COM_METHOD(HRESULT,GetSurfaceLevel)(THIS_ UINT Level,IDirect3DSurface9** ppSurfaceLevel);
	COM_METHOD(HRESULT,LockRect)(THIS_ UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
	COM_METHOD(HRESULT,UnlockRect)(THIS_ UINT Level);
	COM_METHOD(HRESULT,AddDirtyRect)(THIS_ CONST RECT* pDirtyRect);
};

#endif