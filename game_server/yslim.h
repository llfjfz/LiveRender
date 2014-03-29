#ifndef __YSLIM__
#define __YSLIM__

#include <vector>
#include <iostream>
#include "matrix.h"
#include <math.h>
#include <map>
#include <queue>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
using namespace std;


struct Vertex {
	double x;
	double y;
	double z;
	int id;
};

struct Face {
	unsigned int id_vertex[3];
	double plane[4];		// p = [a b c d]
};


struct Error {
	int u_, v_;
	double error_;

	Error(int u, int v, double error): u_(u), v_(v), error_(error) {

	}
};

struct cmp {
	bool operator()(Error& A, Error& B) {
		return A.error_ > B.error_;
	}
};

class YSlim {
public:
	YSlim();
	~YSlim();

	int get_face_num();
	int get_vert_num();

	void read_smf(char* filename);
	void decimate(int target_num_vert);
	void write_smf(char* filename);

	int get_f_cnt(int i) {
		if(i < 0) return 0;
		if(i >= f_cnt.size()) return f_cnt.back();
		return f_cnt[i];
	}

	vector<Vertex> vertices;
	vector<Face> faces;

	vector<int> f_cnt;

	int pack_vertex_time_;
	int decimate_time_;

private:
	int vert_cnt_;
	vector<int> p_;
	vector<Vertex> idx_;
	map<int, Matrix> quadrics;

	void add_edge_face(int v1, int v2, int f);
	void init_border();
	bool is_border(int v1, int v2);
	
	map<pair<int, int>, int> ed_faces_mp_;
	map<pair<int, int>, int> :: iterator it;

	vector<vector<int> > adj;

	priority_queue<Error, vector<Error>, cmp> errors;

	void parse(FILE* file);
	
	bool is_merged(int u, int v);
	void merge(int u, int v);
	int find(int x);
	void pack_vertex();
	void update_faces();

	void add_error(int v1, int v2);
	void init_quadrics();
	void select_pair();
	double calculate_error(int id_v1, int id_v2, double* vx=0, double* vy=0, double* vz=0);

	double distance(Vertex v1, Vertex v2);
	double vertex_error(Matrix q, double vx, double vy, double vz);
};

#endif