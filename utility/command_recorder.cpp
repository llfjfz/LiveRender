#include "command_recorder.h"
#include "opcode.h"
#include <algorithm>
using namespace std;
#include <iostream>

char mp[100][100] = {
	"CreateDevice",
	"BeginScene",
	"EndScene",
	"Clear",
	"Present",
	"SetTransform",
	"SetRenderState",
	"SetStreamSource",
	"SetFVF",
	"DrawPrimitive",
	"DrawIndexedPrimitive",
	"CreateVertexBuffer",
	"VertexBufferLock",
	"VertexBufferUnlock",
	"SetIndices",
	"CreateIndexBuffer",
	"IndexBufferLock",
	"IndexBufferUnlock",
	"SetSamplerState",
	"CreateVertexDeclaration",
	"SetVertexDeclaration",
	"SetSoftwareVertexProcessing",
	"SetLight",
	"LightEnable",
	"CreateVertexShader",
	"SetVertexShader",
	"SetVertexShaderConstantF",
	"CreatePixelShader",
	"SetPixelShader",
	"SetPixelShaderConstantF",
	"DrawPrimitiveUP",
	"DrawIndexedPrimitiveUP",
	"SetVertexShaderConstantI",
	"SetVertexShaderConstantB",
	"SetPixelShaderConstantI",
	"SetPixelShaderConstantB",
	"Reset",
	"SetMaterial",
	"CreateTexture",
	"SetTexture",
	"SetTextureStageState",
	"TransmitTextureData",
	"CreateStateBlock",
	"BeginStateBlock",
	"EndStateBlock",
	"StateBlockCapture",
	"StateBlockApply",
	"DeviceAddRef",
	"DeviceRelease",
	"SetViewport",
	"SetNPatchMode",
	"CreateCubeTexture",
	"SetCubeTexture",
	"GetSwapChain",
	"SwapChainPresent",
	"TextureSetAutoGenFilterType",
	"TextureGenerateMipSubLevels",
	"SetRenderTarget",
	"SetDepthStencilSurface",
	"TextureGetSurfaceLevel",
	"SwapChainGetBackBuffer",
	"GetDepthStencilSurface",
	"CreateDepthStencilSurface",
	"CubeGetCubeMapSurface",
	"DIConfigureDevices",
	"DICreateDevice",
	"DIGetDeviceStatus",
	"DIRunControlPanel",
	"DIDAcquire",
	"DIDBuildActionMap",
	"DIDCreateEffect",
	"DIDEnumCreateEffectObjects",
	"DIDEnumEffects",
	"DIDEscape",
	"DIDGetCapabilities",
	"DIDGetDeviceData",
	"DIDGetDeviceInfo",
	"DIDGetDeviceState",
	"DIDRunControlPanel",
	"DIDSetActionMap",
	"DIDSetCooperativeLevel",
	"DIDSetDataFormat",
	"DIDUnacquire",
	"CreateWindow",
	"DirectCreate",
	"DirectInputCreate",
	"DIDAddRef",
	"DIDRelease",
	"DIDSetProperty",
	"TransmitSurfaceData",
	"D3DDeviceGetBackBuffer",
	"D3DGetDeviceCaps",
	"D3DDGetRenderTarget",
	"D3DDSetScissorRect",
	"SetVertexBufferFormat",
	"SetDecimateResult"
};

bool cmp_by_cnt(RecordType A, RecordType B) {
	return A.count > B.count;
}

bool cmp_by_len(RecordType A, RecordType B) {
	return A.length > B.length;
}

CommandRecorder::CommandRecorder() {
	//Log::log("Recorder::Recorder() called\n");
	memset(info, 0, sizeof info);
	for(int i=0; i<1010; ++i) info[i].id = i;

	cmd_cnt = 0;
	inst_cnt = 0;
	frame_cnt = 0;
	frame_len = 0;
	total_len = 0;
}

void CommandRecorder::cache_hit(int op_code) {
	info[op_code].hit_cnt++;
}

void CommandRecorder::cache_miss(int op_code) {
	info[op_code].miss_cnt++;
}

void CommandRecorder::add_record(int op_code, int len) {

	//if(op_code == VertexBufferUnlock_Opcode || op_code == SetVertexShaderConstantF_Opcode || op_code == SetPixelShaderConstantF_Opcode)
		//return;

	info[op_code].count++;
	info[op_code].length += len;
	cmd_cnt++;

	if(op_code == BeginScene_Opcode) {
		frame_cnt++;
	}
}

void CommandRecorder::add_lengh(int length) {
	total_len += length;
}

void CommandRecorder::print_info() {
	sort(info, info + 1010, cmp_by_len);
	//sort(info, info + 1010, cmp_by_cnt);

	FILE* f = fopen("stat_command.log", "w");

	fprintf(f, "=====================================================\nCommand Statistic\n");
	fprintf(f, "frame count:\t%I64d\n", frame_cnt);
	fprintf(f, "total length:\t%I64d\n", total_len);
	fprintf(f, "size per frame:\t%.2lf\n", total_len * 1.0 / frame_cnt);
	fprintf(f, "cmd_cnt per frame:\t%.2lf\n", cmd_cnt * 1.0 / frame_cnt);

	for(int i=0; i<1010; ++i) {
		if(info[i].count == 0) break;
		fprintf(f, "op_code=%d, name=%s, cache hit=%.2lf%%, length=%I64d, count=%d, ave=%.3lf\n", 
			info[i].id, 
			mp[info[i].id], 
			info[i].hit_cnt ? (info[i].hit_cnt * 100.0 / (info[i].hit_cnt + info[i].miss_cnt)) : 0, 
			info[i].length, info[i].count, info[i].length * 1.0f / info[i].count);
	}
	fprintf(f, "=====================================================\n");

	fclose(f);
}
