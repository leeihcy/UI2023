#include "stdafx.h"
#include "matrix.h"
#include "math.h"

// AffineTransform  (webkit 3*3)
// TransformationMatrix (webkit 4*4)


//
//  float = determinant2x2(float a, float b, float c, float d)
//
//  calculate the determinant of a 2x2 matrix.
//
static float determinant2x2(float& a, float& b, float& c, float& d)
{
	return a * d - b * c;
}

//
//  float = determinant3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3)
//
//  Calculate the determinant of a 3x3 matrix
//  in the form
//
//      | a1,  b1,  c1 |
//      | a2,  b2,  c2 |
//      | a3,  b3,  c3 |
//
static float determinant3x3(
	float& a1, float& a2, float& a3, 
	float& b1, float& b2, float& b3, 
	float& c1, float& c2, float& c3)
{
	return a1 * determinant2x2(b2, b3, c2, c3)
		- b1 * determinant2x2(a2, a3, c2, c3)
		+ c1 * determinant2x2(a2, a3, b2, b3);
}


Matrix33::Matrix33()
{
	Identity();
}

Matrix33::Matrix33(
		float eM11, float eM12,
		float eM21, float eM22,
		float eDx, float eDy)
{
	Identity();

	m11 = eM11;
	m12 = eM12;
	m21 = eM21;
	m22 = eM22;
	m31 = eDx;
	m32 = eDy;
}

void  Matrix33::Identity()
{
	memset(m, 0, sizeof(float)*3*3);
	m11 = m22 = m33 = 1;
}

void  Matrix33::Set(LPMATRIX33 p)
{
	memcpy(m, p, sizeof(MATRIX33));
}

void  Matrix33::CopyTo(__out LPMATRIX33 p)
{
	memcpy(p, m, sizeof(MATRIX33));
}

// 行列式
//      | a1,  b1,  c1 |
//      | a2,  b2,  c2 |
//      | a3,  b3,  c3 |

float  Matrix33::Determinant()
{
	float& a1 = m11;
	float& a2 = m21;
	float& a3 = m31;

	float& b1 = m12;
	float& b2 = m22;
	float& b3 = m32;

	float& c1 = m13;
	float& c2 = m23;
	float& c3 = m33;

	return determinant3x3(a1,a2,a3,b1,b2,b3,c1,c2,c3);
}

bool Matrix33::IsIdentity()
{
	return (m11 == 1 && m12 == 0 &&m13 == 0 &&
			m21 == 0 && m22 == 1 &&m23 == 0 &&
			m31 == 0 && m32 == 0 &&m33 == 1);
}

bool Matrix33::IsIdentityOrTranslation()
{
	return (
		m11 == 1 && m12 == 0 && m13 == 0 &&
		m21 == 0 && m22 == 1 && m23 == 0 &&
		m33 == 1);
}


Matrix33&  Matrix33::Multiply(MATRIX33& matrix)
{
	Matrix33 tmp;
    tmp.m[0][0] = (matrix.m[0][0] * m[0][0] + matrix.m[0][1] * m[1][0] + matrix.m[0][2] * m[2][0]);
    tmp.m[0][1] = (matrix.m[0][0] * m[0][1] + matrix.m[0][1] * m[1][1] + matrix.m[0][2] * m[2][1]);
    tmp.m[0][2] = (matrix.m[0][0] * m[0][2] + matrix.m[0][1] * m[1][2] + matrix.m[0][2] * m[2][2]);

    tmp.m[1][0] = (matrix.m[1][0] * m[0][0] + matrix.m[1][1] * m[1][0] + matrix.m[1][2] * m[2][0]);
    tmp.m[1][1] = (matrix.m[1][0] * m[0][1] + matrix.m[1][1] * m[1][1] + matrix.m[1][2] * m[2][1]);
    tmp.m[1][2] = (matrix.m[1][0] * m[0][2] + matrix.m[1][1] * m[1][2] + matrix.m[1][2] * m[2][2]);

    tmp.m[2][0] = (matrix.m[2][0] * m[0][0] + matrix.m[2][1] * m[1][0] + matrix.m[2][2] * m[2][0]);
    tmp.m[2][1] = (matrix.m[2][0] * m[0][1] + matrix.m[2][1] * m[1][1] + matrix.m[2][2] * m[2][1]);
    tmp.m[2][2] = (matrix.m[2][0] * m[0][2] + matrix.m[2][1] * m[1][2] + matrix.m[2][2] * m[2][2]);

    Set(&tmp);
	return *this;
}

void  Matrix33::Inverse(LPMATRIX33 pMatrix)
{
	Matrix33 mIndentify;
	memcpy(pMatrix, &mIndentify, sizeof(MATRIX33));

	float determinant = Determinant();
	if (determinant == 0.0)
		return;

	if (IsIdentityOrTranslation()) 
	{
		pMatrix->m31 = -m31;
		pMatrix->m32 = -m32;
		return;
	}

	pMatrix->m11 =  m22 / determinant;
	pMatrix->m12 = -m12 / determinant;
	pMatrix->m21 = -m21 / determinant;
	pMatrix->m22 =  m11 / determinant;
	pMatrix->m31 = (m21 * m32 - m22 * m31) / determinant;
	pMatrix->m32 = (m12 * m31 - m11 * m32) / determinant;
}


// *this = *this * translation
Matrix33& Matrix33::Translate(float tx, float ty)
{
	if (IsIdentityOrTranslation()) {
		m31 += tx;
		m32 += ty;
		return *this;
	}

	m31 += tx * m11 + ty * m21;
	m32 += tx * m12 + ty * m22;
	return *this;
}

Matrix33& Matrix33::Rotate(float fDeg)
{
	// angle is in degree. Switch to radian
	float fRad = deg2rad(fDeg);
	float cosAngle = cos(fRad);
	float sinAngle = sin(fRad);
	Matrix33 rot(cosAngle, sinAngle, -sinAngle, cosAngle, 0, 0);

	Multiply(rot);
	return *this;
}

Matrix33& Matrix33::Scale(float sx, float sy)
{
	m11 *= sx;
	m12 *= sx;
	m21 *= sy;
	m22 *= sy;

	return *this;
}

void  Matrix33::MapPoint(POINT* pt, POINT* ptTransform)
{
	float x = (float)pt->x;
	float y = (float)pt->y;
	float fx = (m[0][0] * x + m[1][0] * y + m[2][0]);
	float fy = (m[0][1] * x + m[1][1] * y + m[2][1]);

	ptTransform->x = _round(fx);
	ptTransform->y = _round(fy);
}

// [注]unmap采用的另一种方法，先将matrix逆转，用逆矩阵进行映射
//
// m11*x + m21*y = x' - m31;
// m12*x + m22*y = y' - m32
//
// --> 
//  设 b1 = x' - m31, b2 = y' - m32;
//  解得
//          b1*m22 - b2*m21
//     x = -----------------
//         m11*m22 - m12*m21
//  
void  Matrix33::UnMapPoint(POINT* ptTransform, POINT* pt)
{
	// 使用行列式解二元一次方程
	float b1 = (float)ptTransform->x - m31;
	float b2 = (float)ptTransform->y - m32;

	float D = m11*m22 - m12*m21;
	if (D == 0)
	{
		pt->x = ptTransform->x;
		pt->y = ptTransform->y;
		return;
	}

	float D1 = b1*m22 - m21*b2;
	float D2 = m11*b2 - b1*m12;

	pt->x = _round(D1 / D);
	pt->y = _round(D2 / D);
}

//////////////////////////////////////////////////////////////////////////

Matrix44::Matrix44()
{
	Identity();
}

Matrix44::Matrix44(LPMATRIX44 p)
{
	if (p)
		Set(p);
	else
		Identity();
}

void  Matrix44::Identity()
{
	memset(m, 0, sizeof(float)*4*4);
	m11 = m22 = m33 = m44 = 1;
}

bool  Matrix44::IsIdentity()
{
    return m[0][0] == 1 && m[0][1] == 0 && m[0][2] == 0 && m[0][3] == 0
        && m[1][0] == 0 && m[1][1] == 1 && m[1][2] == 0 && m[1][3] == 0
        && m[2][0] == 0 && m[2][1] == 0 && m[2][2] == 1 && m[2][3] == 0
        && m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
}

bool  Matrix44::IsIdentityOrTranslation()
{
	return m[0][0] == 1 && m[0][1] == 0 && m[0][2] == 0 && m[0][3] == 0
		&& m[1][0] == 0 && m[1][1] == 1 && m[1][2] == 0 && m[1][3] == 0
		&& m[2][0] == 0 && m[2][1] == 0 && m[2][2] == 1 && m[2][3] == 0
		&& m[3][3] == 1;
}

void  Matrix44::Set(LPMATRIX44 p)
{
	memcpy(m, p, sizeof(MATRIX44));
}

void  Matrix44::CopyTo(__out LPMATRIX44 p)
{
	memcpy(p->m, m, sizeof(MATRIX44));
}

void  Matrix44::ToMatrix33(__out LPMATRIX33 p)
{
	p->m11 = m11;
	p->m12 = m12;
	p->m21 = m21;
	p->m22 = m22;
	p->m31 = m41;
	p->m32 = m42;
}

void  Matrix44::MakeAffine()
{
	m[0][2] = 0;
	m[0][3] = 0;

	m[1][2] = 0;
	m[1][3] = 0;

	m[2][0] = 0;
	m[2][1] = 0;
	m[2][2] = 1;
	m[2][3] = 0;

	m[3][2] = 0;
	m[3][3] = 1;
}

float  Matrix44::Determinant()
{
	// Assign to individual variable names to aid selecting
	// correct elements

	float& a1 = m[0][0];
	float& b1 = m[0][1];
	float& c1 = m[0][2];
	float& d1 = m[0][3];

	float& a2 = m[1][0];
	float& b2 = m[1][1];
	float& c2 = m[1][2];
	float& d2 = m[1][3];

	float& a3 = m[2][0];
	float& b3 = m[2][1];
	float& c3 = m[2][2];
	float& d3 = m[2][3];

	float& a4 = m[3][0];
	float& b4 = m[3][1];
	float& c4 = m[3][2];
	float& d4 = m[3][3];

	return a1 * determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4)
		 - b1 * determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4)
		 + c1 * determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4)
		 - d1 * determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

void  Matrix44::Inverse(LPMATRIX44 pMatrix)
{
	memset(pMatrix, 0, sizeof(MATRIX44));
	pMatrix->m11 = pMatrix->m22 = pMatrix->m33 = pMatrix->m44 = 1;

	if (IsIdentityOrTranslation()) 
	{
		// identity matrix
		if (m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0)
			return;

		// translation
		pMatrix->m41 = -m41;
		pMatrix->m42 = -m42;
		pMatrix->m43 = -m43;
		return;
	}

	// Calculate the 4x4 determinant
	// If the determinant is zero,
	// then the inverse matrix is not unique.
	float det = Determinant();
	if (fabs(det) < EPSILON_E6)
		return;

	// Calculate the adjoint matrix
	Adjoint(pMatrix);

	// Scale the adjoint matrix to get the inverse
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pMatrix->m[i][j] = pMatrix->m[i][j] / det;
		}
	}
}

void  Matrix44::Adjoint(LPMATRIX44 p)
{
	float a1 = m[0][0];
	float b1 = m[0][1];
	float c1 = m[0][2];
	float d1 = m[0][3];

	float a2 = m[1][0];
	float b2 = m[1][1];
	float c2 = m[1][2];
	float d2 = m[1][3];

	float a3 = m[2][0];
	float b3 = m[2][1];
	float c3 = m[2][2];
	float d3 = m[2][3];

	float a4 = m[3][0];
	float b4 = m[3][1];
	float c4 = m[3][2];
	float d4 = m[3][3];

	// Row column labeling reversed since we transpose rows & columns
	p->m[0][0]  =   determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
	p->m[1][0]  = - determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
	p->m[2][0]  =   determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
	p->m[3][0]  = - determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

	p->m[0][1]  = - determinant3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
	p->m[1][1]  =   determinant3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
	p->m[2][1]  = - determinant3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
	p->m[3][1]  =   determinant3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

	p->m[0][2]  =   determinant3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
	p->m[1][2]  = - determinant3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
	p->m[2][2]  =   determinant3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
	p->m[3][2]  = - determinant3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

	p->m[0][3]  = - determinant3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
	p->m[1][3]  =   determinant3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
	p->m[2][3]  = - determinant3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
	p->m[3][3]  =   determinant3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

// [1,   0,   0,   0]
// [0,   1,   0,   0]
// [0,   0,   1, -1/d]
// [0,   0,   0,   1]
Matrix44&  Matrix44::Perspective(float d)
{
	if (d == 0)
		return *this;

	Matrix44  mat;
	mat.m[2][3] = 1/d;  // 这里没有用-1/d。在这里与css3不同，认为为z值越大，对象越往屏幕内部，看起来应该越小
	Multiply(mat);

	return *this;
}

// 注：矩阵乘法不满足交换律
Matrix44&  Matrix44::Multiply(MATRIX44& matrix)
{
	Matrix44 tmp;

    tmp.m[0][0] = (matrix.m[0][0] * m[0][0] + matrix.m[0][1] * m[1][0] + matrix.m[0][2] * m[2][0] + matrix.m[0][3] * m[3][0]);
    tmp.m[0][1] = (matrix.m[0][0] * m[0][1] + matrix.m[0][1] * m[1][1] + matrix.m[0][2] * m[2][1] + matrix.m[0][3] * m[3][1]);
    tmp.m[0][2] = (matrix.m[0][0] * m[0][2] + matrix.m[0][1] * m[1][2] + matrix.m[0][2] * m[2][2] + matrix.m[0][3] * m[3][2]);
    tmp.m[0][3] = (matrix.m[0][0] * m[0][3] + matrix.m[0][1] * m[1][3] + matrix.m[0][2] * m[2][3] + matrix.m[0][3] * m[3][3]);

    tmp.m[1][0] = (matrix.m[1][0] * m[0][0] + matrix.m[1][1] * m[1][0] + matrix.m[1][2] * m[2][0] + matrix.m[1][3] * m[3][0]);
    tmp.m[1][1] = (matrix.m[1][0] * m[0][1] + matrix.m[1][1] * m[1][1] + matrix.m[1][2] * m[2][1] + matrix.m[1][3] * m[3][1]);
    tmp.m[1][2] = (matrix.m[1][0] * m[0][2] + matrix.m[1][1] * m[1][2] + matrix.m[1][2] * m[2][2] + matrix.m[1][3] * m[3][2]);
    tmp.m[1][3] = (matrix.m[1][0] * m[0][3] + matrix.m[1][1] * m[1][3] + matrix.m[1][2] * m[2][3] + matrix.m[1][3] * m[3][3]);

    tmp.m[2][0] = (matrix.m[2][0] * m[0][0] + matrix.m[2][1] * m[1][0] + matrix.m[2][2] * m[2][0] + matrix.m[2][3] * m[3][0]);
    tmp.m[2][1] = (matrix.m[2][0] * m[0][1] + matrix.m[2][1] * m[1][1] + matrix.m[2][2] * m[2][1] + matrix.m[2][3] * m[3][1]);
    tmp.m[2][2] = (matrix.m[2][0] * m[0][2] + matrix.m[2][1] * m[1][2] + matrix.m[2][2] * m[2][2] + matrix.m[2][3] * m[3][2]);
    tmp.m[2][3] = (matrix.m[2][0] * m[0][3] + matrix.m[2][1] * m[1][3] + matrix.m[2][2] * m[2][3] + matrix.m[2][3] * m[3][3]);

    tmp.m[3][0] = (matrix.m[3][0] * m[0][0] + matrix.m[3][1] * m[1][0] + matrix.m[3][2] * m[2][0] + matrix.m[3][3] * m[3][0]);
    tmp.m[3][1] = (matrix.m[3][0] * m[0][1] + matrix.m[3][1] * m[1][1] + matrix.m[3][2] * m[2][1] + matrix.m[3][3] * m[3][1]);
    tmp.m[3][2] = (matrix.m[3][0] * m[0][2] + matrix.m[3][1] * m[1][2] + matrix.m[3][2] * m[2][2] + matrix.m[3][3] * m[3][2]);
    tmp.m[3][3] = (matrix.m[3][0] * m[0][3] + matrix.m[3][1] * m[1][3] + matrix.m[3][2] * m[2][3] + matrix.m[3][3] * m[3][3]);

    Set(&tmp);
	return *this;
}


// [1,    0,    0,    0]
// [0,    1,    0,    0]
// [0,    0,    1,    0]
// [tx,   ty,   tz,   1]
Matrix44&  Matrix44::Translate(float tx, float ty, float tz)
{
	m[3][0] += tx * m[0][0] + ty * m[1][0] + tz * m[2][0];
	m[3][1] += tx * m[0][1] + ty * m[1][1] + tz * m[2][1];
	m[3][2] += tx * m[0][2] + ty * m[1][2] + tz * m[2][2];
	m[3][3] += tx * m[0][3] + ty * m[1][3] + tz * m[2][3];

// 	m[3][0] += tx;
// 	m[3][1] += ty;
// 	m[3][2] += tz;
	return *this;
}

// [sx,   0,    0,    0]
// [0,    sy,   0,    0]
// [0,    0,    sz,   0]
// [0,    0,    0,    1]
Matrix44&  Matrix44::Scale(float sx, float sy, float sz)
{
	m[0][0] *= sx;
	m[0][1] *= sx;
	m[0][2] *= sx;
	m[0][3] *= sx;

	m[1][0] *= sy;
	m[1][1] *= sy;
	m[1][2] *= sy;
	m[1][3] *= sy;

	m[2][0] *= sz;
	m[2][1] *= sz;
	m[2][2] *= sz;
	m[2][3] *= sz;

	return *this;
}

// 绕向量[x,y,z]旋转angle度
Matrix44&  Matrix44::Rotate(float x, float y, float z, float angle)
{
	// Normalize the axis of rotation
	float length = sqrt(x * x + y * y + z * z);
	if (length == 0) 
	{
		// A direction vector that cannot be normalized, such as [0, 0, 0], will cause the rotation to not be applied.
		return *this;
	}
	else if (length != 1) 
	{
		x /= length;
		y /= length;
		z /= length;
	}

	// Angles are in degrees. Switch to radians.
	angle = deg2rad(angle);

	float sinTheta = sin(angle);
	float cosTheta = cos(angle);

	Matrix44 mat;

	// Optimize cases where the axis is along a major axis
	if (x == 1.0 && y == 0.0 && z == 0.0) 
	{
		mat.m[0][0] = 1.0;
		mat.m[0][1] = 0.0;
		mat.m[0][2] = 0.0;
		mat.m[1][0] = 0.0;
		mat.m[1][1] = cosTheta;
		mat.m[1][2] = sinTheta;
		mat.m[2][0] = 0.0;
		mat.m[2][1] = -sinTheta;
		mat.m[2][2] = cosTheta;
		mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = 0.0;
		mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0;
		mat.m[3][3] = 1.0;
	} 
	else if (x == 0.0 && y == 1.0 && z == 0.0)
	{
		mat.m[0][0] = cosTheta;
		mat.m[0][1] = 0.0;
		mat.m[0][2] = -sinTheta;
		mat.m[1][0] = 0.0;
		mat.m[1][1] = 1.0;
		mat.m[1][2] = 0.0;
		mat.m[2][0] = sinTheta;
		mat.m[2][1] = 0.0;
		mat.m[2][2] = cosTheta;
		mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = 0.0;
		mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0;
		mat.m[3][3] = 1.0;
	}
	else if (x == 0.0 && y == 0.0 && z == 1.0)
	{
		mat.m[0][0] = cosTheta;
		mat.m[0][1] = sinTheta;
		mat.m[0][2] = 0.0;
		mat.m[1][0] = -sinTheta;
		mat.m[1][1] = cosTheta;
		mat.m[1][2] = 0.0;
		mat.m[2][0] = 0.0;
		mat.m[2][1] = 0.0;
		mat.m[2][2] = 1.0;
		mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = 0.0;
		mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0;
		mat.m[3][3] = 1.0;
	} 
	else 
	{
		// This case is the rotation about an arbitrary unit vector.
		//
		// Formula is adapted from Wikipedia article on Rotation matrix,
		// http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
		//
		// An alternate resource with the same matrix: http://www.fastgraph.com/makegames/3drotation/
		//
		float oneMinusCosTheta = 1 - cosTheta;
		mat.m[0][0] = cosTheta + x * x * oneMinusCosTheta;
		mat.m[0][1] = y * x * oneMinusCosTheta + z * sinTheta;
		mat.m[0][2] = z * x * oneMinusCosTheta - y * sinTheta;
		mat.m[1][0] = x * y * oneMinusCosTheta - z * sinTheta;
		mat.m[1][1] = cosTheta + y * y * oneMinusCosTheta;
		mat.m[1][2] = z * y * oneMinusCosTheta + x * sinTheta;
		mat.m[2][0] = x * z * oneMinusCosTheta + y * sinTheta;
		mat.m[2][1] = y * z * oneMinusCosTheta - x * sinTheta;
		mat.m[2][2] = cosTheta + z * z * oneMinusCosTheta;
		mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = 0.0;
		mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0;
		mat.m[3][3] = 1.0;
	}

	Multiply(mat);
	return *this;
}

//
// x:
// [1,    0,    0,    0]
// [0,   cos@, sin@,  0]
// [0,  -sin@, cos@,  0]
// [0,    0,    0,    1]
//
// y:
// [cos@, 0,  -sin@,  0]
// [0,    1,    0,    0]
// [sin@, 0,   cos@,  0]
// [0,    0,    0,    1]
//
// z:
// [cos@, sin@, 0,    0]
// [-sin@,cos@, 0,    0]
// [0,    0,    1,    0]
// [0,    0,    0,    1]
//
Matrix44&  Matrix44::Rotate(float x, float y, float z)
{
	float rx = deg2rad(x);
	float ry = deg2rad(y);
	float rz = deg2rad(z);

	Matrix44 mat;

	float sinTheta = sin(rz);
	float cosTheta = cos(rz);

	mat.m[0][0] = cosTheta;
	mat.m[0][1] = sinTheta;
	mat.m[0][2] = 0.0;
	mat.m[1][0] = -sinTheta;
	mat.m[1][1] = cosTheta;
	mat.m[1][2] = 0.0;
	mat.m[2][0] = 0.0;
	mat.m[2][1] = 0.0;
	mat.m[2][2] = 1.0;
	mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = 0.0;
	mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0;
	mat.m[3][3] = 1.0;

	Matrix44 rmat(mat);

	sinTheta = sin(ry);
	cosTheta = cos(ry);

	mat.m[0][0] = cosTheta;
	mat.m[0][1] = 0.0;
	mat.m[0][2] = -sinTheta;
	mat.m[1][0] = 0.0;
	mat.m[1][1] = 1.0;
	mat.m[1][2] = 0.0;
	mat.m[2][0] = sinTheta;
	mat.m[2][1] = 0.0;
	mat.m[2][2] = cosTheta;
	mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = 0.0;
	mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0;
	mat.m[3][3] = 1.0;

	rmat.Multiply(mat);

	sinTheta = sin(rx);
	cosTheta = cos(rx);

	mat.m[0][0] = 1.0;
	mat.m[0][1] = 0.0;
	mat.m[0][2] = 0.0;
	mat.m[1][0] = 0.0;
	mat.m[1][1] = cosTheta;
	mat.m[1][2] = sinTheta;
	mat.m[2][0] = 0.0;
	mat.m[2][1] = -sinTheta;
	mat.m[2][2] = cosTheta;
	mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = 0.0;
	mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0;
	mat.m[3][3] = 1.0;

	rmat.Multiply(mat);

	Multiply(rmat);
	return *this;
}

void  Matrix44::MapPoint(POINT* pt, POINT* ptTransform)
{
	float fx, fy;
	this->Map((float)pt->x, (float)pt->y, fx, fy);

	ptTransform->x = _round(fx);
	ptTransform->y = _round(fy);
}


// Webkit TransformationMatrix.cpp
// TransformationMatrix::projectPoint
//
// This is basically raytracing. We have a point in the destination
// plane with z=0, and we cast a ray parallel to the z-axis from that
// point to find the z-position at which it intersects the z=0 plane
// with the transform applied. Once we have that point we apply the
// inverse transform to find the corresponding point in the source
// space.
//
// Given a plane with normal Pn, and a ray starting at point R0 and
// with direction defined by the vector Rd, we can find the
// intersection point as a distance d from R0 in units of Rd by:
//
// d = -dot (Pn', R0) / dot (Pn', Rd)
//
//
// 将屏幕上的点(z=0)映射到该矩阵变换后的一个平面上，用于鼠标hittest
//
bool  Matrix44::ProjectPoint(POINT* pt, POINT* pOut)
{
	if (m33 == 0) // 90度的平面，不可见。
	{
		return false;
	}

	float x = (float)pt->x;
	float y = (float)pt->y;
	float z = -(m13 * x + m23 * y + m43) / m33;

	float outX = x * m11 + y * m21 + z * m31 + m41;
	float outY = x * m12 + y * m22 + z * m32 + m42;

	float w = x * m14 + y * m24 + z * m34 + m44;
	if (w <= 0) 
	{
		return false;
	} 
	else if (w != 1)
	{
		outX /= w;
		outY /= w;
	}

	pOut->x = _round(outX);
	pOut->y = _round(outY);
	return true;
}

void  Matrix44::MapPoint(POINT pIn, POINTF2* pOut)
{
	POINTF2  point = {(float)pIn.x, (float)pIn.y};
	MapPoint(point, pOut);
}
void  Matrix44::MapPoint(POINTF2 pIn, POINTF2* pOut)
{
	Map(pIn.x, pIn.y, pOut->x, pOut->y);
}
void  Matrix44::MapPoint(POINTF3 pIn, POINTF3* pOut)
{
	Map(pIn.x, pIn.y, pIn.z, pOut->x, pOut->y, pOut->z);
}

void Matrix44::Map(float x, float y, float& resultX, float& resultY)
{
	float z;
	Map(x, y, 0, resultX, resultY, z);

	return;

// 	if (IsIdentityOrTranslation())
// 	{
// 		resultX = x + m[3][0];
// 		resultY = y + m[3][1];
// 		return;
// 	}
// 
// 	resultX = m[3][0] + x * m[0][0] + y * m[1][0];
// 	resultY = m[3][1] + x * m[0][1] + y * m[1][1];
// 	float w = m[3][3] + x * m[0][3] + y * m[1][3];
// 
// 	if (w != 1 && w != 0)
// 	{
// 		resultX /= w;
// 		resultY /= w;
// 	}
}

void Matrix44::Map(
	float x, float y, float z,
	float& resultX,
	float& resultY, 
	float& resultZ)
{
	if (IsIdentityOrTranslation())
	{
		resultX = x + m[3][0];
		resultY = y + m[3][1];
		resultZ = z + m[3][2];
		return;
	}

	resultX = m[3][0] + x * m[0][0] + y * m[1][0] + z * m[2][0];
	resultY = m[3][1] + x * m[0][1] + y * m[1][1] + z * m[2][1];
	resultZ = m[3][2] + x * m[0][2] + y * m[1][2] + z * m[2][2];
	float w = m[3][3] + x * m[0][3] + y * m[1][3] + z * m[2][3];

	if (w != 1 && w != 0) 
	{
		resultX /= w;
		resultY /= w;
		resultZ /= w;
	}
}

void  Matrix44::MapRect2Quad(RECTF* prc, QUADF* pQuad)
{
	if (IsIdentityOrTranslation()) 
	{
		QuadF* p = static_cast<QuadF*>(pQuad);
		p->SetByRect(prc);
		p->Offset(m[3][0], m[3][1]);
		return;
	}

	Map(prc->left,  prc->top,     pQuad->Ax, pQuad->Ay);
	Map(prc->right, prc->top,     pQuad->Bx, pQuad->By);
	Map(prc->right, prc->bottom,  pQuad->Cx, pQuad->Cy);
	Map(prc->left,  prc->bottom,  pQuad->Dx, pQuad->Dy);
}

void  Matrix44::MapRect2Quad(RECT* prc, QUAD* pQuad)
{
	RectF rc(prc);
	QuadF quadf;

	MapRect2Quad(&rc, &quadf);
	quadf.ToQuad(pQuad);
}

//
// 计算应用了该matrix之后，对象的背面是否朝着相机（可见了）。
// https://code.google.com/p/chromium/codesearch#chromium/src/ui/gfx/transform.cc&q=gfx::Transform::IsBackFaceVisible&sq=package:chromium&type=cs&l=219
// 
bool  Matrix44::IsBackfaceVisible()
{
    // Compute whether a layer with a forward-facing normal of (0, 0, 1, 0)
    // would have its back face visible after applying the transform.
    if (IsIdentity())
        return false;

    // This is done by transforming the normal and seeing if the resulting z
    // value is positive or negative. However, note that transforming a normal
    // actually requires using the inverse-transpose of the original transform.
    //
    // We can avoid inverting and transposing the matrix since we know we want
    // to transform only the specific normal vector (0, 0, 1, 0). In this case,
    // we only need the 3rd row, 3rd column of the inverse-transpose. We can
    // calculate only the 3rd row 3rd column element of the inverse, skipping
    // everything else.
    //
    // For more information, refer to:
    //   http://en.wikipedia.org/wiki/Invertible_matrix#Analytic_solution
    //

    float determinant = this->Determinant();

    // If matrix was not invertible, then just assume back face is not visible.
    if (fabs(determinant) <= EPSILON_E5)
        return false;

    // Compute the cofactor of the 3rd row, 3rd column.
    float cofactor_part_1 = m[0][0] * m[1][1] * m[3][3];
    float cofactor_part_2 = m[0][1] * m[1][3] * m[3][0];
    float cofactor_part_3 = m[0][3] * m[1][0] * m[3][1];
    float cofactor_part_4 = m[0][0] * m[1][3] * m[3][1];
    float cofactor_part_5 = m[0][1] * m[1][0] * m[3][3];
    float cofactor_part_6 = m[0][3] * m[1][1] * m[3][0];

    float cofactor33 =
        cofactor_part_1 +
        cofactor_part_2 +
        cofactor_part_3 -
        cofactor_part_4 -
        cofactor_part_5 -
        cofactor_part_6;

    // Technically the transformed z component is cofactor33 / determinant.  But
    // we can avoid the costly division because we only care about the resulting
    // +/- sign; we can check this equivalently by multiplication.
    return cofactor33 * determinant < 0;
}