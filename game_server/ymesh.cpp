#include "game_server.h"
#include "ymesh.h"
#include "wrap_direct3ddevice9.h"
#include "float_helper.h"

vector<YMesh*> YMesh::mesh_list;

int YMesh::decimate_count_ = 0;

YMesh::YMesh(WrapperDirect3DDevice9* device): bs_(1000) {
	for(int i=0; i<Source_Count; ++i) {
		source_format_[i].clear();
		vbs_[i] = NULL;
		format_sum_[i] = 0;
	}

	config_low_bound_ = cs.config_->mesh_low_;
	config_up_bound_ = cs.config_->mesh_up_;
	config_ratio_ = cs.config_->mesh_ratio_;

	assert(config_low_bound_ && config_up_bound_ && fabs(config_ratio_) > eps);


	leader_vb_id = -1;

	device_ = device;

	decl_ = NULL;
	ib_ = NULL;

	pos_vb_ = NULL;

	decimate_ib_ = NULL;

	mesh_dumped_ = false;
	pos_dumped_ = false;
	face_dumped_ = false;

	decimated_ = false;

	draw_method_ = NONE;
	decision_ = INIT;

	cur_prim_count_ = 0;

	slim_ = NULL;

	update_count_ = 0;
	data_bytes_ = 0;
	init_vertex_count_ = 0;
	remain_vertices_.clear();
	indices_.clear();

	memset(select_result_sent_, 0, sizeof select_result_sent_);

	mesh_list.push_back(this);
}

bool YMesh::vb_changed(WrapperDirect3DVertexBuffer9* cur_vbs[]) {
	int changed = false;
	for(int i=0; i<Source_Count; ++i) {
		Log::log("%d %d\n", vbs_[i] ? vbs_[i]->GetId() : -1, cur_vbs[i] ? cur_vbs[i]->GetId() : -1);
		if(vbs_[i] != cur_vbs[i]) {
			changed = true;
			break;
		}
	}

	if(changed) {
		Log::log("YMesh::vb_changed(), ib=%d\n", ib_->GetID());
	}

	return changed;
}

void YMesh::set_index_params(D3DPRIMITIVETYPE Type, INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount) {
	type_ = Type;
	base_vertex_index_ = BaseVertexIndex;
	min_vertex_index_ = MinVertexIndex;
	num_vertices_ = NumVertices;
	start_index_ = startIndex;
	prim_count_ = primCount;
}

int YMesh::get_vertex_count() {
	Log::log("YMesh::get_vertex_count() called\n");
	return slim_->get_vert_num();
}

int YMesh::get_face_count() {
	Log::log("YMesh::get_face_count() called\n");
	return slim_->faces.size();
}

void YMesh::set_indices(WrapperDirect3DIndexBuffer9* ib) {
	
	ib_ = ib;

	Log::log("YMesh::set_indices(WrapperDirect3DIndexBuffer9* ib), ib=%d, ib_=%d\n", ib, ib_);
}

void YMesh::set_draw_type(DrawMethod method, D3DPRIMITIVETYPE type) {
	Log::log("YMesh::set_draw_type() called\n");
	draw_method_ = method;
	primitive_type_ = type;
}

void YMesh::clear_stream_source() {
	for(int i=0; i<Source_Count; ++i) {
		vbs_[i] = NULL;
	}
}

void YMesh::set_stream_source(WrapperDirect3DVertexBuffer9* sources[]) {
	Log::log("YMesh::set_stream_source() called\n");
	clear_stream_source();

	for(int i=0; i<Source_Count; ++i) {
		if(sources[i] == NULL) continue;

		vbs_[i] = sources[i];
	}
}

void YMesh::set_declaration(WrapperDirect3DVertexDeclaration9* decl) {
	Log::log("YMesh::set_declaration() called\n");
	decl_ = decl;
	
	assert(decl);

	for(int i=0; i<decl_->numElements; ++i) {
		WORD stream = decl_->pDecl[i].Stream;
		WORD offest = decl_->pDecl[i].Offset;
		BYTE type = decl_->pDecl[i].Type;
		BYTE us_index = decl_->pDecl[i].UsageIndex;

		int field_size = get_field_size(type);
		format_sum_[stream] += field_size;

		switch(decl_->pDecl[i].Usage) {
		case D3DDECLUSAGE_POSITION:
			source_format_[stream].push_back(parse_node(1, offest, field_size, encode_position));
			break;
		case D3DDECLUSAGE_NORMAL:
			source_format_[stream].push_back(parse_node(2, offest, field_size, encode_normal));
			break;
		case D3DDECLUSAGE_TEXCOORD:
			source_format_[stream].push_back(parse_node(4, offest, field_size, encode_tex));
			break;
		case D3DDECLUSAGE_TANGENT:
			source_format_[stream].push_back(parse_node(8, offest, field_size, encode_tangent));
			break;
		case D3DDECLUSAGE_COLOR:
			source_format_[stream].push_back(parse_node(16, offest, field_size, encode_color));
			break;
		case D3DDECLUSAGE_BLENDWEIGHT:
			source_format_[stream].push_back(parse_node(32, offest, field_size, encode_weight));
			break;
		case D3DDECLUSAGE_BLENDINDICES:
			source_format_[stream].push_back(parse_node(64, offest, field_size, encode_indice));
			break;
		default:
			break;
		}
	}

}

void YMesh::send_format(WrapperDirect3DVertexBuffer9* vb, int sn) {
	Log::log("YMesh::send_format(), id=%d, draw_type=%d, ib=%d, leader_vb_id=%d, sn=%d, format size=%d\n", vb->GetId(), draw_method_, ib_, leader_vb_id, sn, source_format_[sn].size());
	cs.begin_command(SetVertexBufferFormat_Opcode, vb->GetId());
	cs.write_uint(source_format_[sn].size());
	
	int sz = source_format_[sn].size();
	for(int i=0; i<sz; ++i) {
		parse_node& nd = source_format_[sn][i];
		cs.write_char(nd.flag_);
		cs.write_uchar(nd.offest_);
		cs.write_uchar(nd.size_);

		Log::log("flag=%d, offest=%d, size=%d\n", nd.flag_, nd.offest_, nd.size_);
	}

	cs.end_command();
}

bool YMesh::need_decimate() {
	Log::log("YMesh::need_decimate() called\n");
	if(decision_ != DecimateDecision::INIT) {
		return (decision_ == DecimateDecision::NEED);
	}
	else {

		
		if(config_low_bound_ > config_up_bound_) {
			decision_ = DecimateDecision::NO_NEED;
			return false;
		}
		
		
		//if(true) {
		if(draw_method_ == DrawMethod::DrawPrimitive) {
			decision_ = DecimateDecision::NO_NEED;
			return false;
		}

		/*
		if( !(base_vertex_index_ == 0 && min_vertex_index_ == 0 && prim_count_ > 200) ) {
			decision_ = DecimateDecision::NO_NEED;
			return false;
		}
		*/

		if(init_vertex_count_ >= config_low_bound_ && init_vertex_count_ <= config_up_bound_ && primitive_type_ == D3DPT_TRIANGLELIST) {
			decision_ = DecimateDecision::NEED;
			return true;
		}
		else {
			decision_ = DecimateDecision::NO_NEED;
			return false;
		}
	}
}

void YMesh::dump_pos() {
	Log::log("YMesh::dump_pos() called\n");
	//if(pos_dumped_) return;
	//pos_dumped_ = true;
	

	pos_stride = -1, pos_length = -1, pos_offest = -1;
	char* vb_data = NULL;
	for(int i=0; i<source_format_->size() && !vb_data; ++i) {
		for(int j=0; j<source_format_[i].size() && !vb_data; ++j) {
			if(source_format_[i][j].flag_ == 1) { //it is the position flag
				parse_node& nd = source_format_[i][j];
				
				pos_stride = vbs_[i]->stride;
				pos_length = vbs_[i]->length;
				pos_offest = nd.offest_;
				vbs_[i]->read_data_from_buffer(&vb_data, 0, 0);

				pos_vb_ = vbs_[i];
			}
		}
	}

	if(slim_ == NULL) slim_ = new YSlim();
	slim_->vertices.clear();

	assert(vb_data);
	assert(slim_);
	assert(slim_->vertices.size() == 0);
	
	//如果要decimate时，这个记得去掉
	//if(!need_decimate()) return;

	Vertex v;
	int v_cnt = 0;

	//vertex of the mesh start from here
	vb_data += (base_vertex_index_) * pos_stride;

	if(base_vertex_index_ + init_vertex_count_ > pos_vb_->length / pos_vb_->stride) {
		Log::slog("base_ver %d, min_ver %d, init_vet %d, count %d\n", base_vertex_index_, min_vertex_index_, init_vertex_count_, pos_vb_->length / pos_vb_->stride);
		//assert(false);
	}

	int ed = min(init_vertex_count_, pos_vb_->length / pos_vb_->stride - base_vertex_index_);
	init_vertex_count_ = ed;

	for(int i=0; i<ed; vb_data+=pos_stride, ++i) {


		v.x = *( (float*)(vb_data + pos_offest) );
		v.y = *( (float*)(vb_data + pos_offest + 4) );
		v.z = *( (float*)(vb_data + pos_offest + 8) );
		v.id = v_cnt++;
		
		slim_->vertices.push_back(v);
	}
}

int YMesh::get_index_data(char*& ib_data, int& i, D3DFORMAT format) {
	int ret;
	if(format == D3DFMT_INDEX32) {
		ret = *( (unsigned int*)(ib_data) );
		i += 4;
		ib_data += 4;
	}
	else {
		ret = *( (unsigned short*)(ib_data) );
		i += 2;
		ib_data += 2;
	}

	return ret;
}

void YMesh::add_face(int aa, int bb, int cc) {
	Face f;
	double x0, y0, z0;		//ax + by + cz = 0
	double x1, y1, z1;
	double x2, y2, z2;
	double a, b, c, M;

	f.id_vertex[0] = aa;
	f.id_vertex[1] = bb;
	f.id_vertex[2] = cc;

	//Log::slog("YMesh::add_face(), vsz=%d, a=%d, b=%d, c=%d\n", slim_->vertices.size(), aa, bb, cc);

	if(aa >= slim_->get_vert_num() || bb >= slim_->get_vert_num() || cc >= slim_->get_vert_num()) {
		Log::log("fuck: %d %d %d %d\n", slim_->get_vert_num(), aa, bb, cc);
		Log::log("base %d, min_ver %d, ver_cnt %d\n", base_vertex_index_, min_vertex_index_, num_vertices_);
		//assert(false);
		return;
	}

	x0 = slim_->vertices[f.id_vertex[0]].x;
	y0 = slim_->vertices[f.id_vertex[0]].y;
	z0 = slim_->vertices[f.id_vertex[0]].z;
	x1 = slim_->vertices[f.id_vertex[1]].x;
	y1 = slim_->vertices[f.id_vertex[1]].y;
	z1 = slim_->vertices[f.id_vertex[1]].z;
	x2 = slim_->vertices[f.id_vertex[2]].x;
	y2 = slim_->vertices[f.id_vertex[2]].y;
	z2 = slim_->vertices[f.id_vertex[2]].z;
	a = (y1-y0)*(z2-z0) - (z1-z0)*(y2-y0);   /* a1*b2 - a2*b1;        */
	b = (z1-z0)*(x2-x0) - (x1-x0)*(z2-z0);   /* a2*b0 - a0*b2;        */
	c = (x1-x0)*(y2-y0) - (y1-y0)*(x2-x0);   /* a0*b1 - a1*b0;        */
	M = sqrt(a*a + b*b + c*c + 0.1);
	a = a/M;
	b = b/M;
	c = c/M;
	f.plane[0] = a;
	f.plane[1] = b;
	f.plane[2] = c;
	f.plane[3] = -1*(a*x0 + b*y0 + c*z0);				/* -1*(a*x + b*y + c*z); */
	slim_->faces.push_back(f);
}

int YMesh::get_index_list() {
	if(ib_ == NULL) {
		Log::log("YMesh::dump_index(), create a new index\n");
		init_vertex_count_ = num_vertices_;
		create_index_buffer(init_vertex_count_, NULL, &ib_);
	}

	char* ib_data = NULL;
	ib_->read_data_from_buffer(&ib_data, 0, 0);
	

	int sz = (ib_->Format == D3DFMT_INDEX32) ? 4 : 2;
	int st = start_index_ * sz;

	int ed = min(ib_->length, st + 3 * prim_count_ * sz);

	ib_data += st;

	indices_.clear();
	int mx = 0;
	for(int i=st; i<ed; ) {
		int x = get_index_data(ib_data, i, ib_->Format);
		indices_.push_back(x);

		mx = max(mx, x);
	}

	return mx;
}

void YMesh::dump_index() {
	Log::log("YMesh::dump_index() called\n");
	//if(face_dumped_) return;
	//face_dumped_ = true;
	
	//如果要decimate时，这个记得去掉
	//if(!need_decimate()) return;

	if(slim_ == NULL) slim_ = new YSlim();

	slim_->faces.clear();

	assert(slim_->faces.size() == 0);
	 
	if(primitive_type_ == D3DPT_TRIANGLELIST) {

		
		int ed = min(indices_.size(), prim_count_ * 3);


		for(int i=0; i+2<ed; i+=3) {
			int a = indices_[i];
			int b = indices_[i+1];
			int c = indices_[i+2];

			add_face(a, b, c);
		}
	}

	init_face_count_ = slim_->faces.size();
}

void YMesh::dump_mesh(bool write_to_file) {
	Log::log("YMesh::dump_mesh() called\n");
	
	/*
	if(mesh_dumped_) {
		Log::log("YMesh::dump_mesh(), already dump\n");
		return;
	}
	mesh_dumped_ = true;
	*/
	
	init_vertex_count_ = get_index_list() + 1;
	//好烦啊，难道init_vertex_count太大了？
	//init_vertex_count_ = num_vertices_;
	dump_pos();
	dump_index();

	static bool dump_ = false;

	if(write_to_file) {
	//if(base_vertex_index_ == 0 && min_vertex_index_ == 0 && num_vertices_ == 1082 && start_index_ == 1680 && prim_count_ == 792 && !dump_) {
		dump_ = true;
		char fname[100];
		sprintf(fname, "mesh_%c%d.smf", (ib_ ? 'i' : 'v'), (ib_ ? ib_->GetID() : vbs_[0]->GetId()) );
		slim_->write_smf(fname);
	}
}

void YMesh::select_vertex() {

	remain_vertices_.clear();

	if(!need_decimate()) {
		
		//TODO 这里有问题 每个sn会重复push back这个vector
		for(int i=0; i<init_vertex_count_; ++i) remain_vertices_.push_back(i + base_vertex_index_);
	}
	else {
		Log::log("YMesh::select_vertex(), need_decimate, face size=%d\n", slim_->faces.size());
		for(int i=0; i<slim_->faces.size(); ++i) {

			if(slim_->get_f_cnt(i) - slim_->get_f_cnt(i - 1) <= 0) continue;

			remain_vertices_.push_back(slim_->faces[i].id_vertex[0] + base_vertex_index_);
			remain_vertices_.push_back(slim_->faces[i].id_vertex[1] + base_vertex_index_);
			remain_vertices_.push_back(slim_->faces[i].id_vertex[2] + base_vertex_index_);
		}

		sort(remain_vertices_.begin(), remain_vertices_.end());
		int sz = unique(remain_vertices_.begin(), remain_vertices_.end()) - remain_vertices_.begin();
		remain_vertices_.resize(sz);
	}
}

void YMesh::send_vertex_select(WrapperDirect3DVertexBuffer9* ib, int sn) {
	//has sent before

	int sz = remain_vertices_.size();
	cs.begin_command(SetDecimateResult_Opcode, ib->GetId());
	cs.write_int(sz);
	Log::log("result size: %d\n", sz);
	
	for(int i=0; i<sz; ++i) {
		cs.write_int(remain_vertices_[i]);
		//Log::log("vertex %d\n", remain_vertices_[i]);
	}
	
	cs.end_command();
}

void YMesh::update_index_buffer(WrapperDirect3DIndexBuffer9* ib) {
	Log::log("YMesh::update_index_buffer() called\n");

	assert(ib);

	ib->PrepareIndexBuffer();

	//set indices
	cs.begin_command(SetIndices_Opcode, device_->GetID());
	cs.write_short(ib->GetID());
	cs.end_command();
}

void YMesh::update_vertex_buffer(int sn) {
	Log::log("YMesh::update_vertex_buffer() called\n");

	WrapperDirect3DVertexBuffer9* vb_ = vbs_[sn];
	if(vb_->isFirst) {
		memset(vb_->cache_buffer, 0, vb_->length);

		vb_->isFirst = false;
	}

	
	if(vb_->owner_mesh_ != this || !select_result_sent_[sn]) {
		if(this != vb_->owner_mesh_) {
			Log::log("switching owner mesh, vb id=%d\n", vb_->GetId());
		}

		vb_->owner_mesh_ = this;
		select_result_sent_[sn] = true;

		send_format(vb_, sn);
		//send_vertex_select(vb_, sn);
	}

	if(!vb_->changed) {
		Log::log("YMesh::update_vertex_buffer(), vb %d doesn't change\n", vb_->GetId());
		goto RET;
	}
	
	cs.begin_command(VertexBufferUnlock_Opcode, vb_->GetId());
	/*
	cs.write_uint(vb_->m_LockData.OffsetToLock);
	cs.write_uint(vb_->m_LockData.SizeToLock);
	cs.write_uint(vb_->m_LockData.Flags);
	*/

	//cs.write_int(CACHE_MODE_COPY);
	cs.write_int(vb_->stride);

	char* vt = NULL;
	vb_->read_data_from_buffer(&vt, 0, 0);
	char* sv_vt = vt;

	//assert(remain_vertices_.size() > 0);
	//assert(source_format_[sn].size() > 0);

	int change_cnt = 0;
	int cnt = 0;
	cs.write_int(bs_.count_);

#ifdef USE_FLOAT_COMPRESS

	cs.write_float(pos_mx_);
	cs.write_float(pos_my_);
	cs.write_float(pos_mz_);

#endif

	for(int i=0; i<remain_vertices_.size(); ++i) {

		if(bs_.is_set(i)) {

			vt = sv_vt + remain_vertices_[i] * vb_->stride;
			
			cs.write_ushort(remain_vertices_[i]);

			//cs.write_byte_arr(vt, vb_->stride);
			
			
			for(int j=0; j<source_format_[sn].size(); ++j) {
				parse_node& nd = source_format_[sn][j];
				int ret = nd.func_(this, vb_, vt, nd.offest_, nd.size_);

			}
			

			cnt++;
		}
	}

	Log::log("remain size=%d, bs count=%d, cnt=%d\n", remain_vertices_.size(), bs_.count_, cnt);
	assert(cnt == bs_.count_);

	int cm_len = cs.get_command_length();
	Log::log("YMesh::update_vertex_buffer(), uncompress length=%d, compress length=%d\n", remain_vertices_.size() * format_sum_[sn], cm_len);

	data_bytes_ += cs.get_command_length();
	update_count_++;

	cs.end_command();

RET:
	//send setstreamsource command
	cs.begin_command(SetStreamSource_Opcode, device_->GetID());
	cs.write_uint(vb_->streamNumber);
	cs.write_int(vb_->GetId());
	cs.write_uint(vb_->offsetInBytes);
	cs.write_uint(vb_->stride);
	cs.end_command();
}

void YMesh::decimate() {
	Log::log("YMesh::decimate() called\n");

	if(prim_count_ > cur_prim_count_) {
		cur_prim_count_ = prim_count_;
		decimate_count_++;
		decimated_ = 0;
	}

	if(decimated_) return;
	decimated_ = true;

	dump_mesh();

	if(need_decimate()) {

		Log::log("YMesh::decimate(), before decimate vertex count=%d, face count=%d\n", slim_->get_vert_num(), slim_->faces.size());
		slim_->decimate(slim_->get_vert_num() * config_ratio_);

		Log::log("YMesh::decimate(), after decimate vertex count=%d, face count=%d\n", slim_->get_vert_num(), slim_->faces.size());

		if(decimate_ib_) {
			decimate_ib_->Release();
			decimate_ib_ = NULL;
		}

		create_index_buffer(slim_->faces.size() * 3, slim_, &decimate_ib_);
	}

	select_vertex();
}

void YMesh::select_pos_change() {
	bs_.reset(max(1, remain_vertices_.size()));

	if(remain_vertices_.size() == 0) return;

	int offest = -1;
	char* pos = NULL;
	pos_vb_->read_data_from_buffer(&pos, 0, 0);

	pos_mx_ = pos_my_ = pos_mz_ = 0.0f;

	pos += remain_vertices_[0] * pos_vb_->stride;

	for(int i=0; i<remain_vertices_.size(); ++i) {

		if(i > 0) {
			if(remain_vertices_[i] <= remain_vertices_[i-1]) {
				Log::log("select_pos_change(), remain arr not sort\n");
			}
			pos += (remain_vertices_[i] - remain_vertices_[i-1]) * pos_vb_->stride;
		}

		float x = *( (float*)(pos + pos_offest) );
		float y = *( (float*)(pos + pos_offest + 4) );
		float z = *( (float*)(pos + pos_offest + 8) );

		char* ptr = pos_vb_->cache_buffer + (pos - pos_vb_->m_LockData.pRAMBuffer);
		float ox = *( (float*)(ptr + pos_offest) );
		float oy = *( (float*)(ptr + pos_offest + 4) );
		float oz = *( (float*)(ptr + pos_offest + 8) );

		if(fabs(x - ox) > eps || fabs(y - oy) > eps && fabs(z - oz) > eps) {
			bs_.set(i);

			memcpy(ptr, pos, 12);
		}

		pos_mx_ = max(pos_mx_, fabs(x));
		pos_my_ = max(pos_my_, fabs(y));
		pos_mz_ = max(pos_mz_, fabs(z));

	}

	pos_mx_ += 1.0f;
	pos_my_ += 1.0f;
	pos_mz_ += 1.0f;
}

void YMesh::render(INT BaseVertexIndex,UINT MinVertexIndex, int offest, int prim_count) {
	Log::log("YMesh::render() called\n");

	
	select_pos_change();

	for(int i=0; i<Source_Count; ++i) {
		if(vbs_[i] == NULL) {
			continue;
		}

		update_vertex_buffer(i);
	}

	if(need_decimate()) {

		Log::log("YMesh::render(), vertex count=%d, face count=%d\n", slim_->get_vert_num(), slim_->faces.size());

		update_index_buffer(decimate_ib_);

		//drawindexprimitive
		cs.begin_command(DrawIndexedPrimitive_Opcode, device_->GetID());
		/*
		cs.write_char(D3DPT_TRIANGLELIST);
		cs.write_int(BaseVertexIndex);
		cs.write_int(MinVertexIndex);
		cs.write_uint(init_vertex_count_);
		cs.write_uint(offest);
		cs.write_uint(prim_count);
		*/

		cs.write_char(D3DPT_TRIANGLELIST);
		cs.write_int(base_vertex_index_);
		cs.write_int(min_vertex_index_);
		cs.write_uint(num_vertices_);
		cs.write_uint(0);
		
		cs.write_uint(slim_->get_f_cnt(prim_count - 1));
		//cs.write_int(prim_count_);

		cs.end_command();
	}
	else {
		if(this->draw_method_ == DrawMethod::DrawIndexedPrimitive)
			update_index_buffer(ib_);
	}
}

void YMesh::create_index_buffer(int idx_count, YSlim* slim, WrapperDirect3DIndexBuffer9** ib) {

	int idx_size = 4;
	WrapperDirect3DIndexBuffer9* tmp_ib = NULL;

	DWORD usage = (ib_) ? ib_->Usage : D3DUSAGE_WRITEONLY;
	D3DPOOL pool = (ib_) ? ib_->Pool : D3DPOOL_DEFAULT;

	
	device_->CreateIndexBuffer(idx_size * prim_count_ * 3 + 8, usage, D3DFMT_INDEX32, pool, (IDirect3DIndexBuffer9**)&tmp_ib, NULL);
	
	char* ptr = NULL;

	assert(tmp_ib);

	tmp_ib->Lock(0, 0, (void**)&ptr, D3DLOCK_NOSYSLOCK);

	if(slim != NULL) {
		for(int i=0; i<slim_->faces.size(); ++i) {
			if(slim_->get_f_cnt(i) - slim_->get_f_cnt(i - 1) <= 0) {
				continue;
			}
			
			*( (unsigned int*)ptr ) = slim_->faces[i].id_vertex[0];
			ptr += sizeof(unsigned int);

			*( (unsigned int*)ptr ) = slim_->faces[i].id_vertex[1];
			ptr += sizeof(unsigned int);

			*( (unsigned int*)ptr ) = slim_->faces[i].id_vertex[2];
			ptr += sizeof(unsigned int);
		}
	}
	else {
		for(int i=0; i<idx_count; ++i) {
			*( (unsigned int*)ptr ) = i;
		}
	}

	tmp_ib->Unlock();

	*ib = tmp_ib;
}

int YMesh::get_field_size(int type) {
	Log::log("YMesh::get_field_size() called\n");
	switch(type){
	case D3DDECLTYPE_FLOAT1:		//0
		return 4;
	case D3DDECLTYPE_FLOAT2:		//1
		return 8;
	case D3DDECLTYPE_FLOAT3:		//2
		return 12;
	case D3DDECLTYPE_FLOAT4:		//3
		return 16;
	case D3DDECLTYPE_D3DCOLOR:		//4
		return 4;
	case D3DDECLTYPE_UBYTE4:		//5
		return 4;
	case D3DDECLTYPE_SHORT2:		//6
		return 4;
	case D3DDECLTYPE_SHORT4:		//7
		return 8;
	case D3DDECLTYPE_UBYTE4N:		//8
		return 4;
	case D3DDECLTYPE_SHORT2N:		//9
		return 4;
	case D3DDECLTYPE_SHORT4N:		//10
		return 8;
	case D3DDECLTYPE_USHORT2N:		//11
		return 4;
	case D3DDECLTYPE_USHORT4N:		//12
		return 8;
	case D3DDECLTYPE_FLOAT16_2:		//15
		return 4;
	case D3DDECLTYPE_FLOAT16_4:		//16
		return 8;
	default:
		return 0;
	}
}

bool encode_position( YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size )
{
	float x = *( (float*)(pos + offest) );
	float y = *( (float*)(pos + offest + 4) );
	float z = *( (float*)(pos + offest + 8) );

#ifndef USE_FLOAT_COMPRESS
	
	cs.write_float(x);
	cs.write_float(y);
	cs.write_float(z);
	

#else

	cs.write_uint(compress_pos_to_dword(x, y, z, mesh->pos_mx_, mesh->pos_my_, mesh->pos_mz_));
#endif

	return true;
}

bool encode_normal( YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size )
{
	float nx = *( (float*)(pos + offest) );
	float ny = *( (float*)(pos + offest + 4) );
	float nz = *( (float*)(pos + offest + 8) );

#ifndef USE_FLOAT_COMPRESS
	cs.write_float(nx);
	cs.write_float(ny);
	cs.write_float(nz);
#else

	cs.write_uint(compress_normal_to_dword(nx, ny, nz));
#endif

	return true;
}

bool encode_tex( YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size )
{
	cs.write_vec(VertexBufferUnlock_Opcode, (float*)(pos + offest), size);

	return true;
}

bool encode_tangent( YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size )
{
	float tx = *( (float*)(pos + offest) );
	float ty = *( (float*)(pos + offest + 4) );
	float tz = *( (float*)(pos + offest + 8) );
	float ta = *( (float*)(pos + offest + 12) );

#ifndef USE_FLOAT_COMPRESS
	cs.write_float(tx);
	cs.write_float(ty);
	cs.write_float(tz);
	cs.write_float(ta);
#else

	cs.write_uint(compress_tangent_to_dword(tx, ty, tz, ta));
#endif

	return true;
}

bool encode_color( YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size )
{
	cs.write_byte_arr(pos + offest, size);

	return true;
}

bool encode_weight(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size) {

	float tx = *( (float*)(pos + offest) );
	float ty = *( (float*)(pos + offest + 4) );
	float tz = *( (float*)(pos + offest + 8) );
	float ta = *( (float*)(pos + offest + 12) );


	cs.write_float(tx);
	cs.write_float(ty);
	cs.write_float(tz);
	cs.write_float(ta);

	return true;
}

bool encode_indice(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size) {
	unsigned char tx = *( (unsigned char*)(pos + offest) );
	unsigned char ty = *( (unsigned char*)(pos + offest + 1) );
	unsigned char tz = *( (unsigned char*)(pos + offest + 2) );
	unsigned char ta = *( (unsigned char*)(pos + offest + 3) );

	cs.write_uchar(tx);
	cs.write_uchar(ty);
	cs.write_uchar(tz);
	cs.write_uchar(ta);

	return true;
}

void YMesh::stat(FILE* f) {
	fprintf(f, "mesh:\t0x%x\n", this);
	
	fprintf(f, "\tvb:\t");
	for(int i=0; i<Source_Count; ++i) {
		if(vbs_[i] == NULL) continue;
		fprintf(f, "%d ", vbs_[i]->GetId());
	}
	fprintf(f, "\n");

	fprintf(f, "\tib:\t%d\n", ib_ ? ib_->GetID() : -1);

	fprintf(f, "\tvc:\t%d -> %d\n", init_vertex_count_, remain_vertices_.size());
	fprintf(f, "\tfc:\t%d -> %d\n", init_face_count_, slim_->faces.size());
	fprintf(f, "\tdata_bytes: %I64d\n", data_bytes_);
	fprintf(f, "\tupdate_count: %d\n", update_count_);
	fprintf(f, "\tdecimate_count: %d\n", decimate_count_);
	fprintf(f, "\tpack: %d ms\n", slim_->pack_vertex_time_);
	fprintf(f, "\tdecimate: %d ms\n", slim_->decimate_time_);

	fprintf(f, "\ttype: %d\n", type_);
	fprintf(f, "\tbase_vertex_index: %d\n", base_vertex_index_);
	fprintf(f, "\tmin_vertex_index: %d\n", min_vertex_index_);
	fprintf(f, "\tnum_vertices: %d\n", num_vertices_);
	fprintf(f, "\tstart_index: %d\n", start_index_);
	fprintf(f, "\tprim_count: %d\n", prim_count_);

	for(int i=0; i<decl_->numElements; ++i) {
		WORD stream = decl_->pDecl[i].Stream;
		WORD offest = decl_->pDecl[i].Offset;
		BYTE type = decl_->pDecl[i].Type;
		BYTE us_index = decl_->pDecl[i].UsageIndex;

		int field_size = get_field_size(type);

		switch(decl_->pDecl[i].Usage) {
		case D3DDECLUSAGE_POSITION:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "POSITION", type, us_index);
			break;
		case D3DDECLUSAGE_BLENDWEIGHT:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "BLENDWEIGHT", type, us_index);
			break;
		case D3DDECLUSAGE_BLENDINDICES:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "BLENDINDICES", type, us_index);
			break;
		case D3DDECLUSAGE_NORMAL:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "NORMAL", type, us_index);
			break;
		case D3DDECLUSAGE_PSIZE:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "PSIZE", type, us_index);
			break;
		case D3DDECLUSAGE_TEXCOORD:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "TEXCOORD", type, us_index);
			break;
		case D3DDECLUSAGE_TANGENT:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "TANGENT", type, us_index);
			break;
		case D3DDECLUSAGE_BINORMAL:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "BINORMAL", type, us_index);
			break;
		case D3DDECLUSAGE_TESSFACTOR:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "TESSFACTOR", type, us_index);
			break;
		case D3DDECLUSAGE_POSITIONT:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "POSITIONT", type, us_index);
			break;
		case D3DDECLUSAGE_COLOR:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "COLOR", type, us_index);
			break;
		case D3DDECLUSAGE_FOG:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "FOG", type, us_index);
			break;
		case D3DDECLUSAGE_DEPTH:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "DEPTH", type, us_index);
			break;
		case D3DDECLUSAGE_SAMPLE:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "SAMPLE", type, us_index);
			break;
		default:
			fprintf(f, "\tstream %d, offest %d, usage %s, type %d, index %d\n", stream, offest, "UNKNOWN", type, us_index);
			break;
		}
	}
}

bool mesh_cmp(YMesh* a, YMesh* b) {
	//return a->data_bytes_ > b->data_bytes_;
	return a->need_decimate() > b->need_decimate() || (a->need_decimate() == b->need_decimate() && a->init_vertex_count_ > b->init_vertex_count_);
}

void YMesh::log_meshes(char* fname) {
	
	FILE* f = fopen(fname, "w");

	fprintf(f, "totaly %d meshes\n", mesh_list.size());

	sort(mesh_list.begin(), mesh_list.end(), mesh_cmp);

	for(int i=0; i<mesh_list.size(); ++i) {
		mesh_list[i]->stat(f);
	}

	fclose(f);
	
}
