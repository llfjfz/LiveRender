#ifndef __CLIENT_CUBETEXTURE9__
#define __CLIENT_CUBETEXTURE9__

#include "game_client.h"

class ClientCubeTexture9 {
private:
	IDirect3DCubeTexture9* m_cube_tex;
public:
	ClientCubeTexture9(IDirect3DCubeTexture9* ptr);
	IDirect3DCubeTexture9* GetCubeTex9();
};

#endif