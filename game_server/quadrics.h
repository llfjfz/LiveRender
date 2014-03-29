#ifndef __QUADRICS__
#define __QUADRICS__

#include <map>
#include <deque>
#include "matrix.h"

class YMesh;

struct Vertex {
	double x;
	double y;
	double z;
};

struct Face {
	unsigned int id_vertex[3];
	double plane[4];		// p = [a b c d]
};

typedef std::map<int, Vertex> Vertices;
typedef std::deque<Face> Faces;
typedef std::pair<int, int> Pair;
typedef std::map<Pair, double> Errors;
typedef std::map<int, Matrix> Matrices;

class Quadrics {
public:
	Quadrics();
	~Quadrics();

	int get_vertex_num();
	int get_face_num();
	void read_smf(char* filename);
	void set_data_source(YMesh* ymesh);
	void initial_quadrics();
	void decimate(int target_num_faces);
	void write_smf(char* filename);

	Vertices vertices;
	Faces faces;

private:
	
	 Matrices quadrics;
	 Errors errors;

	 void parse(FILE* file);
	 void select_pair();
	 double calculate_error(int id_v1, int id_v2, double* vx=0, double* vy=0, double* vz=0);
	 // inline methods
	 double distance(Vertex v1, Vertex v2);
	 double vertex_error(Matrix q, double vx, double vy, double vz);
};

#endif

