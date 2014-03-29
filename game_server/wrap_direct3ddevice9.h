#ifndef __WRAP_DIRECT3DDEVICE9__
#define __WRAP_DIRECT3DDEVICE9__

#include "game_server.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "wrap_direct3dvertexdeclaration9.h"
#include "wrap_direct3dindexbuffer9.h"
#include "wrap_direct3dvertexbuffer9.h"
#include "utility.h"
using namespace std;

class WrapperDirect3DDevice9: public IDirect3DDevice9 {
private:
	
	int id;
	static HashSet m_list;

	WrapperDirect3DVertexDeclaration9* cur_decl_;
	WrapperDirect3DIndexBuffer9* cur_ib_;
	WrapperDirect3DVertexBuffer9* cur_vbs_[Source_Count];


public:

	int is_even_frame_;

	IDirect3DDevice9* m_device;
	WrapperDirect3DDevice9(IDirect3DDevice9* ptr, int _id);
	void SetID(int id);
	int GetID();

	static WrapperDirect3DDevice9* GetWrapperDevice9(IDirect3DDevice9* ptr);
	IDirect3DDevice9* GetIDirect3DDevice9(){
		return m_device;
	}

public:

	static int ins_count;

	COM_METHOD(HRESULT, QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	COM_METHOD(ULONG, AddRef)(THIS);
	COM_METHOD(ULONG, Release)(THIS);

	/*** IDirect3DDevice9 methods ***/
	COM_METHOD(HRESULT, TestCooperativeLevel)(THIS);
	COM_METHOD(UINT, GetAvailableTextureMem)(THIS);
	COM_METHOD(HRESULT, EvictManagedResources)(THIS);
	COM_METHOD(HRESULT, GetDirect3D)(THIS_ IDirect3D9** ppD3D9);
	COM_METHOD(HRESULT, GetDeviceCaps)(THIS_ D3DCAPS9* pCaps);
	COM_METHOD(HRESULT, GetDisplayMode)(THIS_ UINT iSwapChain,D3DDISPLAYMODE* pMode);
	COM_METHOD(HRESULT, GetCreationParameters)(THIS_ D3DDEVICE_CREATION_PARAMETERS *pParameters);
	COM_METHOD(HRESULT, SetCursorProperties)(THIS_ UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap);
	COM_METHOD(void, SetCursorPosition)(THIS_ int X,int Y,DWORD Flags);
	COM_METHOD(BOOL, ShowCursor)(THIS_ BOOL bShow);
	COM_METHOD(HRESULT, CreateAdditionalSwapChain)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain);
	COM_METHOD(HRESULT, GetSwapChain)(THIS_ UINT iSwapChain,IDirect3DSwapChain9** pSwapChain);
	COM_METHOD(UINT, GetNumberOfSwapChains)(THIS);
	COM_METHOD(HRESULT, Reset)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters);
	COM_METHOD(HRESULT, Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	COM_METHOD(HRESULT, GetBackBuffer)(THIS_ UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	COM_METHOD(HRESULT, GetRasterStatus)(THIS_ UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus);
	COM_METHOD(HRESULT, SetDialogBoxMode)(THIS_ BOOL bEnableDialogs);
	COM_METHOD(void, SetGammaRamp)(THIS_ UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp);
	COM_METHOD(void, GetGammaRamp)(THIS_ UINT iSwapChain,D3DGAMMARAMP* pRamp);
	COM_METHOD(HRESULT, CreateTexture)(THIS_ UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
	COM_METHOD(HRESULT, CreateVolumeTexture)(THIS_ UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle);
	COM_METHOD(HRESULT, CreateCubeTexture)(THIS_ UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
	COM_METHOD(HRESULT, CreateVertexBuffer)(THIS_ UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
	COM_METHOD(HRESULT, CreateIndexBuffer)(THIS_ UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle);
	COM_METHOD(HRESULT, CreateRenderTarget)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	COM_METHOD(HRESULT, CreateDepthStencilSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	COM_METHOD(HRESULT, UpdateSurface)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint);
	COM_METHOD(HRESULT, UpdateTexture)(THIS_ IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture);
	COM_METHOD(HRESULT, GetRenderTargetData)(THIS_ IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface);
	COM_METHOD(HRESULT, GetFrontBufferData)(THIS_ UINT iSwapChain,IDirect3DSurface9* pDestSurface);
	COM_METHOD(HRESULT, StretchRect)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter);
	COM_METHOD(HRESULT, ColorFill)(THIS_ IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color);
	COM_METHOD(HRESULT, CreateOffscreenPlainSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	COM_METHOD(HRESULT, SetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
	COM_METHOD(HRESULT, GetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
	COM_METHOD(HRESULT, SetDepthStencilSurface)(THIS_ IDirect3DSurface9* pNewZStencil);
	COM_METHOD(HRESULT, GetDepthStencilSurface)(THIS_ IDirect3DSurface9** ppZStencilSurface);
	COM_METHOD(HRESULT, BeginScene)(THIS);
	COM_METHOD(HRESULT, EndScene)(THIS);
	COM_METHOD(HRESULT, Clear)(THIS_ DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	COM_METHOD(HRESULT, SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	COM_METHOD(HRESULT, GetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	COM_METHOD(HRESULT, MultiplyTransform)(THIS_ D3DTRANSFORMSTATETYPE Type,CONST D3DMATRIX* pD3DMatrix);
	COM_METHOD(HRESULT, SetViewport)(THIS_ CONST D3DVIEWPORT9* pViewport);
	COM_METHOD(HRESULT, GetViewport)(THIS_ D3DVIEWPORT9* pViewport);
	COM_METHOD(HRESULT, SetMaterial)(THIS_ CONST D3DMATERIAL9* pMaterial);
	COM_METHOD(HRESULT, GetMaterial)(THIS_ D3DMATERIAL9* pMaterial);
	COM_METHOD(HRESULT, SetLight)(THIS_ DWORD Index,CONST D3DLIGHT9* pD3DLight9);
	COM_METHOD(HRESULT, GetLight)(THIS_ DWORD Index,D3DLIGHT9* pD3DLight9);
	COM_METHOD(HRESULT, LightEnable)(THIS_ DWORD Index,BOOL Enable);
	COM_METHOD(HRESULT, GetLightEnable)(THIS_ DWORD Index,BOOL* pEnable);
	COM_METHOD(HRESULT, SetClipPlane)(THIS_ DWORD Index,CONST float* pPlane);
	COM_METHOD(HRESULT, GetClipPlane)(THIS_ DWORD Index,float* pPlane);
	COM_METHOD(HRESULT, SetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD Value);
	COM_METHOD(HRESULT, GetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD* pValue);
	COM_METHOD(HRESULT, CreateStateBlock)(THIS_ D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB);
	COM_METHOD(HRESULT, BeginStateBlock)(THIS);
	COM_METHOD(HRESULT, EndStateBlock)(THIS_ IDirect3DStateBlock9** ppSB);
	COM_METHOD(HRESULT, SetClipStatus)(THIS_ CONST D3DCLIPSTATUS9* pClipStatus);
	COM_METHOD(HRESULT, GetClipStatus)(THIS_ D3DCLIPSTATUS9* pClipStatus);
	COM_METHOD(HRESULT, GetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9** ppTexture);
	COM_METHOD(HRESULT, SetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9* pTexture);
	COM_METHOD(HRESULT, GetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
	COM_METHOD(HRESULT, SetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	COM_METHOD(HRESULT, GetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue);
	COM_METHOD(HRESULT, SetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
	COM_METHOD(HRESULT, ValidateDevice)(THIS_ DWORD* pNumPasses);
	COM_METHOD(HRESULT, SetPaletteEntries)(THIS_ UINT PaletteNumber,CONST PALETTEENTRY* pEntries);
	COM_METHOD(HRESULT, GetPaletteEntries)(THIS_ UINT PaletteNumber,PALETTEENTRY* pEntries);
	COM_METHOD(HRESULT, SetCurrentTexturePalette)(THIS_ UINT PaletteNumber);
	COM_METHOD(HRESULT, GetCurrentTexturePalette)(THIS_ UINT *PaletteNumber);
	COM_METHOD(HRESULT, SetScissorRect)(THIS_ CONST RECT* pRect);
	COM_METHOD(HRESULT, GetScissorRect)(THIS_ RECT* pRect);
	COM_METHOD(HRESULT, SetSoftwareVertexProcessing)(THIS_ BOOL bSoftware);
	COM_METHOD(BOOL, GetSoftwareVertexProcessing)(THIS);
	COM_METHOD(HRESULT, SetNPatchMode)(THIS_ float nSegments);
	COM_METHOD(float, GetNPatchMode)(THIS);
	COM_METHOD(HRESULT, DrawPrimitive)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	COM_METHOD(HRESULT, DrawIndexedPrimitive)(THIS_ D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
	COM_METHOD(HRESULT, DrawPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	COM_METHOD(HRESULT, DrawIndexedPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	COM_METHOD(HRESULT, ProcessVertices)(THIS_ UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags);
	COM_METHOD(HRESULT, CreateVertexDeclaration)(THIS_ CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl);
	COM_METHOD(HRESULT, SetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9* pDecl);
	COM_METHOD(HRESULT, GetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9** ppDecl);
	COM_METHOD(HRESULT, SetFVF)(THIS_ DWORD FVF);
	COM_METHOD(HRESULT, GetFVF)(THIS_ DWORD* pFVF);
	COM_METHOD(HRESULT, CreateVertexShader)(THIS_ CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
	COM_METHOD(HRESULT, SetVertexShader)(THIS_ IDirect3DVertexShader9* pShader);
	COM_METHOD(HRESULT, GetVertexShader)(THIS_ IDirect3DVertexShader9** ppShader);
	COM_METHOD(HRESULT, SetVertexShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	COM_METHOD(HRESULT, GetVertexShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	COM_METHOD(HRESULT, SetVertexShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
	COM_METHOD(HRESULT, GetVertexShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	COM_METHOD(HRESULT, SetVertexShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
	COM_METHOD(HRESULT, GetVertexShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	COM_METHOD(HRESULT, SetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
	COM_METHOD(HRESULT, GetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride);
	COM_METHOD(HRESULT, SetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT Setting);
	COM_METHOD(HRESULT, GetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT* pSetting);
	COM_METHOD(HRESULT, SetIndices)(THIS_ IDirect3DIndexBuffer9* pIndexData);
	COM_METHOD(HRESULT, GetIndices)(THIS_ IDirect3DIndexBuffer9** ppIndexData);
	COM_METHOD(HRESULT, CreatePixelShader)(THIS_ CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);
	COM_METHOD(HRESULT, SetPixelShader)(THIS_ IDirect3DPixelShader9* pShader);
	COM_METHOD(HRESULT, GetPixelShader)(THIS_ IDirect3DPixelShader9** ppShader);
	COM_METHOD(HRESULT, SetPixelShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	COM_METHOD(HRESULT, GetPixelShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	COM_METHOD(HRESULT, SetPixelShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
	COM_METHOD(HRESULT, GetPixelShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	COM_METHOD(HRESULT, SetPixelShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
	COM_METHOD(HRESULT, GetPixelShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) ;
	COM_METHOD(HRESULT, DrawRectPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
	COM_METHOD(HRESULT, DrawTriPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
	COM_METHOD(HRESULT, DeletePatch)(THIS_ UINT Handle);
	COM_METHOD(HRESULT, CreateQuery)(THIS_ D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery);

};

#endif