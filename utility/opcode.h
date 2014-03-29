#ifndef __OPCODE__
#define __OPCODE__

#define OBJECTTYPE_D3D9						0
#define OBJECTTYPE_D3DDEVICE9				1
#define OBJECTTYPE_D3DVERTEXBUFFER9			2

#define CreateDevice_Opcode					0
#define BeginScene_Opcode					1
#define EndScene_Opcode						2
#define Clear_Opcode						3
#define Present_Opcode						4

#define SetTransform_Opcode					5
#define SetRenderState_Opcode				6
#define SetStreamSource_Opcode				7
#define SetFVF_Opcode						8
#define DrawPrimitive_Opcode				9
#define DrawIndexedPrimitive_Opcode			10

#define CreateVertexBuffer_Opcode			11
#define VertexBufferLock_Opcode				12
#define VertexBufferUnlock_Opcode			13

#define SetIndices_Opcode					14
#define CreateIndexBuffer_Opcode			15
#define IndexBufferLock_Opcode				16
#define IndexBufferUnlock_Opcode			17
#define SetSamplerState_Opcode				18

#define CreateVertexDeclaration_Opcode		19
#define SetVertexDeclaration_Opcode			20
#define SetSoftwareVertexProcessing_Opcode	21
#define SetLight_Opcode						22
#define LightEnable_Opcode					23

#define CreateVertexShader_Opcode			24
#define SetVertexShader_Opcode				25
#define SetVertexShaderConstantF_Opcode		26
#define CreatePixelShader_Opcode			27
#define SetPixelShader_Opcode				28
#define SetPixelShaderConstantF_Opcode		29

#define DrawPrimitiveUP_Opcode				30
#define DrawIndexedPrimitiveUP_Opcode		31
#define SetVertexShaderConstantI_Opcode		32
#define SetVertexShaderConstantB_Opcode		33
#define SetPixelShaderConstantI_Opcode		34
#define SetPixelShaderConstantB_Opcode		35

#define Reset_Opcode						36
#define SetMaterial_Opcode					37
#define CreateTexture_Opcode				38
#define SetTexture_Opcode					39
#define SetTextureStageState_Opcode			40

#define TransmitTextureData_Opcode			41

#define CreateStateBlock_Opcode				42
#define BeginStateBlock_Opcode				43
#define EndStateBlock_Opcode				44

#define StateBlockCapture_Opcode			45
#define StateBlockApply_Opcode				46

#define DeviceAddRef_Opcode					47
#define DeviceRelease_Opcode				48

#define SetViewport_Opcode					49
#define SetNPatchMode_Opcode				50

#define CreateCubeTexture_Opcode			51
#define SetCubeTexture_Opcode				52

#define GetSwapChain_Opcode					53
#define SwapChainPresent_Opcode				54
#define TextureSetAutoGenFilterType_Opcode			55
#define TextureGenerateMipSubLevels_Opcode			56
#define SetRenderTarget_Opcode				57
#define SetDepthStencilSurface_Opcode		58
#define TextureGetSurfaceLevel_Opcode		59
#define SwapChainGetBackBuffer_Opcode		60
#define GetDepthStencilSurface_Opcode		61
#define CreateDepthStencilSurface_Opcode	62
#define CubeGetCubeMapSurface_Opcode		63

// add by alan, the input api code
// for the direct input 8
#define DIConfigureDevices_Opcode         64
#define DICreateDevice_Opcode 65
#define DIGetDeviceStatus_Opcode 66
#define DIRunControlPanel_Opcode 67

// for the direct input device 8
#define DIDAcquire_Opcode 68
#define DIDBuildActionMap_Opcode 69
#define DIDCreateEffect_Opcode 70
#define DIDEnumCreateEffectObjects_Opcode 71
#define DIDEnumEffects_Opcode 72
#define DIDEscape_Opcode 73   //ignore for now
#define DIDGetCapabilities_Opcode 74  //IG
#define DIDGetDeviceData_Opcode 75
#define DIDGetDeviceInfo_Opcode 76
#define DIDGetDeviceState_Opcode 77
#define DIDRunControlPanel_Opcode 78
#define DIDSetActionMap_Opcode 79
#define DIDSetCooperativeLevel_Opcode 80
#define DIDSetDataFormat_Opcode 81
#define DIDUnacquire_Opcode 82

// for the direct input effect, not work for now
// special function
#define CreateWindow_Opcode 83
#define DirectCreate_Opcode 84
#define DirectInputCreate_Opcode 85

#define DIDAddRef_Opcode 86
#define DIDRelease_Opcode 87
#define DIDSetProperty_Opcode 88
#define TransmitSurfaceData_Opcode 89

// add by alan, 2013/7/24 morning
#define D3DDeviceGetBackBuffer_Opcode 90
#define D3DGetDeviceCaps_Opcode 91
#define D3DDGetRenderTarget_Opcode 92
#define D3DDSetScissorRect_Opcode 93
// define the release functions for all Interfaces

#define SetVertexBufferFormat_Opcode	94
#define SetDecimateResult_Opcode		95

#define SetGammaRamp_Opcode				96
#define NULLINSTRUCT_Opcode					97

#define MaxSizeUntilNow_Opcode				98


#endif