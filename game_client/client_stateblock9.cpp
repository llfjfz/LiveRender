#include "client_stateblock9.h"

ClientStateBlock9::ClientStateBlock9(IDirect3DStateBlock9* ptr): m_sb(ptr) {

}

HRESULT ClientStateBlock9::Capture() {
	return m_sb->Capture();
}

HRESULT ClientStateBlock9::Apply() {
	return m_sb->Apply();
}