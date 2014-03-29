#ifndef __CLIENT_API__
#define __CLIENT_API__

#include <d3d9.h>

struct FuncJumpTable {
	char * name;
	void (**func)();
};

struct fptype {
	HRESULT (*CreateDevice)();
	HRESULT (*BeginScene)();
	HRESULT (*EndScene)();
	HRESULT (*Clear)();
	HRESULT (*Present)();

	HRESULT (*SetTransform)();
	HRESULT (*SetRenderState)();
	HRESULT (*SetStreamSource)();
	HRESULT (*SetFVF)();
	HRESULT (*DrawPrimitive)();
	HRESULT (*DrawIndexedPrimitive)();

	HRESULT (*CreateVertexBuffer)();
	HRESULT (*VertexBufferLock)();
	HRESULT (*VertexBufferUnlock)();

	HRESULT (*SetIndices)();
	HRESULT (*CreateIndexBuffer)();
	HRESULT (*IndexBufferLock)();
	HRESULT (*IndexBufferUnlock)();
	HRESULT (*SetSamplerState)();

	HRESULT (*CreateVertexDeclaration)();
	HRESULT (*SetVertexDeclaration)();
	HRESULT (*SetSoftwareVertexProcessing)();
	HRESULT (*SetLight)();
	HRESULT (*LightEnable)();

	HRESULT (*CreateVertexShader)();
	HRESULT (*SetVertexShader)();
	HRESULT (*SetVertexShaderConstantF)();
	HRESULT (*CreatePixelShader)();
	HRESULT (*SetPixelShader)();
	HRESULT (*SetPixelShaderConstantF)();

	HRESULT (*DrawPrimitiveUP)();
	HRESULT (*DrawIndexedPrimitiveUP)();
	HRESULT (*SetVertexShaderConstantI)();
	HRESULT (*SetVertexShaderConstantB)();
	HRESULT (*SetPixelShaderConstantI)();
	HRESULT (*SetPixelShaderConstantB)();

	HRESULT (*Reset)();
	HRESULT (*SetMaterial)();
	HRESULT (*CreateTexture)();
	HRESULT (*SetTexture)();
	HRESULT (*SetTextureStageState)();

	HRESULT (*TransmitTextureData)();

	HRESULT (*CreateStateBlock)();
	HRESULT (*BeginStateBlock)();
	HRESULT (*EndStateBlock)();

	HRESULT (*StateBlockCapture)();
	HRESULT (*StateBlockApply)();

	HRESULT (*DeviceAddRef)();
	HRESULT (*DeviceRelease)();

	HRESULT (*SetViewport)();
	HRESULT (*SetNPatchMode)();

	HRESULT (*CreateCubeTexture)();
	HRESULT (*SetCubeTexture)();

	HRESULT (*GetSwapChain)();
	HRESULT (*SwapChainPresent)();
	HRESULT (*SetAutoGenFilterType)();
	void (*GenerateMipSubLevels)();

	HRESULT (*SetRenderTarget)();
	HRESULT (*SetDepthStencilSurface)();
	HRESULT (*TextureGetSurfaceLevel)();
	HRESULT (*SwapChainGetBackBuffer)();
	HRESULT (*GetDepthStencilSurface)();
	HRESULT (*CreateDepthStencilSurface)();
	HRESULT (*CubeGetCubeMapSurface)();

	// add by alan 2013/1/5
	HRESULT (*DIConfigureDevice)();
	HRESULT (*DICreateDevice)();
	HRESULT (*DIGetDeviceStatus)();
	HRESULT (*DIRunContorlPanel)();
	HRESULT (*DIDAcquire)();
	HRESULT (*DIDBuildActionMap)();
	HRESULT (*DIDCreateEffect)();
	HRESULT (*DIDCreateEffectObjects)();
	HRESULT (*DIDEnumEffects)();
	HRESULT (*DIDEscape)();
	HRESULT (*DIDGetCapabilities)();
	HRESULT (*DIDGetDeviceData)();
	HRESULT (*DIDGetDeviceInfo)();
	HRESULT (*DIDGetDeviceState)();
	HRESULT (*DIDRunControlPanel)();
	HRESULT (*DIDSetActionMap)();
	HRESULT (*DIDCooperativeLevel)();
	HRESULT (*DIDSetDataFormat)();
	HRESULT (*DIDUnacquire)();
	HRESULT (*CreateWindow)();
	HRESULT (*DDirectCreate)();
	HRESULT (*DIDirectInputCreate)();
	HRESULT (*DIDAddRef)();
	HRESULT (*DIDRelease)();
	HRESULT (*DIDSetProperty)();

	HRESULT (*TransmitSurface)();
	HRESULT (*D3DDeviceGetBackBuffer)();
	HRESULT (*D3DGetDeviceCaps)();
	HRESULT (*D3DDGetRenderTarget)();
	HRESULT (*D3DDSetScissorRect)();

	HRESULT (*SetVertexBufferFormat)();
	HRESULT (*SetDecimateResult)();

	HRESULT (*SetGammaRamp)();
	HRESULT (*NullInstruct)();
};

extern fptype fptable;

typedef void(*ApiFunc)();

extern FuncJumpTable funcs[];

HRESULT FakedCreateDevice();
HRESULT FakedBeginScene();
HRESULT FakedEndScene();
HRESULT FakedClear();
HRESULT FakedPresent();

HRESULT FakedSetTransform();
HRESULT FakedSetRenderState();
HRESULT FakedSetStreamSource();
HRESULT FakedSetFVF();
HRESULT FakedDrawPrimitive();
HRESULT FakedDrawIndexedPrimitive();

HRESULT FakedCreateVertexBuffer();
HRESULT FakedVertexBufferLock();
HRESULT FakedVertexBufferUnlock();

HRESULT FakedSetIndices();
HRESULT FakedCreateIndexBuffer();
HRESULT FakedIndexBufferLock();
HRESULT FakedIndexBufferUnlock();
HRESULT FakedSetSamplerState();

HRESULT FakedCreateVertexDeclaration();
HRESULT FakedSetVertexDeclaration();
HRESULT FakedSetSoftwareVertexProcessing();
HRESULT FakedSetLight();
HRESULT FakedLightEnable();

HRESULT FakedCreateVertexShader();
HRESULT FakedSetVertexShader();
HRESULT FakedSetVertexShaderConstantF();
HRESULT FakedCreatePixelShader();
HRESULT FakedSetPixelShader();
HRESULT FakedSetPixelShaderConstantF();

HRESULT FakedDrawPrimitiveUP();
HRESULT FakedDrawIndexedPrimitiveUP();
HRESULT FakedSetVertexShaderConstantI();
HRESULT FakedSetVertexShaderConstantB();
HRESULT FakedSetPixelShaderConstantI();
HRESULT FakedSetPixelShaderConstantB();

HRESULT FakedReset();
HRESULT FakedSetMaterial();
HRESULT FakedCreateTexture();
HRESULT FakedSetTexture();
HRESULT FakedSetTextureStageState();

HRESULT FakedTransmitTextureData();

HRESULT FakedCreateStateBlock();
HRESULT FakedBeginStateBlock();
HRESULT FakedEndStateBlock();

HRESULT FakedStateBlockCapture();
HRESULT FakedStateBlockApply();

HRESULT FakedDeviceAddRef();
HRESULT FakedDeviceRelease();

HRESULT FakedSetViewport();
HRESULT FakedSetNPatchMode();

HRESULT FakedCreateCubeTexture();
HRESULT FakedSetCubeTexture();

HRESULT FakedGetSwapChain();
HRESULT FakedSwapChainPresent();
HRESULT FakedSetAutoGenFilterType();
void FakedGenerateMipSubLevels();

HRESULT FakedSetRenderTarget();
HRESULT FakedSetDepthStencilSurface();
HRESULT FakedTextureGetSurfaceLevel();
HRESULT FakedSwapChainGetBackBuffer();
HRESULT FakedGetDepthStencilSurface();
HRESULT FakedCreateDepthStencilSurface();
HRESULT FakedCubeGetCubeMapSurface();

// add by alan , dinput api , 2013/1/4
HRESULT FakeDIConfigureDevice();
HRESULT FakeDICreateDevice();
HRESULT FakeDIGetDeviceStatus();
HRESULT FakeDIRunContorlPanel();

HRESULT FakeDIDAcquire();
HRESULT FakeDIDBuildActionMap();
HRESULT FakeDIDCreateEffect();
HRESULT FakeDIDCreateEffectObjects();
HRESULT FakeDIDEnumEffects();
HRESULT FakeDIDEscape();
HRESULT FakeDIDGetCapabilities();
HRESULT FakeDIDGetDeviceData();
HRESULT FakeDIDGetDeviceInfo();
HRESULT FakeDIDGetDeviceState();
HRESULT FakeDIDRunControlPanel();
HRESULT FakeDIDSetActionMap();
HRESULT FakeDIDCooperativeLevel();
HRESULT FakeDIDSetDataFormat();
HRESULT FakeDIDUnacquire();

HRESULT FakeDCreateWindow();
HRESULT FakeDDirectCreate();
HRESULT FakeDIDirectInputCreate();
HRESULT FakeDIDAddRef();
HRESULT FakeDIDRelease();
HRESULT FakeDIDSetProperty();
HRESULT FakeTransmitSurface();

HRESULT FakeD3DDeviceGetBackBuffer();
HRESULT FakeD3DGetDeviceCaps();
HRESULT FakeD3DDGetRenderTarget();
HRESULT FakeD3DDSetScissorRect();

HRESULT FakedSetVertexBufferFormat();
HRESULT FakedSetDecimateResult();

HRESULT FakedSetGammaRamp();
HRESULT FakeNullInstruct();

#endif