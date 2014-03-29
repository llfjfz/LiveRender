#ifndef __YMESH__
#define __YMESH__

#include "utility.h"
#include "wrap_direct3dindexbuffer9.h"
#include "wrap_direct3dvertexbuffer9.h"
#include "wrap_direct3dvertexdeclaration9.h"
#include "yslim.h"
#include "bit_set.h"

class YMesh;

typedef bool (*parse_decl)(YMesh* mesh_, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size);

struct parse_node {
	int flag_;
	int offest_;
	int size_;
	parse_decl func_;

	parse_node(int flag, int offest, int size, parse_decl func): flag_(flag), offest_(offest), size_(size), func_(func) {

	}
};

bool encode_position(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size);
bool encode_normal(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size);
bool encode_tex(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size);
bool encode_tangent(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size);
bool encode_color(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size);
bool encode_weight(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size);
bool encode_indice(YMesh* mesh, WrapperDirect3DVertexBuffer9* vb, char* pos, int offest, int size);

enum DrawMethod{
	DrawPrimitive = 0,
	DrawIndexedPrimitive,
	NONE
};

enum DecimateDecision {
	INIT = 0,
	NEED,
	NO_NEED
};

class WrapperDirect3DDevice9;

class YMesh {
public:

	YMesh(WrapperDirect3DDevice9* device);

	int get_field_size(int type);
	void clear_stream_source();
	void set_stream_source(WrapperDirect3DVertexBuffer9* sources[]);
	void set_declaration(WrapperDirect3DVertexDeclaration9* decl);
	void set_indices(WrapperDirect3DIndexBuffer9* ib);
	void set_draw_type(DrawMethod method, D3DPRIMITIVETYPE type);

	void set_index_params(D3DPRIMITIVETYPE Type, INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);


	bool need_decimate();
	void decimate();

	bool vb_changed(WrapperDirect3DVertexBuffer9* cur_vbs[]);

	void create_index_buffer(int index_count, YSlim* slim, WrapperDirect3DIndexBuffer9** ib);

	void send_format(WrapperDirect3DVertexBuffer9* vb, int sn);
	void send_vertex_select(WrapperDirect3DVertexBuffer9* vb, int sn);
	void update_vertex_buffer(int sn);
	void update_index_buffer(WrapperDirect3DIndexBuffer9* ib);
	void render(INT BaseVertexIndex,UINT MinVertexIndex, int offest, int prim_count);

	void select_vertex();
	void select_pos_change();

	void add_face(int a, int b, int c);
	void dump_mesh(bool write_to_file=0);
	void dump_pos();
	void dump_index();
	int get_index_data(char*& ib_data, int& i, D3DFORMAT format);

	int get_vertex_count();
	int get_face_count();

	DecimateDecision decision_;

	void set_leader_vb(int id) {
		leader_vb_id = id;
	}

	void stat(FILE* f);
	static void log_meshes(char* fname);

	int get_index_list();

	float pos_mx_;
	float pos_my_;
	float pos_mz_;

	int init_vertex_count_;
	int init_face_count_;

	long long data_bytes_;
	int update_count_;

private:
	WrapperDirect3DDevice9* device_;
	WrapperDirect3DVertexDeclaration9* decl_;
	WrapperDirect3DIndexBuffer9* ib_;
	WrapperDirect3DVertexBuffer9* vbs_[Source_Count];

	WrapperDirect3DVertexBuffer9* pos_vb_;
	int pos_stride;
	int pos_length;
	int pos_offest;

	BitSet bs_;

	WrapperDirect3DIndexBuffer9* decimate_ib_;

	vector<parse_node> source_format_[Source_Count];
	int format_sum_[Source_Count];

	int mesh_dumped_;
	int pos_dumped_;
	int face_dumped_;

	int decimated_;

	D3DPRIMITIVETYPE primitive_type_;
	DrawMethod draw_method_;

	YSlim* slim_;

	int config_low_bound_;
	int config_up_bound_;
	float config_ratio_;

	
	vector<int> remain_vertices_;
	vector<int> indices_;

	int select_result_sent_[Source_Count];

	int leader_vb_id;

	static vector<YMesh*> mesh_list;

	//DrawIndexPrimitive params
	D3DPRIMITIVETYPE type_;
	INT base_vertex_index_;
	UINT min_vertex_index_;
	UINT num_vertices_;
	UINT start_index_;
	UINT prim_count_;

	int cur_prim_count_;

	static int decimate_count_;
};

#endif

