#ifndef _UI_MATH_MATRIX_H_
#define _UI_MATH_MATRIX_H_
#include "point.h"
#include "rect.h"
#include "quad.h"

// 一个3D变换测试网页
// http://ie.microsoft.com/testdrive/Graphics/hands-on-css3/hands-on_3d-transforms.htm
// 变换矩阵数学公式
// http://www.w3.org/TR/css3-transforms/#mathematical-description
// 3d变换阐述
// https://dev.opera.com/articles/understanding-3d-transforms/
namespace UI
{

typedef struct tagMATRIX33
{
	union
	{
		struct 
		{
			float  m11; float  m12; float  m13;
			float  m21; float  m22; float  m23;
			float  m31; float  m32; float  m33;
		};
		float  m[3][3];
	};
}
MATRIX33, *LPMATRIX33;

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


class UIAPI Matrix33 : public MATRIX33
{
public:
	Matrix33();
	Matrix33(
		float eM11, float eM12,
		float eM21, float eM22,
		float eDx, float eDy);

public:
	void  Set(LPMATRIX33 p);
	void  CopyTo(__out LPMATRIX33 p);
	void  Identity();
	bool  IsIdentity();
	bool  IsIdentityOrTranslation();

	Matrix33&  Translate(float tx, float ty);
	Matrix33&  Rotate(float fDeg);
	Matrix33&  Scale(float sx, float sy);
	float  Determinant();
	void  Inverse(LPMATRIX33 pMatrix);

	Matrix33&  Multiply(MATRIX33& m);

	void  MapPoint(POINT* pt, POINT* ptTransform);
	void  UnMapPoint(POINT* ptTransform, POINT* pt);
};

class UIAPI Matrix44 : public MATRIX44
{
public:
	Matrix44();
	Matrix44(LPMATRIX44);

public:
	void  Set(LPMATRIX44 p);
	void  CopyTo(__out LPMATRIX44 p);
	void  Identity();
    bool  IsIdentity();
	bool  IsIdentityOrTranslation();
	float  Determinant();
	void  Adjoint(LPMATRIX44 p);
	void  Inverse(LPMATRIX44 pMatrix);
    bool  IsBackfaceVisible();

	Matrix44&  Translate(float x, float y, float z);
	Matrix44&  Perspective(float d);  // perspective要放在rotate/scale/translate-z前面，否则乘出来的结果矩阵不正确。为什么?
	Matrix44&  Scale(float x, float y, float z);
	Matrix44&  Rotate(float x, float y, float z);
	Matrix44&  Rotate(float x, float y, float z, float angle);
	Matrix44&  Multiply(MATRIX44& m);
	
	void  ToMatrix33(__out LPMATRIX33 p);
	void  MakeAffine();
	
	bool  ProjectPoint(POINT* pt, POINT* pOut);

	void  Map(float x, float y, float& resultX, float& resultY);
	void  Map(float x, float y, float z, float& resultX, float& resultY, float& resultZ);
	void  MapPoint(POINT* pt, POINT* pOut);
	void  MapPoint(POINT pIn, POINTF2* pOut);
	void  MapPoint(POINTF2 pIn, POINTF2* pOut);
	void  MapPoint(POINTF3 pIn, POINTF3* pOut);
	void  MapRect2Quad(RECTF* prc, QUADF* pQuad);
	void  MapRect2Quad(RECT* prc, QUAD* pQuad);
};

}

#endif // _UI_MATH_MATRIX_H_