#ifndef __CLIENT_STATEBLOCK9__
#define __CLIENT_STATEBLOCK9__

#include "game_client.h"

class ClientStateBlock9 {
private:
	IDirect3DStateBlock9* m_sb;
public:
	ClientStateBlock9(IDirect3DStateBlock9* ptr);
	HRESULT Capture();
	HRESULT Apply();
};

#endif