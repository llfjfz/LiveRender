#ifndef __WRAP_DIRECT3DBASETEXTURE9__
#define __WRAP_DIRECT3DBASETEXTURE9__

#include "game_server.h"

class WrapperDirect3DCubeTexture9 : public IDirect3DCubeTexture9 {
private:
	int id;
	static HashSet m_list;
	IDirect3DCubeTexture9* m_cube_tex;

public:
	static int ins_count;
	IDirect3DCubeTexture9* GetCubeTex9();
	int GetID();
	void SetID(int id);
	WrapperDirect3DCubeTexture9(IDirect3DCubeTexture9* ptr, int id);
	static WrapperDirect3DCubeTexture9* GetWrapperCubeTexture9(IDirect3DCubeTexture9* ptr);

public:
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
	STDMETHOD(SetAutoGenFilterType)(THIS_ D3DTEXTUREFILTERTYPE FilterType);
	COM_METHOD(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)(THIS);
	COM_METHOD(void, GenerateMipSubLevels)(THIS);
	COM_METHOD(HRESULT,GetLevelDesc)(THIS_ UINT Level,D3DSURFACE_DESC *pDesc);
	COM_METHOD(HRESULT,GetCubeMapSurface)(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface9** ppCubeMapSurface);
	COM_METHOD(HRESULT,LockRect)(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
	COM_METHOD(HRESULT,UnlockRect)(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level);
	COM_METHOD(HRESULT,AddDirtyRect)(THIS_ D3DCUBEMAP_FACES FaceType,CONST RECT* pDirtyRect);
};

#endif