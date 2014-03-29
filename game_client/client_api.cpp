#include "game_client.h"

fptype fptable;
#define X(a) { #a, (ApiFunc*)&fptable.##a }

FuncJumpTable funcs[] = {
	X(CreateDevice),
	X(BeginScene),
	X(EndScene),
	X(Clear),
	X(Present),

	X(SetTransform),
	X(SetRenderState),
	X(SetStreamSource),
	X(SetFVF),
	X(DrawPrimitive),
	X(DrawIndexedPrimitive),

	X(CreateVertexBuffer),
	X(VertexBufferLock),
	X(VertexBufferUnlock),

	X(SetIndices),
	X(CreateIndexBuffer),
	X(IndexBufferLock),
	X(IndexBufferUnlock),
	X(SetSamplerState),

	X(CreateVertexDeclaration),
	X(SetVertexDeclaration),
	X(SetSoftwareVertexProcessing),
	X(SetLight),
	X(LightEnable),

	X(CreateVertexShader),
	X(SetVertexShader),
	X(SetVertexShaderConstantF),
	X(CreatePixelShader),
	X(SetPixelShader),
	X(SetPixelShaderConstantF),

	X(DrawPrimitiveUP),
	X(DrawIndexedPrimitiveUP),
	X(SetVertexShaderConstantI),
	X(SetVertexShaderConstantB),
	X(SetPixelShaderConstantI),
	X(SetPixelShaderConstantB),

	X(Reset),
	X(SetMaterial),
	X(CreateTexture),
	X(SetTexture),
	X(SetTextureStageState),

	X(TransmitTextureData),

	X(CreateStateBlock),
	X(BeginStateBlock),
	X(EndStateBlock),

	X(StateBlockCapture),
	X(StateBlockApply),

	X(DeviceAddRef),
	X(DeviceRelease),

	X(SetViewport),
	X(SetNPatchMode),

	X(CreateCubeTexture),
	X(SetCubeTexture),

	X(GetSwapChain),
	X(SwapChainPresent),
	X(SetAutoGenFilterType),
	X(GenerateMipSubLevels),

	X(SetRenderTarget),
	X(SetDepthStencilSurface),
	X(TextureGetSurfaceLevel),
	X(SwapChainGetBackBuffer),
	X(GetDepthStencilSurface),
	X(CreateDepthStencilSurface),
	X(CubeGetCubeMapSurface),

	// add by alan 2013/1/5
	X(DIConfigureDevice),
	X(DICreateDevice),
	X(DIGetDeviceStatus),
	X(DIRunContorlPanel),
	X(DIDAcquire),
	X(DIDBuildActionMap),
	X(DIDCreateEffect),
	X(DIDCreateEffectObjects),
	X(DIDEnumEffects),
	X(DIDEscape),
	X(DIDGetCapabilities),
	X(DIDGetDeviceData),
	X(DIDGetDeviceInfo),
	X(DIDGetDeviceState),
	X(DIDRunControlPanel),
	X(DIDSetActionMap),
	X(DIDCooperativeLevel),
	X(DIDSetDataFormat),
	X(DIDUnacquire),
	X(CreateWindow),
	X(DDirectCreate),
	X(DIDirectInputCreate),
	X(DIDAddRef),
	X(DIDRelease),
	X(DIDSetProperty),
	X(TransmitSurface),

	X(D3DDeviceGetBackBuffer),
	X(D3DGetDeviceCaps),
	X(D3DDGetRenderTarget),
	X(D3DDSetScissorRect),

	X(SetVertexBufferFormat),
	X(SetDecimateResult),

	X(SetGammaRamp),
	X(NullInstruct),

	{NULL, NULL}
};

#undef X


void init_fptable() {
	fptable.CreateWindow = FakeDCreateWindow;
	fptable.DDirectCreate = FakeDDirectCreate;

	fptable.CreateDevice = FakedCreateDevice;
	fptable.BeginScene = FakedBeginScene;
	fptable.EndScene = FakedEndScene;
	fptable.Clear = FakedClear;
	fptable.Present = FakedPresent;

	fptable.SetTransform = FakedSetTransform;
	fptable.SetRenderState = FakedSetRenderState;
	fptable.SetStreamSource = FakedSetStreamSource;
	fptable.SetFVF = FakedSetFVF;
	fptable.DrawPrimitive = FakedDrawPrimitive;
	fptable.DrawIndexedPrimitive = FakedDrawIndexedPrimitive;

	fptable.CreateVertexBuffer = FakedCreateVertexBuffer;
	fptable.VertexBufferLock = FakedVertexBufferLock;
	fptable.VertexBufferUnlock = FakedVertexBufferUnlock;

	fptable.SetIndices = FakedSetIndices;
	fptable.CreateIndexBuffer = FakedCreateIndexBuffer;
	fptable.IndexBufferLock = FakedIndexBufferLock;
	fptable.IndexBufferUnlock = FakedIndexBufferUnlock;
	fptable.SetSamplerState = FakedSetSamplerState;

	fptable.CreateVertexDeclaration = FakedCreateVertexDeclaration;
	fptable.SetVertexDeclaration = FakedSetVertexDeclaration;
	fptable.SetSoftwareVertexProcessing = FakedSetSoftwareVertexProcessing;
	fptable.SetLight = FakedSetLight;
	fptable.LightEnable = FakedLightEnable;

	fptable.CreateVertexShader = FakedCreateVertexShader;
	fptable.SetVertexShader = FakedSetVertexShader;
	fptable.SetVertexShaderConstantF = FakedSetVertexShaderConstantF;
	fptable.CreatePixelShader = FakedCreatePixelShader;
	fptable.SetPixelShader = FakedSetPixelShader;
	fptable.SetPixelShaderConstantF = FakedSetPixelShaderConstantF;

	fptable.DrawPrimitiveUP = FakedDrawPrimitiveUP;
	fptable.DrawIndexedPrimitiveUP = FakedDrawIndexedPrimitiveUP;
	fptable.SetVertexShaderConstantI = FakedSetVertexShaderConstantI;
	fptable.SetVertexShaderConstantB = FakedSetVertexShaderConstantB;
	fptable.SetPixelShaderConstantI = FakedSetPixelShaderConstantI;
	fptable.SetPixelShaderConstantB = FakedSetPixelShaderConstantB;

	fptable.Reset = FakedReset;
	fptable.SetMaterial = FakedSetMaterial;
	fptable.CreateTexture = FakedCreateTexture;
	fptable.SetTexture = FakedSetTexture;
	fptable.SetTextureStageState = FakedSetTextureStageState;

	fptable.TransmitTextureData = FakedTransmitTextureData;

	fptable.CreateStateBlock = FakedCreateStateBlock;
	fptable.BeginStateBlock = FakedBeginStateBlock;
	fptable.EndStateBlock = FakedEndStateBlock;

	fptable.StateBlockCapture = FakedStateBlockCapture;
	fptable.StateBlockApply = FakedStateBlockApply;

	fptable.DeviceAddRef = FakedDeviceAddRef;
	fptable.DeviceRelease = FakedDeviceRelease;

	fptable.SetViewport = FakedSetViewport;
	fptable.SetNPatchMode = FakedSetNPatchMode;

	fptable.CreateCubeTexture = FakedCreateCubeTexture;
	fptable.SetCubeTexture = FakedSetCubeTexture;

	fptable.GetSwapChain = FakedGetSwapChain;
	fptable.SwapChainPresent = FakedSwapChainPresent;
	fptable.SetAutoGenFilterType = FakedSetAutoGenFilterType;
	fptable.GenerateMipSubLevels = FakedGenerateMipSubLevels;

	fptable.SetRenderTarget = FakedSetRenderTarget;
	fptable.SetDepthStencilSurface = FakedSetDepthStencilSurface;
	fptable.TextureGetSurfaceLevel = FakedTextureGetSurfaceLevel;
	fptable.SwapChainGetBackBuffer = FakedSwapChainGetBackBuffer;
	fptable.GetDepthStencilSurface = FakedGetDepthStencilSurface;
	fptable.CreateDepthStencilSurface = FakedCreateDepthStencilSurface;
	fptable.CubeGetCubeMapSurface = FakedCubeGetCubeMapSurface;

	/*
	// direct input concered functions
	fptable.DIConfigureDevice= FakeDIConfigureDevice;
	fptable.DICreateDevice = FakeDICreateDevice;
	fptable.DIDAcquire = FakeDIDAcquire;
	fptable.DIDBuildActionMap = FakeDIDBuildActionMap;
	fptable.DIDCreateEffect = FakeDIDCreateEffect;
	fptable.DIDCreateEffectObjects = FakeDIDCreateEffectObjects;
	fptable.DIDEnumEffects = FakeDIDEnumEffects;
	fptable.DIDEscape = FakeDIDEscape;
	fptable.DIDGetCapabilities = FakeDIDGetCapabilities;
	fptable.DIDGetDeviceData = FakeDIDGetDeviceData;
	fptable.DIDGetDeviceInfo = FakeDIDGetDeviceInfo;
	fptable.DIDGetDeviceState = FakeDIDGetDeviceState;
	fptable.DIDRunControlPanel = FakeDIDRunControlPanel;
	fptable.DIDSetActionMap = FakeDIDSetActionMap;
	fptable.DIDCooperativeLevel = FakeDIDCooperativeLevel;
	fptable.DIDSetDataFormat = FakeDIDSetDataFormat;
	fptable.DIDUnacquire = FakeDIDUnacquire;


	fptable.DIDirectInputCreate = FakeDIDirectInputCreate;
	fptable.DIDAddRef = FakeDIDAddRef;
	fptable.DIDRelease = FakeDIDRelease;
	fptable.DIDSetProperty = FakeDIDSetProperty;
	*/

	fptable.TransmitSurface = FakeTransmitSurface;

	fptable.D3DDeviceGetBackBuffer = FakeD3DDeviceGetBackBuffer;
	fptable.D3DGetDeviceCaps = FakeD3DGetDeviceCaps;
	fptable.D3DDGetRenderTarget = FakeD3DDGetRenderTarget;
	fptable.D3DDSetScissorRect = FakeD3DDSetScissorRect;

	fptable.SetVertexBufferFormat = FakedSetVertexBufferFormat;
	fptable.SetDecimateResult = FakedSetDecimateResult;

	fptable.SetGammaRamp = FakedSetGammaRamp;
	fptable.NullInstruct = FakeNullInstruct;
}


