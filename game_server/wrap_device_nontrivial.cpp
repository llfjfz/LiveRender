#include "game_server.h"
#include "wrap_direct3d9.h"
#include "wrap_direct3ddevice9.h"
#include "wrap_direct3dvertexbuffer9.h"
#include "wrap_direct3dindexbuffer9.h"
#include "wrap_direct3dvertexdeclaration9.h"
#include "wrap_direct3dvertexshader9.h"
#include "wrap_direct3dpixelshader9.h"
#include "wrap_direct3dtexture9.h"
#include "wrap_direct3dstateblock9.h"
#include "wrap_direct3dcubetexture9.h"
#include "wrap_direct3dswapchain9.h"
#include "wrap_direct3dsurface9.h"
#include "wrap_direct3dvolumetexture9.h"
#include "opcode.h"
#include "ymesh.h"

#define DELAY_TO_DRAW


STDMETHODIMP WrapperDirect3DDevice9::DrawPrimitive(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount) {
	Log::log("WrapperDirect3DDevice9::DrawPrimitive(), PrimitiveType=%d, startVertex=%d, primitiveCount=%d\n", PrimitiveType, StartVertex, PrimitiveCount);

	Log::log("cur_vbs:\n");
	for(int i=0; i<Source_Count; ++i) {
		Log::log("%d ", cur_vbs_[i]);
	}
	
	YMesh* y_mesh = NULL;
	if(true) {
		WrapperDirect3DVertexBuffer9* vb0 = cur_vbs_[0];

		assert(cur_vbs_[0]);
		
		//get mesh object, and set the vb, ib, decl
		if(vb0->y_mesh_ == NULL) {
			y_mesh = vb0->y_mesh_ = new YMesh(this);
		
			y_mesh->set_leader_vb(vb0->GetId());
			y_mesh->set_stream_source(cur_vbs_);
			y_mesh->set_indices(NULL);
			y_mesh->set_declaration(cur_decl_);
			y_mesh->set_draw_type(DrawMethod::DrawPrimitive, PrimitiveType);
		}
		else {
			y_mesh = vb0->y_mesh_;
		}
	}


	y_mesh->decimate();
	y_mesh->render(0, 0, 0, 0);

	if(y_mesh->decision_ == NO_NEED) {
		cs.begin_command(DrawPrimitive_Opcode, id);
		cs.write_char(PrimitiveType);
		cs.write_uint(StartVertex);
		cs.write_uint(PrimitiveCount);
		cs.end_command();
	}
	/*
	cur_ib_ = NULL;
	cur_decl_ = NULL;
	
	*/

if(enableRender)
	return m_device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
else
	return D3D_OK;

}

STDMETHODIMP WrapperDirect3DDevice9::DrawIndexedPrimitive(THIS_ D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount) {
	Log::log("WrapperDirect3DDevice9::DrawIndexedPrimitive() device id:%d, type=%d, baseVertexIndex=%d, MinVertexIndex=%d, NumVertex=%d, startIndex=%d, count=%d\n", id, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

	assert(cur_ib_);

	//Log::log("cur_vbs:\n");
	/*
	for(int i=0; i<Source_Count; ++i) {
		Log::slog("0x%x ", cur_vbs_[i]);
	}
	*/
	//Log::slog("\n");
	Log::log("cur_ib: %d, cur_vb: %d\n", cur_ib_->GetID(), cur_vbs_[0]->GetId());

	//if(cur_ib_->GetID() == cs.config_->ban_ib_ && cur_vbs_[0]->GetId() == 1428) return D3D_OK;
	//if(cur_ib_->GetID() == 1454 || cur_ib_->GetID() == 1015) return D3D_OK;
	//if(cur_ib_->GetID() == 1566) return D3D_OK;

	/*
	if(Type != D3DPT_TRIANGLELIST) {
		assert(false);
	}
	*/

	YMesh* y_mesh = NULL;
	if(cur_ib_) {

		y_mesh = cur_ib_->get_y_mesh(cur_vbs_[0]->GetId(), BaseVertexIndex, startIndex);

		if(y_mesh == NULL) {
			y_mesh = new YMesh(this);
			cur_ib_->put_y_mesh(cur_vbs_[0]->GetId(), BaseVertexIndex, startIndex, y_mesh);

			Log::log("cur_ib_=%d, new mesh\n", cur_ib_->GetID());

			assert(cur_vbs_[0]);
			y_mesh->set_leader_vb(cur_vbs_[0]->GetId());
			y_mesh->set_indices(cur_ib_);
			y_mesh->set_declaration(cur_decl_);
			y_mesh->set_stream_source(cur_vbs_);
			y_mesh->set_draw_type(DrawMethod::DrawIndexedPrimitive, Type);

			y_mesh->set_index_params(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		}
		else {
			Log::log("cur_ib_=%d,  old mesh\n", cur_ib_->GetID());
			y_mesh->set_index_params(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
			//y_mesh = cur_ib_->get_y_mesh(cur_vbs_[0]->GetId(), BaseVertexIndex, MinVertexIndex, startIndex);
		}
		
		//y_mesh->dump_mesh();
	}

	

	y_mesh->decimate();

	
	y_mesh->render(BaseVertexIndex, MinVertexIndex, startIndex, primCount);

	//the mesh doesn't do the decimation, render yourself.
	if(y_mesh->decision_ == NO_NEED) {
		cs.begin_command(DrawIndexedPrimitive_Opcode, id);
		cs.write_char(Type);
		cs.write_int(BaseVertexIndex);
		cs.write_int(MinVertexIndex);
		cs.write_uint(NumVertices);
		cs.write_uint(startIndex);
		cs.write_uint(primCount);
		cs.end_command();
	}

	/*
	cur_ib_ = NULL;
	cur_decl_ = NULL;
	
	*/

if(enableRender)
	return m_device->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
else
	return D3D_OK;

}

UINT arr[4];

STDMETHODIMP WrapperDirect3DDevice9::DrawPrimitiveUP(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) {
	Log::log("WrapperDirect3DDevice9::DrawPrimitiveUP(), type=%d, count=%d\n", PrimitiveType, PrimitiveCount);

	int VertexCount = 0;
	switch(PrimitiveType) {
	case D3DPT_POINTLIST:
		VertexCount = PrimitiveCount;
		break;
	case D3DPT_LINELIST:
		VertexCount = PrimitiveCount * 2;
		break;
	case D3DPT_LINESTRIP:
		VertexCount = PrimitiveCount + 1;
		break;
	case D3DPT_TRIANGLELIST:
		VertexCount = PrimitiveCount * 3;
	case D3DPT_TRIANGLESTRIP:
	case D3DPT_TRIANGLEFAN:
		VertexCount = PrimitiveCount * 2;
		break;
	default:
		VertexCount = 0;
		break;
	}

	cs.begin_command(DrawPrimitiveUP_Opcode, id);
	
	/*
	cs.write_uint(PrimitiveType);
	cs.write_uint(PrimitiveCount);
	cs.write_uint(VertexCount);
	cs.write_uint(VertexStreamZeroStride);
	*/

	
	arr[0] = PrimitiveType;
	arr[1] = PrimitiveCount;
	arr[2] = VertexCount;
	arr[3] = VertexStreamZeroStride;
	cs.write_vec(DrawPrimitiveUP_Opcode, (float*)&arr);
	

	//assert(VertexCount * VertexStreamZeroStride + 16 < 200);

	//cs.write_byte_arr((char*)pVertexStreamZeroData, VertexCount * VertexStreamZeroStride);

	cs.write_vec(DrawPrimitiveUP_Opcode, (float*)pVertexStreamZeroData, VertexCount * VertexStreamZeroStride);

	cs.end_command();

	

if(enableRender)
	return m_device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
else
	return D3D_OK;

}

STDMETHODIMP WrapperDirect3DDevice9::DrawIndexedPrimitiveUP(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) {
	Log::log("WrapperDirect3DDevice9::DrawIndexedPrimitiveUP(), type=%d, count=%d\n", PrimitiveType, PrimitiveCount);

	int IndexSize = 2;
	if(IndexDataFormat == D3DFMT_INDEX32) IndexSize = 4;

	cs.begin_command(DrawIndexedPrimitiveUP_Opcode, id);
	cs.write_uint(PrimitiveType);
	cs.write_uint(MinVertexIndex);
	cs.write_uint(NumVertices);
	cs.write_uint(PrimitiveCount);
	cs.write_uint(IndexDataFormat);
	cs.write_uint(VertexStreamZeroStride);
	cs.write_byte_arr((char*)pIndexData, NumVertices * IndexSize);
	cs.write_byte_arr((char*)pVertexStreamZeroData, NumVertices * VertexStreamZeroStride);
	cs.end_command();

if(enableRender)
	return m_device->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
else
	return D3D_OK;

}

STDMETHODIMP WrapperDirect3DDevice9::CreateVertexDeclaration(THIS_ CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl) {
	Log::log("WrapperDirect3DDevice9::CreateVertexDeclaration() called\n");


	int ve_cnt = 0;
	D3DVERTEXELEMENT9 end = D3DDECL_END();

	// create the vertex declaration
	WrapperDirect3DVertexDeclaration9::ins_count++;
	LPDIRECT3DVERTEXDECLARATION9 base_vd = NULL;
	HRESULT hr = m_device->CreateVertexDeclaration(pVertexElements, &base_vd);
	WrapperDirect3DVertexDeclaration9 * vd = NULL;
	if(SUCCEEDED(hr)) {
		vd = new WrapperDirect3DVertexDeclaration9(base_vd, WrapperDirect3DVertexDeclaration9::ins_count - 1);
		*ppDecl = dynamic_cast<IDirect3DVertexDeclaration9*>(vd);
	}
	else {
		Log::log("WrapperDirect3DDevice9::CreateVertexDeclaration() failed\n");
	}

	while(true) {
		ve_cnt++;
		if(pVertexElements[ve_cnt].Type == end.Type && pVertexElements[ve_cnt].Method == end.Method && pVertexElements[ve_cnt].Offset == end.Offset && pVertexElements[ve_cnt].Stream == end.Stream && pVertexElements[ve_cnt].Usage == end.Usage && pVertexElements[ve_cnt].UsageIndex == end.UsageIndex) break;
	}

	cs.begin_command(CreateVertexDeclaration_Opcode, id);
	cs.write_int(WrapperDirect3DVertexDeclaration9::ins_count-1);
	cs.write_int(ve_cnt + 1);
	cs.write_byte_arr((char*)pVertexElements, sizeof(D3DVERTEXELEMENT9) * (ve_cnt + 1));
	cs.end_command();

	if(vd){
		Log::log("WrapperDirect3DDevice::CreateVertexDeclaration() set numElement:%d and pDecl:%d\n",ve_cnt + 1, pVertexElements);
		//set the VERTEXELEMENT and numElemts
		vd->numElements = ve_cnt;
		//vd->pDecl = pVertexElements; // check when use, the pointer may be NULL
		vd->pDecl = (D3DVERTEXELEMENT9 *)malloc(sizeof(D3DVERTEXELEMENT9) * (ve_cnt +1));
		memcpy(vd->pDecl, (const void *)pVertexElements, sizeof(D3DVERTEXELEMENT9) * (ve_cnt +1));
	}

	return hr;
}


STDMETHODIMP WrapperDirect3DDevice9::SetVertexDeclaration(THIS_ IDirect3DVertexDeclaration9* pDecl) {
	Log::log("WrapperDirect3DDevice9::SetVertexDeclaration() called\n");
	
	cur_decl_ = (WrapperDirect3DVertexDeclaration9*)pDecl;

	
	if(pDecl == NULL) {
		Log::log("WrapperDirect3DDevice9::SetVertexDeclaration() pDecl is NULL\n");
	
		cs.begin_command(SetVertexDeclaration_Opcode, id);
		cs.write_short(-1);
		cs.end_command();

		return m_device->SetVertexDeclaration(pDecl);
	}

	cs.begin_command(SetVertexDeclaration_Opcode, id);
	cs.write_short(((WrapperDirect3DVertexDeclaration9*)pDecl)->GetID());
	cs.end_command();

	HRESULT hh= m_device->SetVertexDeclaration(((WrapperDirect3DVertexDeclaration9*)pDecl)->GetVD9());

	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::SetStreamSource(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride) {
	Log::log("WrapperDirect3DDevice9::SetStreamSource() invoke, StreamNumber:%d, vb:%d, OffsetInBytes:%d, stride:%d\n", StreamNumber, pStreamData, OffsetInBytes, Stride);

	assert(StreamNumber < Source_Count);


	if(pStreamData == NULL) {
		Log::log("Special SetStreamSource stream:%d\n", StreamNumber);
		
		cur_vbs_[StreamNumber] = NULL;

		cs.begin_command(SetStreamSource_Opcode, id);
		cs.write_uint(StreamNumber);
		cs.write_int(-1);
		cs.write_uint(0);
		cs.write_uint(0);
		cs.end_command();

		Log::log("WrapperDirect3DDevice9::SetStreamSource() pStreamData is NULL\n");

		return m_device->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
	}

	WrapperDirect3DVertexBuffer9* wvb = (WrapperDirect3DVertexBuffer9*)pStreamData;
	cur_vbs_[StreamNumber] = wvb;
	
	Log::log("Special SetStreamSource vid:%d, stream:%d, OffsetInBytes:%d, stride:%d\n", wvb->GetId(), StreamNumber,OffsetInBytes, Stride);
	
	wvb->streamNumber = StreamNumber;
	wvb->stride =Stride;
	wvb->offsetInBytes = OffsetInBytes;

	HRESULT hh = m_device->SetStreamSource(StreamNumber, ((WrapperDirect3DVertexBuffer9*)pStreamData)->GetVB9(), OffsetInBytes, Stride);
	return hh;
}


STDMETHODIMP WrapperDirect3DDevice9::SetIndices(THIS_ IDirect3DIndexBuffer9* pIndexData) {
	Log::log("WrapperDirect3DDevice9::SetIndices(), ib=%d\n", pIndexData);

	

	if(pIndexData == NULL) {
		Log::log("WrapperDirect3DDevice9::SetIndices() pIndexData is NULL\n");

		cur_ib_ = NULL;

		cs.begin_command(SetIndices_Opcode, id);
		cs.write_short(-1);
		cs.end_command();
		
		return m_device->SetIndices(pIndexData);
	}

	WrapperDirect3DIndexBuffer9* wib = (WrapperDirect3DIndexBuffer9*)pIndexData;
	Log::log("WrapperDirect3DDevice9::SetIndices() index buffer id:%d\n", wib->GetID());

	cur_ib_ = wib;

	HRESULT hh = m_device->SetIndices(((WrapperDirect3DIndexBuffer9*)pIndexData)->GetIB9());
	return hh;
}


