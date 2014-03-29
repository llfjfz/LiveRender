#ifndef __WRAP_DIRECT3DSTATEBLOCK9__
#define __WRAP_DIRECT3DSTATEBLOCK9__

#include "game_server.h"

class WrapperDirect3DStateBlock9 : public IDirect3DStateBlock9 {
private:
	IDirect3DStateBlock9* m_sb;
	int id;

public:
	static int ins_count;
	WrapperDirect3DStateBlock9(IDirect3DStateBlock9* _sb, int _id);
	int GetID();
	void SetID(int id);
	IDirect3DStateBlock9* GetSB9();

public:
	/*** IUnknown methods ***/
	COM_METHOD(HRESULT, QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	COM_METHOD(ULONG,AddRef)(THIS);
	COM_METHOD(ULONG,Release)(THIS);

	/*** IDirect3DStateBlock9 methods ***/
	COM_METHOD(HRESULT, GetDevice)(THIS_ IDirect3DDevice9** ppDevice);
	COM_METHOD(HRESULT, Capture)(THIS);
	COM_METHOD(HRESULT, Apply)(THIS);
};

#endif
