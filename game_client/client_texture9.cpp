#include "client_texture9.h"

void ClientTexture9::ReplaceTexture(IDirect3DTexture9* nptr,IDirect3DDevice9* cur_device){
	if(nptr){
		int levelCount = this->m_tex->GetLevelCount();
		LPDIRECT3DSURFACE9 des= NULL, src  = NULL;
		for(int level = 0; level < levelCount; level ++){
			this->m_tex->GetSurfaceLevel(level, &src);
			nptr->GetSurfaceLevel(level, &des);
			//src = (des);
			cur_device->UpdateSurface(src,NULL,des,NULL);
		}
		this->m_tex->Release();
		this->m_tex = nptr;
	}
	else{
		MessageBoxA(NULL,"Texture NULL!", "WARNING",MB_OK);
	}
}
ClientTexture9::ClientTexture9(IDirect3DTexture9* ptr): m_tex(ptr) {
	Log::log("serverTexture9 constructor called!\n");
}

HRESULT ClientTexture9::FillData(int Level, int Pitch, int size, void* ptr) {
	Log::log("ClientTexture9::FillData() called, this:%d\n",this);
	
	D3DLOCKED_RECT rect;
	LPDIRECT3DSURFACE9 top_surface = NULL;
	
	this->m_tex->GetSurfaceLevel(Level, &top_surface);

	Log::log("ClientTexture9::FillData() surface return:%d\n",top_surface);
	if(top_surface == NULL)
		return D3D_OK;
	top_surface->LockRect(&rect, 0, 0);
	memcpy(rect.pBits, ptr, size);
	rect.Pitch = Pitch;
	top_surface->UnlockRect();
	
	top_surface->Release();
	top_surface = NULL;

	return D3D_OK;
}

IDirect3DTexture9* ClientTexture9::GetTex9() {
	return this->m_tex;
}

HRESULT ClientTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) {
	return m_tex->SetAutoGenFilterType(FilterType);
}

void ClientTexture9::GenerateMipSubLevels() {
	return m_tex->GenerateMipSubLevels();
}

HRESULT ClientTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel) {
	return m_tex->GetSurfaceLevel(Level, ppSurfaceLevel);
}


