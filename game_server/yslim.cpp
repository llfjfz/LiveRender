#include "yslim.h"
#include <assert.h>
#include <stdlib.h>
#define eps (1e-3)

#define Bound_Constraint 1e14


#include <time.h>

YSlim::YSlim() {
	vertices.clear();
	faces.clear();
	quadrics.clear();

	pack_vertex_time_ = 0;
	decimate_time_ = 0;

	vert_cnt_ = -1;

	while(!errors.empty()) errors.pop();
}

YSlim::~YSlim() {

}

int YSlim::get_face_num() {
	return faces.size();
}

int YSlim::get_vert_num() {
	if(vert_cnt_ == -1)
		return vertices.size();
	else
		return vert_cnt_;
}

inline double YSlim::distance(Vertex v1, Vertex v2) {
	return sqrt( pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2) );
}

inline double YSlim::vertex_error(Matrix q, double x, double y, double z) {
	return q[0]*x*x + 2*q[1]*x*y + 2*q[2]*x*z + 2*q[3]*x + q[5]*y*y
		+ 2*q[6]*y*z + 2*q[7]*y + q[10]*z*z + 2*q[11]*z + q[15];
}

void YSlim::init_quadrics() {
	quadrics.clear();
	for(int i=0; i<vertices.size(); ++i) {
		quadrics.insert( make_pair(i, Matrix(0.0)) );
	}

	for(it=ed_faces_mp_.begin(); it!=ed_faces_mp_.end(); ++it) {
		if(it->second == -1) continue;
		int v1 = it->first.first;
		int v2 = it->first.second;

		quadrics[v1] += Matrix(Bound_Constraint);
		quadrics[v2] += Matrix(Bound_Constraint);
	}

	for(int i=0; i<faces.size(); ++i) {
		for(int j=0; j<3; ++j) {
			if(faces[i].id_vertex[j] >= vertices.size()) continue;
			quadrics[ faces[i].id_vertex[j] ] += Matrix(faces[i].plane);
		}
	}
}

void YSlim::add_error(int v1, int v2) {
	if(v1 >= vertices.size() || v2 >= vertices.size()) return;

	if(is_border(v1, v2)) return;

	errors.push( Error(v1, v2, calculate_error(v1, v2)) );
	
	adj[v1].push_back(v2);
	adj[v2].push_back(v1);
}

//use for boundaries constrain
void YSlim::add_edge_face(int v1, int v2, int f) {
	if(v1 > v2) swap(v1, v2);
	it = ed_faces_mp_.find(make_pair(v1, v2));

	if(it == ed_faces_mp_.end()) {
		ed_faces_mp_.insert(make_pair(make_pair(v1, v2), f));
	}
	else {
		it->second = -1;
	}
}

void YSlim::init_border() {
	ed_faces_mp_.clear();

	for(int i=0; i<faces.size(); ++i) {
		add_edge_face(faces[i].id_vertex[0], faces[i].id_vertex[1], i);
		add_edge_face(faces[i].id_vertex[0], faces[i].id_vertex[2], i);
		add_edge_face(faces[i].id_vertex[1], faces[i].id_vertex[2], i);
	}
}

bool YSlim::is_border(int v1, int v2) {
	if(v1 > v2) swap(v1, v2);
	it = ed_faces_mp_.find(make_pair(v1, v2));

	if(it != ed_faces_mp_.end() && it->second != -1) {
		return true;
	}
	else
		return false;
}

void YSlim::select_pair() {
	
	for(int i=0; i<adj.size(); ++i) {
		adj[i].clear();
	}
	adj.clear();

	vector<int> line;
	for(int i=0; i<vertices.size(); ++i) {
		adj.push_back(line);
	}

	while(!errors.empty()) errors.pop();

	for(int i=0; i<faces.size(); ++i) {
		add_error(faces[i].id_vertex[0], faces[i].id_vertex[1]);
		add_error(faces[i].id_vertex[0], faces[i].id_vertex[2]);
		add_error(faces[i].id_vertex[1], faces[i].id_vertex[2]);
	}
}

double YSlim::calculate_error(int id_v1, int id_v2, double* vx/* =0 */, double* vy/* =0 */, double* vz/* =0 */) {
	double min_error;
	Matrix q_bar;
	Matrix q_delta;
	bool isReturnVertex = true;
	if (vx == NULL) { vx = new double; isReturnVertex = false; }
	if (vy == NULL) { vy = new double; }
	if (vz == NULL) { vz = new double; }

	/* computer quadric of virtual vertex vf */
	q_bar = quadrics[id_v1] + quadrics[id_v2];

	/* test if q_bar is symmetric */
	if (q_bar[1] != q_bar[4] || q_bar[2] != q_bar[8] || q_bar[6] != q_bar[9] || 
		q_bar[3] != q_bar[12] || q_bar[7] != q_bar[13] || q_bar[11] != q_bar[14])
	{
		fprintf(stderr, "ERROR: Matrix q_bar is not symmetric!\nid_v1 = %d, id_v2 = %d\n", id_v1, id_v2);
		//system("PAUSE");
		exit(-3);
	}

	double vx1 = vertices[id_v1].x;
	double vy1 = vertices[id_v1].y;
	double vz1 = vertices[id_v1].z;

	double vx2 = vertices[id_v2].x;
	double vy2 = vertices[id_v2].y;
	double vz2 = vertices[id_v2].z;

	double vx3 = double (vx1+vx2)/2;
	double vy3 = double (vy1+vy2)/2;
	double vz3 = double (vz1+vz2)/2;

	double error1 = vertex_error(q_bar, vx1, vy1, vz1);
	double error2 = vertex_error(q_bar, vx2, vy2, vz2);
	double error3 = vertex_error(q_bar, vx3, vy3, vz3);

	min_error = min(error1, error2);
	//min_error = std::min(error1, std::min(error2, error3));
	if (error1 == min_error) { *vx = vx1; *vy = vy1, *vz = vz1; }
	if (error2 == min_error) { *vx = vx2; *vy = vy2, *vz = vz2; }
	//if (error3 == min_error) { *vx = vx3; *vy = vy3, *vz = vz3; }

	min_error = vertex_error(q_bar, *vx, *vy, *vz);

	if (isReturnVertex == false) { delete vx; delete vy; delete vz; }
	return min_error;
}

/* 
 * pass: second pass at a Simple Mesh Format SMF file 
 * that gets all the data.
 *
 * file  - (fopen'd) file descriptor 
*/
void YSlim::parse(FILE* file) {
	char ch;
	char buf[1024];
	Vertex v;
	Face f;
	int local_num_vertices = 0;
	double x0, y0, z0;		//ax + by + cz = 0
	double x1, y1, z1;
	double x2, y2, z2;
	double a, b, c, M;

	while(fscanf(file, "%c", &ch) != EOF) {
		switch(ch) {
		case ' ' :         /* blanks */
		case '\t':
		case '\n':
			continue;
		case '#':
			fgets(buf, sizeof(buf), file);
			break;
		case 'v':
		case 'V':
			local_num_vertices++;		//vertex index start from 0
			fscanf(file, "%lf %lf %lf", &v.x, &v.y, &v.z);
			v.id = local_num_vertices-1;
			vertices.push_back(v);
			break;
		case 'f':
		case 'F':
			fscanf(file, "%d %d %d", &f.id_vertex[0], &f.id_vertex[1], &f.id_vertex[2]);

			f.id_vertex[0]--;
			f.id_vertex[1]--;
			f.id_vertex[2]--;

			x0 = vertices[f.id_vertex[0]].x;
			y0 = vertices[f.id_vertex[0]].y;
			z0 = vertices[f.id_vertex[0]].z;
			x1 = vertices[f.id_vertex[1]].x;
			y1 = vertices[f.id_vertex[1]].y;
			z1 = vertices[f.id_vertex[1]].z;
			x2 = vertices[f.id_vertex[2]].x;
			y2 = vertices[f.id_vertex[2]].y;
			z2 = vertices[f.id_vertex[2]].z;
			a = (y1-y0)*(z2-z0) - (z1-z0)*(y2-y0);   /* a1*b2 - a2*b1;        */
			b = (z1-z0)*(x2-x0) - (x1-x0)*(z2-z0);   /* a2*b0 - a0*b2;        */
			c = (x1-x0)*(y2-y0) - (y1-y0)*(x2-x0);   /* a0*b1 - a1*b0;        */
			M = sqrt(a*a + b*b + c*c + 0.01);
			a = a/M;
			b = b/M;
			c = c/M;
			f.plane[0] = a;
			f.plane[1] = b;
			f.plane[2] = c;
			f.plane[3] = -1*(a*x0 + b*y0 + c*z0);				/* -1*(a*x + b*y + c*z); */
			faces.push_back(f);
			break;
		default:
			fgets(buf, sizeof(buf), file);

			fprintf(stderr, "Parse() failed: invalid attributes: \"%c\".\n", ch);
			exit(-2);
		}
	}
}

void YSlim::read_smf(char* filename) {
	FILE* file;

	if( (file = fopen(filename, "r")) == NULL ) {
		fprintf(stderr, "read_smf() failed: can't open data file \"%s\".\n", filename);
		exit(-1);
	}

	parse(file);

	printf("vertex %d, face %d\n", vertices.size(), faces.size());

	fclose(file);
}

void YSlim::write_smf(char* filename) {
	FILE* file;

	if ((file = fopen(filename, "w")) == NULL)
	{
		fprintf(stderr, "write_pm() failed: can't write data file \"%s\".\n", filename);
		//system("PAUSE");
		exit(-1);
	}

	int vert_cnt = 0;
	for(int i=0; i<vertices.size(); ++i) {
		if(find(i) == i) vert_cnt++;
	}

	/* print header info */
	fprintf(file, "#$PM 0.1\n");
	fprintf(file, "#$vertices %d\n", vert_cnt);
	fprintf(file, "#$faces %d\n", f_cnt[faces.size()-1]);
	fprintf(file, "#\n");

	/* print vertices */
	int ii = 1;
	std::map<int, int> imap;

	for(int i=0; i<vertices.size(); ++i) {
		if(find(i) != i) continue;

		fprintf(file, "v %lf %lf %lf\n", vertices[i].x, vertices[i].y, vertices[i].z);
		imap.insert( std::map<int, int>::value_type(i, ii++) );
	}

	/* print faces */
	int v1, v2, v3;
	for (int i = 0; i < static_cast<int>(faces.size()); i++)
	{
		
		v1 = imap[ find(faces[i].id_vertex[0]) ];
		v2 = imap[ find(faces[i].id_vertex[1]) ];
		v3 = imap[ find(faces[i].id_vertex[2]) ];

		if(v1 == v2 || v1 == v3 || v2 == v3) continue;
		
		/*
		v1 = faces[i].id_vertex[0];
		v2 = faces[i].id_vertex[1];
		v3 = faces[i].id_vertex[2];
		*/
		fprintf(file, "f %d %d %d\n", v1, v2, v3);
	}
	fprintf(file, "\n");

	/* close the file */
	fclose(file);
}

int YSlim::find(int x) {
	if(x >= vertices.size()) return INT_MAX;
	if(p_[x] == x) return x;
	else return p_[x] = find(p_[x]);
}

bool YSlim::is_merged(int u, int v) {
	return find(u) == find(v);
}

bool cmp_idx(Vertex& A, Vertex& B) {
	return A.x < B.x || (A.x == B.x && A.y < B.y) || (A.x == B.x && A.y == B.y && A.z < B.z);
}

void YSlim::pack_vertex() {
	p_.clear();
	idx_.clear();
	
	int t1 = clock();
	
	for(int i=0; i<vertices.size(); ++i) {
		p_.push_back(i);
		idx_.push_back(vertices[i]);
	}

	sort(idx_.begin(), idx_.end(), cmp_idx);

	//printf("%d\n", vertices.size());

	
	
	
	
	for(int i=0; i<idx_.size(); ++i) {
		for(int j=i+1; j<idx_.size(); ++j) {

			if(distance(vertices[i], vertices[j]) < eps) {
				//printf("%d %d %.4lf\n", i, j, distance(vertices[i], vertices[j]));

				if(is_border(i, j)) continue;
				
				int u = find(idx_[i].id), v = find(idx_[j].id);
				p_[u] = v;
			}

			if(fabs(idx_[i].x - idx_[j].x) > eps || fabs(idx_[i].y - idx_[j].y) > eps || fabs(idx_[i].z - idx_[j].z) > eps) break;
		}
	}

	for(int i=0; i<faces.size(); ++i) {
		faces[i].id_vertex[0] = find(faces[i].id_vertex[0]);
		faces[i].id_vertex[1] = find(faces[i].id_vertex[1]);
		faces[i].id_vertex[2] = find(faces[i].id_vertex[2]);
	}
	
	
	int t2 = clock();

	pack_vertex_time_ = t2 - t1;

	//Log::log("vc: %d, fc: %d\n", get_vert_num(), get_face_num());
	//Log::log("pack vertex: %d ms\n", t2 - t1);
}

void YSlim::decimate(int target_num_vert) {

	init_border();

	pack_vertex();

	int t1 = clock();
	init_quadrics();
	select_pair();

	int id_v1, id_v2;
	double vx, vy, vz;

	vert_cnt_ = 0;
	for(int i=0; i<vertices.size(); ++i) {
		if(find(i) == i) vert_cnt_++;
	}
	
	while(vert_cnt_ > target_num_vert) {
		//assert(!errors.empty());

		//assert(false);

		if(errors.empty()) break;

		Error nd = errors.top();
		errors.pop();
		if(is_merged(nd.u_, nd.v_)) continue;

		if(nd.error_ > Bound_Constraint * 3) continue;

		id_v1 = find(nd.u_);
		id_v2 = find(nd.v_);

		if(id_v1 >= vertices.size() || id_v2 >= vertices.size())continue;
		if(id_v1 != nd.u_ || id_v2 != nd.v_) continue;

		//merge the smaller set to the bigger set
		//make id_v1 the bigger one 
		if(adj[id_v1].size() < adj[id_v2].size())
			swap(id_v1, id_v2);

		//update quadric of v1
		quadrics[id_v1] = quadrics[id_v1] + quadrics[id_v2];


		//merge the errors of id_v2 to id_v1
		while(adj[id_v2].size() > 0) {
			int x = adj[id_v2].back();
			
			if(find(x) == x && x != id_v2)
				add_error(x, id_v1);

			adj[id_v2].pop_back();
		}

		//printf("merging %d %d\n", id_v1, id_v2);

		//merge the point of id_v2 to id_v1
		p_[id_v2] = p_[id_v1];

		//decrease the vertex count
		vert_cnt_--;
	}

	update_faces();

	int t2 = clock();
	decimate_time_ = t2 - t1;
	//Log::log("decimate: %d ms\n", t2 - t1);
}

void YSlim::update_faces() {
	int v1, v2, v3;

	f_cnt.resize(faces.size());

	for (int i = 0; i < static_cast<int>(faces.size()); i++)
	{
		v1 = find(faces[i].id_vertex[0]);
		v2 = find(faces[i].id_vertex[1]);
		v3 = find(faces[i].id_vertex[2]);

		faces[i].id_vertex[0] = v1;
		faces[i].id_vertex[1] = v2;
		faces[i].id_vertex[2] = v3;

		if(v1 == v2 || v1 == v3 || v2 == v3) {
			//delete this face

			//assert(false);
			
			/*
			swap(faces[i], faces.back());
			faces.pop_back();
			i--;
			*/
			if(i == 0) f_cnt[i] = 0;
			else f_cnt[i] = f_cnt[i-1];
		}
		else {
			if(i == 0) f_cnt[i] = 1;
			else f_cnt[i] = f_cnt[i-1] + 1;
		}
	}
}


