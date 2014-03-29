#include "client_cubetexture9.h"

ClientCubeTexture9::ClientCubeTexture9(IDirect3DCubeTexture9* ptr): m_cube_tex(ptr) {

}

IDirect3DCubeTexture9* ClientCubeTexture9::GetCubeTex9() {
	return m_cube_tex;
}