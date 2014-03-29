#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include "quadrics.h"

Quadrics::Quadrics() {
	vertices.clear();
	faces.clear();
}

Quadrics::~Quadrics() {

}

int Quadrics::get_vertex_num() {
	return vertices.size();
}

int Quadrics::get_face_num() {
	return faces.size();
}

/* 
 * pass: second pass at a Simple Mesh Format SMF file 
 * that gets all the data.
 *
 * file  - (fopen'd) file descriptor 
*/
void Quadrics::parse(FILE* file) {
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
			local_num_vertices++;		//vertex index start from 1
			fscanf(file, "%lf %lf %lf", &v.x, &v.y, &v.z);
			vertices.insert(Vertices::value_type(local_num_vertices, v));
			break;
		case 'f':
		case 'F':
			fscanf(file, "%d %d %d", &f.id_vertex[0], &f.id_vertex[1], &f.id_vertex[2]);
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
			M = sqrt(a*a + b*b + c*c + 0.1);
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

void Quadrics::read_smf(char* filename) {
	FILE* file;
	if( (file = fopen(filename, "r")) == NULL ) {
		fprintf(stderr, "read_smf() failed: can't open data file \"%s\".\n", filename);
		exit(-1);
	}

	parse(file);
	fclose(file);
}

inline double
	Quadrics::distance(Vertex v1, Vertex v2)
{
	return sqrt( pow(v1.x-v2.x, 2) + pow(v1.y-v2.y, 2) + pow(v1.z-v2.z, 2) );
}

inline double
	Quadrics::vertex_error(Matrix q, double x, double y, double z)
{
	return q[0]*x*x + 2*q[1]*x*y + 2*q[2]*x*z + 2*q[3]*x + q[5]*y*y
		+ 2*q[6]*y*z + 2*q[7]*y + q[10]*z*z + 2*q[11]*z + q[15];
}

void Quadrics::initial_quadrics() {
	for(int i=1; i<=vertices.size(); ++i) {
		quadrics.insert( Matrices::value_type(i, Matrix(0.0)) );
	}

	//compute initial quadric
	for(int i=0; i<faces.size(); ++i) {
		//face are triangles
		for(int j=0; j<3; ++j) {
			quadrics[ faces[i].id_vertex[j] ] += Matrix(faces[i].plane);
		}
	}
}

void Quadrics::select_pair() {
	int max_vid;
	int min_vid;
	int i, j;

	/* (v1, v2) is an edge */
	/* id_v1 < id_v2*/
	for (i = 0; i < static_cast<int>(faces.size()); i++)
	{
		min_vid = std::min(faces[i].id_vertex[0], faces[i].id_vertex[1]);
		max_vid = std::max(faces[i].id_vertex[0], faces[i].id_vertex[1]);
		if ( errors.find(Pair(min_vid, max_vid)) == errors.end() )
		{
			/* (faces[i].id_vertex[0], faces[i].id_vertex[1]) is an edge */
			errors.insert(Errors::value_type(Pair(min_vid, max_vid), calculate_error(min_vid, max_vid)));
		}

		min_vid = std::min(faces[i].id_vertex[0], faces[i].id_vertex[2]);
		max_vid = std::max(faces[i].id_vertex[0], faces[i].id_vertex[2]);
		if ( errors.find(Pair(min_vid, max_vid)) == errors.end() )
		{
			/* (faces[i].id_vertex[0], faces[i].id_vertex[2]) is an edge */
			errors.insert(Errors::value_type(Pair(min_vid, max_vid), calculate_error(min_vid, max_vid)));
		}

		min_vid = std::min(faces[i].id_vertex[1], faces[i].id_vertex[2]);
		max_vid = std::max(faces[i].id_vertex[1], faces[i].id_vertex[2]);
		if ( errors.find(Pair(min_vid, max_vid)) == errors.end() )
		{
			/* (faces[i].id_vertex[1], faces[i].id_vertex[2]) is an edge */
			errors.insert(Errors::value_type(Pair(min_vid, max_vid), calculate_error(min_vid, max_vid)));
		}
	}
}

double Quadrics::calculate_error(int id_v1, int id_v2, double* vx, double* vy, double* vz)
{
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
		
	//double vx3 = double (vx1+vx2)/2;
	//double vy3 = double (vy1+vy2)/2;
	//double vz3 = double (vz1+vz2)/2;

	double error1 = vertex_error(q_bar, vx1, vy1, vz1);
	double error2 = vertex_error(q_bar, vx2, vy2, vz2);
	//double error3 = vertex_error(q_bar, vx3, vy3, vz3);

	min_error = std::min(error1, error2);
	//min_error = std::min(error1, std::min(error2, error3));
	if (error1 == min_error) { *vx = vx1; *vy = vy1, *vz = vz1; }
	if (error2 == min_error) { *vx = vx2; *vy = vy2, *vz = vz2; }
	//if (error3 == min_error) { *vx = vx3; *vy = vy3, *vz = vz3; }

	min_error = vertex_error(q_bar, *vx, *vy, *vz);
	
	if (isReturnVertex == false) { delete vx; delete vy; delete vz; }
	return min_error;
}

void Quadrics::decimate(int target_vertex_num) {
	//calculate initial error for each valid pair
	initial_quadrics();
	select_pair();

	int id_v1, id_v2;
	double vx, vy, vz;

	while(vertices.size() > target_vertex_num) {
		//find least-error pair
		double min_error = INT_MAX;
		Errors::iterator iter_min_error;

		for(Errors::iterator iter = errors.begin(); iter != errors.end(); ++iter) {
			if(iter->second < min_error) {
				min_error = iter->second;
				iter_min_error = iter;
			}
			const Pair& p = iter->first;
		}

		Pair pair_min_error = iter_min_error->first;
		id_v1 = pair_min_error.first;
		id_v2 = pair_min_error.second;

		calculate_error(id_v1, id_v2, &vx, &vy, &vz);
		//update coordinate of v1
		vertices[id_v1].x = vx;
		vertices[id_v1].y = vy;
		vertices[id_v1].z = vz;

		//update quadric of v1
		quadrics[id_v1] = quadrics[id_v1] + quadrics[id_v2];

		//replace v2 with v1 in faces
		//remove faces that has edge of (v1, v2)
		for(Faces::iterator iter = faces.begin(); iter != faces.end(); ) {
			for(int j=0; j<3; ++j) {
				if(iter->id_vertex[j] == id_v2) {
					if (iter->id_vertex[0] == id_v1 || iter->id_vertex[1] == id_v1 || iter->id_vertex[2] == id_v1)
					{
						iter = faces.erase(iter);
					}
					else
					{
						iter->id_vertex[j] = id_v1;
						iter++;
					}
					break;
				}
				else if(j == 2)
					iter++;
			}
		}

		//remove v2 in vertices
		vertices.erase(id_v2);

		//merge pairs of v2 to v1
		Pair p;
		for(Errors::iterator iter = errors.begin(); iter != errors.end(); ) {
			p = iter->first;
			if(p.first == id_v2 && p.second != id_v1) {
				iter = errors.erase(iter);
				errors.insert(Errors::value_type( Pair(std::min(id_v1, p.second), std::max(id_v1, p.second)), 0.0 ));
			}
			else if(p.second == id_v2 && p.first != id_v1) {
				iter = errors.erase(iter);
				errors.insert(Errors::value_type( Pair(std::min(id_v1, p.first), std::max(id_v1, p.first)), 0 ));
			}
			else
				iter++;
		}

		//remove pair (v1, v2)
		errors.erase(iter_min_error);

		for(Errors::iterator iter = errors.begin(); iter != errors.end(); ++iter) {
			p = iter->first;
			if(p.first == id_v1) {
				iter->second = calculate_error(id_v1, p.second);
			}
			if(p.second == id_v1) {
				iter->second = calculate_error(id_v1, p.first);
			}
		}
	}
}

void Quadrics::write_smf(char* filename)
{
	FILE *file;

	if ((file = fopen(filename, "w")) == NULL)
	{
		fprintf(stderr, "write_pm() failed: can't write data file \"%s\".\n", filename);
		//system("PAUSE");
		exit(-1);
	}

	/* print header info */
	fprintf(file, "#$PM 0.1\n");
	fprintf(file, "#$vertices %d\n", vertices.size());
	fprintf(file, "#$faces %d\n", faces.size());
	fprintf(file, "#\n");

	/* print vertices */
	int ii = 1;
	std::map<int, int> imap;
	for (Vertices::iterator iter = vertices.begin(); iter != vertices.end(); iter++)
	{
		fprintf(file, "v %lf %lf %lf\n", /*iter->first, */iter->second.x, iter->second.y, iter->second.z);
		imap.insert(std::map<int, int>::value_type(iter->first, ii));
		ii++;
	}

	/* print faces */
	int v1, v2, v3;
	for (int i = 0; i < static_cast<int>(faces.size()); i++)
	{
		v1 = imap[faces[i].id_vertex[0]];
		v2 = imap[faces[i].id_vertex[1]];
		v3 = imap[faces[i].id_vertex[2]];
		fprintf(file, "f %d %d %d\n", v1, v2, v3);
	}

	/* print vsplits */
	/*
	for (int i = 0; i < static_cast<int>(vsplits.size()); i++)
	{
		fprintf(file, "vsplit %d %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", i+1, vsplits[i].v1.x, vsplits[i].v1.y, vsplits[i].v1.z, 
			vsplits[i].v2.x, vsplits[i].v2.y, vsplits[i].v2.z, vsplits[i].vf.x, vsplits[i].vf.y, vsplits[i].vf.z);
	}
	*/

	/* close the file */
	fclose(file);
}
