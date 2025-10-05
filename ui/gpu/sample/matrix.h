#pragma once

typedef struct tagMATRIX44
{
	union
	{
		struct 
		{
			float  m11; float  m12; float  m13; float  m14;
			float  m21; float  m22; float  m23; float  m24;
			float  m31; float  m32; float  m33; float  m34;
			float  m41; float  m42; float  m43; float  m44;
		};
		float  m[4][4];
	};
}
MATRIX44, *LPMATRIX44;


class Matrix44 : public MATRIX44
{
public:
    Matrix44();
    void Identity();

	Matrix44& Translate(float x, float y, float z);
	Matrix44& Scale(float x, float y, float z);
	Matrix44& Rotate(float x, float y, float z);
	Matrix44& Rotate(float x, float y, float z, float angle);
    Matrix44& Perspective(float d);

    void Set(LPMATRIX44 p);
    Matrix44& Multiply(MATRIX44& matrix);
};

const double piDouble = 3.14159265358979323846;
const float piFloat = 3.14159265358979323846f;

inline double deg2rad(double d)  { return d * piDouble / 180.0; }
inline double rad2deg(double r)  { return r * 180.0 / piDouble; }
inline float  deg2rad(float d)   { return d * piFloat / 180.0f; }
inline float  rad2deg(float r)   { return r * 180.0f / piFloat; }


Matrix44::Matrix44()
{
	Identity();
}

void  Matrix44::Identity()
{
	memset(m, 0, sizeof(float)*4*4);
	m11 = m22 = m33 = m44 = 1;
}

void  Matrix44::Set(LPMATRIX44 p)
{
	memcpy(m, p, sizeof(MATRIX44));
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
	float length = (float)sqrt(x * x + y * y + z * z);
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

	float sinTheta = (float)sin(angle);
	float cosTheta = (float)cos(angle);

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

	float sinTheta = (float)sin(rz);
	float cosTheta = (float)cos(rz);

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

	sinTheta = (float)sin(ry);
	cosTheta = (float)cos(ry);

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

	sinTheta = (float)sin(rx);
	cosTheta = (float)cos(rx);

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