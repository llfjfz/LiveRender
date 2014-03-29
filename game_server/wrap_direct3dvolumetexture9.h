#ifndef __WRAP_DIRECT3DVOLUMETEXTURE9__
#define __WRAP_DIRECT3DVOLUMETEXTURE9__
#include "game_server.h"

class WrapperDirect3DVolumeTexture9: public IDirect3DVolumeTexture9 {
private:
	IDirect3DVolumeTexture9* m_tex;
	int id;

	static HashSet m_list;

public:

	static int ins_count;
	WrapperDirect3DVolumeTexture9(IDirect3DVolumeTexture9* ptr, int _id);
	int GetID();
	void SetID(int id);
	IDirect3DVolumeTexture9* GetVolumeTex9();
	void SayHi(char* str);
	HRESULT SendTextureData();
	static WrapperDirect3DVolumeTexture9* GetWrapperTexture9(IDirect3DVolumeTexture9* ptr);

	/*** IUnknown methods ***/
	COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	COM_METHOD(ULONG,AddRef)(THIS);
	COM_METHOD(ULONG,Release)(THIS);

	COM_METHOD(HRESULT, GetDevice)(THIS_ IDirect3DDevice9** ppDevice);
    COM_METHOD(HRESULT, SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags);
    COM_METHOD(HRESULT, GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData);
    COM_METHOD(HRESULT, FreePrivateData)(THIS_ REFGUID refguid);
    COM_METHOD(DWORD, SetPriority)(THIS_ DWORD PriorityNew);
    COM_METHOD(DWORD, GetPriority)(THIS);
    COM_METHOD(void, PreLoad)(THIS);
    COM_METHOD(D3DRESOURCETYPE, GetType)(THIS);
    COM_METHOD(DWORD, SetLOD)(THIS_ DWORD LODNew);
    COM_METHOD(DWORD, GetLOD)(THIS);
    COM_METHOD(DWORD, GetLevelCount)(THIS);
    COM_METHOD(HRESULT, SetAutoGenFilterType)(THIS_ D3DTEXTUREFILTERTYPE FilterType);
    COM_METHOD(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)(THIS);
    COM_METHOD(void, GenerateMipSubLevels)(THIS);
    COM_METHOD(HRESULT, GetLevelDesc)(THIS_ UINT Level,D3DVOLUME_DESC *pDesc);
    COM_METHOD(HRESULT, GetVolumeLevel)(THIS_ UINT Level,IDirect3DVolume9** ppVolumeLevel);
    COM_METHOD(HRESULT, LockBox)(THIS_ UINT Level,D3DLOCKED_BOX* pLockedVolume,CONST D3DBOX* pBox,DWORD Flags);
    COM_METHOD(HRESULT, UnlockBox)(THIS_ UINT Level);
    COM_METHOD(HRESULT, AddDirtyBox)(THIS_ CONST D3DBOX* pDirtyBox);
};

#endif