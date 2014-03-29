#ifndef __MATRIX__
#define __MATRIX__

class Matrix {
public:
	Matrix() {
		for(int i=0; i<16; ++i) {
			m[i] = 0.0;
		}
	}

	Matrix(double c) {
		for(int i=0; i<16; ++i) {
			m[i] = c;
		}
	}

	Matrix(	double m11, double m12, double m13, double m14, 
		double m21, double m22, double m23, double m24,
		double m31, double m32, double m33, double m34,
		double m41, double m42, double m43, double m44) {
			m[0] = m11;  m[1] = m12;  m[2] = m13;  m[3] = m14; 
			m[4] = m21;  m[5] = m22;  m[6] = m23;  m[7] = m24; 
			m[8] = m31;  m[9] = m32; m[10] = m33; m[11] = m34;
			m[12] = m41; m[13] = m42; m[14] = m43; m[15] = m44;
	}

	/* 
	* matrix constructor for a specific plane
	*
	* plane - a 4-dimension array 
	*/
	Matrix(double plane[]) {
		double a = plane[0];
		double b = plane[1];
		double c = plane[2];
		double d = plane[3];

		m[0] = a*a;  m[1] = a*b;  m[2] = a*c;  m[3] = a*d; 
		m[4] = a*b;  m[5] = b*b;  m[6] = b*c;  m[7] = b*d; 
		m[8] = a*c;  m[9] = b*c; m[10] = c*c; m[11] = c*d;
		m[12] = a*d; m[13] = b*d; m[14] = c*d; m[15] = d*d;
	}

	double operator[](int c) const {
		return m[c];
	}

	/* 
	 * calculate determinant of a 3*3 matrix 
	 *
	 * a(mn)  - index of matrix element
	 */
	double det(	int a11, int a12, int a13,
				int a21, int a22, int a23,
				int a31, int a32, int a33) {
		double det = m[a11]*m[a22]*m[a33] + m[a13]*m[a21]*m[a32] + m[a12]*m[a23]*m[a31] 
					- m[a13]*m[a22]*m[a31] - m[a11]*m[a23]*m[a32] - m[a12]*m[a21]*m[a33]; 
		return det;
	}

	/* For test only */
	void print() {
		for (int i = 0; i < 16; ) {
			printf("%.8lf ", m[i++]);
			if (i % 4 == 0) {
				printf("\n");
			}
		}
	}

	const Matrix operator+(const Matrix& n) const { 
		return Matrix( 
			m[0]+n[0],   m[1]+n[1],   m[2]+n[2],   m[3]+n[3], 
			m[4]+n[4],   m[5]+n[5],   m[6]+n[6],   m[7]+n[7], 
			m[8]+n[8],   m[9]+n[9],  m[10]+n[10], m[11]+n[11],
			m[12]+n[12], m[13]+n[13], m[14]+n[14], m[15]+n[15]);
	}

	Matrix& operator+=(const Matrix& n) {
		m[0]+=n[0];   m[1]+=n[1];   m[2]+=n[2];   m[3]+=n[3]; 
		m[4]+=n[4];   m[5]+=n[5];   m[6]+=n[6];   m[7]+=n[7]; 
		m[8]+=n[8];   m[9]+=n[9];  m[10]+=n[10]; m[11]+=n[11];
		m[12]+=n[12]; m[13]+=n[13]; m[14]+=n[14]; m[15]+=n[15]; 
		return *this; 
	}

private:
	double m[16];
};

#endif

