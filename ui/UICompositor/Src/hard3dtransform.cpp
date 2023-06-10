#include "stdafx.h"
#include "hard3dtransform.h"

using namespace UI;

Hard3DTransform::Hard3DTransform()
{
	identity();
}
Hard3DTransform::~Hard3DTransform()
{

}

Hard3DTransform*  Hard3DTransform::CreateInstance()
{
	return new Hard3DTransform();
}

void  Hard3DTransform::Release()
{
	delete this;
}

TRANSFORM_TYPE  Hard3DTransform::get_type()
{
	return TRANSFORM_HARD3D;
}

void Hard3DTransform::identity()
{
	m_bInvalidate = true;
    m_backface_visibility = true;

	m_eOriginTypeX = TRANSFORM_ROTATE_ORIGIN_CENTER;
	m_eOriginTypeY = TRANSFORM_ROTATE_ORIGIN_CENTER;
	m_fAnchorPointX = 0;
	m_fAnchorPointY = 0;
	m_fAnchorPointZ = 0;
	m_fWidth = 0;
	m_fHeight = 0;

	m_xTranslate = 0;
	m_yTranslate = 0;
	m_zTranslate = 0;
	m_xRotate = 0;
	m_yRotate = 0;
	m_zRotate = 0;
	m_xScale = 1;
	m_yScale = 1;
	m_zScale = 1;
	m_perspective = 0;

	D3DXMatrixIdentity(&m_matrix);
	D3DXMatrixIdentity(&m_matrixInverse);
}

void  Hard3DTransform::set_transform_rotate_origin(
	TRANSFORM_ROTATE_ORIGIN eX, float fX,
	TRANSFORM_ROTATE_ORIGIN eY, float fY,
	float fZ)
{
	m_eOriginTypeX = eX;
	m_eOriginTypeY = eY;
	m_fAnchorPointX = fX;
	m_fAnchorPointY = fY;
	m_fAnchorPointZ = fZ;
}

void  Hard3DTransform::set_pos(int x, int y)
{
    m_fx = (float)x;
    m_fy = (float)y;
}

void  Hard3DTransform::set_size(int w, int h)
{
	m_fWidth = (float)w;
	m_fHeight = (float)h;
}

void  Hard3DTransform::get_matrix(LPMATRIX44 pMatrix)
{
	if (!pMatrix)
		return;

	if (m_bInvalidate)
		Update();

	memcpy(pMatrix, &m_matrix, sizeof(MATRIX44));
}

void  Hard3DTransform::mappoint_layer_2_view(__inout POINT* ptInLayer)
{
// TODO: m_matrix.MapPoint(ptInLayer, ptInLayer);
}
void  Hard3DTransform::mappoint_view_2_layer(__inout POINT* ptInLayer)
{
// TODO: m_matrixInverse.MapPoint(ptInLayer, ptInLayer);
}

void  Hard3DTransform::maprect_layer_2_view(__in RECT* rcInLayer, __out QUAD* pqInView)
{
	POINT& ptA = pqInView->ptA;
	POINT& ptB = pqInView->ptB;
	POINT& ptC = pqInView->ptC;
	POINT& ptD = pqInView->ptD;

	ptA.x = rcInLayer->left;
	ptA.y = rcInLayer->top;
	ptB.x = rcInLayer->right;
	ptB.y = rcInLayer->top;
	ptC.x = rcInLayer->right;
	ptC.y = rcInLayer->bottom;
	ptD.x = rcInLayer->left;
	ptD.y = rcInLayer->bottom;

	mappoint_layer_2_view(&ptA);
	mappoint_layer_2_view(&ptB);
	mappoint_layer_2_view(&ptC);
	mappoint_layer_2_view(&ptD);
}

void  Hard3DTransform::Invalidate()
{
	m_bInvalidate = true;
}

inline float  deg2rad(float d)   { return d * 3.14159265358979323846f / 180.0f; }

void  Hard3DTransform::Update()
{
	if (!m_bInvalidate) 
		return;

	D3DXMATRIX  tempMatrix;
	m_bInvalidate = false;
	D3DXMatrixIdentity(&m_matrix);
   
	// 要先旋转，再移动。否则旋转会基于移动进行大旋转，而不是绕对象本身。
	// 旋转
	if (m_xRotate != 0 || m_yRotate != 0 || m_zRotate != 0)
	{
		D3DXMatrixRotationYawPitchRoll(&tempMatrix, 
			deg2rad(m_yRotate), deg2rad(m_xRotate), deg2rad(m_zRotate));
		m_matrix *= tempMatrix;
	} 

	// 移动
	D3DXMatrixTranslation(&tempMatrix, m_xTranslate, m_yTranslate, m_zTranslate);
	m_matrix *= tempMatrix;
 	
	// 缩放
	if (m_xScale != 1 || m_yScale != 1 || m_zScale != 1)
	{
		D3DXMatrixScaling(&tempMatrix, m_xScale, m_yScale, m_zScale);
		m_matrix *= tempMatrix;
	}

    // 透视
    if (m_perspective > 0)
    {
        D3DXMATRIX  matrix;
        D3DXMatrixIdentity(&matrix);

        matrix.m[2][3] = 1.0f/m_perspective;
        m_matrix *= matrix;
    }

	// 求出反转值
	float fDet = D3DXMatrixDeterminant(&m_matrix);
	D3DXMatrixInverse(&m_matrixInverse, &fDet, &m_matrix);
}

void Hard3DTransform::rotate3d(float x, float y, float z)
{
	if (m_xRotate == x && m_yRotate == y && m_zRotate == z)
		return;

	m_xRotate = x;
	m_yRotate = y;
	m_zRotate = z;

	Invalidate();
}

void  Hard3DTransform::rotateX(float angle)
{
    if (m_xRotate == angle)
        return;

    m_xRotate = angle;
    Invalidate();
}
void  Hard3DTransform::rotateY(float angle)
{
    if (m_yRotate == angle)
        return;

    m_yRotate = angle;
    Invalidate();
}
void  Hard3DTransform::rotateZ(float angle)
{
    if (m_zRotate == angle)
        return;

    m_zRotate = angle;
    Invalidate();
}

void  Hard3DTransform::translate3d(float x, float y, float z)
{
	if (m_xTranslate == x && m_yTranslate == y && m_zTranslate == z)
		return;

	m_xTranslate = x;
	m_yTranslate = y;
	m_zTranslate = z;

	Invalidate();
}

void  Hard3DTransform::translateX(float x)
{
    translate3d(x, m_yTranslate, m_zTranslate);
}
void  Hard3DTransform::translateY(float y)
{
    translate3d(m_xTranslate, y, m_zTranslate);
}
void  Hard3DTransform::translateZ(float z)
{
    translate3d(m_xTranslate, m_yTranslate, z);
}

void  Hard3DTransform::scale3d(float x, float y, float z)
{
	if (m_xScale == x && m_yScale == y && m_zScale == z)
		return;

	m_xScale = x;
	m_yScale = y;
	m_zScale = z;

	Invalidate();
}
void  Hard3DTransform::scaleX(float x)
{
	if (m_xScale == x)
		return;

	m_xScale = x;
	Invalidate();
}
void  Hard3DTransform::scaleY(float y)
{
	if (m_yScale == y)
		return;

	m_yScale = y;
	Invalidate();
}
void  Hard3DTransform::scaleZ(float z)
{
	if (m_zScale == z)
		return;

	m_zScale = z;
	Invalidate();
}

// 如果不加透视，rotate-y是没有3d效果的，只会显示为内容变窄了。
// http://www.w3.org/TR/css3-transforms/
//
// 指定观察者距离z=0平面的距离，为元素及其内容应用透视变换。当值为0或负值时，无透视变换。
void  Hard3DTransform::perspective(float n)
{
	if (n <= 0)
		m_perspective = 0;
	else
		m_perspective = n;

	Invalidate();
}

void  Hard3DTransform::GetRotateOrigin(float* pfx, float* pfy, float* pfz)
{
	if (pfx)
	{
		switch (m_eOriginTypeX)
		{
		case TRANSFORM_ROTATE_ORIGIN_ABSOLUTE:
		case TRANSFORM_ROTATE_ORIGIN_LEFT:
			*pfx = m_fAnchorPointX;
			break;

		case TRANSFORM_ROTATE_ORIGIN_RIGHT:
			*pfx = m_fWidth - m_fAnchorPointX;
			break;

		case TRANSFORM_ROTATE_ORIGIN_CENTER:
			*pfx = m_fWidth/2 + m_fAnchorPointX;
			break;
		}

        *pfx += m_fx;
	}
	if (pfy)
	{
		switch (m_eOriginTypeY)
		{
		case TRANSFORM_ROTATE_ORIGIN_ABSOLUTE:
		case TRANSFORM_ROTATE_ORIGIN_TOP:
			*pfy = m_fAnchorPointY;
			break;

		case TRANSFORM_ROTATE_ORIGIN_BOTTOM:
			*pfy = m_fHeight - m_fAnchorPointY;
			break;

		case TRANSFORM_ROTATE_ORIGIN_CENTER:
			*pfy = m_fHeight/2 + m_fAnchorPointY;
			break;
		}

        *pfy += m_fy;
	}

	if (pfz)
	{
		*pfz = m_fAnchorPointZ;
	}
}

void  Hard3DTransform::set_backface_visibility(bool b)
{
    m_backface_visibility = b;
}
bool  Hard3DTransform::get_backface_visibility()
{
    return m_backface_visibility;
}
bool  Hard3DTransform::is_visible()
{
	return true;
// TODO:
//     if (m_backface_visibility)
//         return true;
// 
//     Update();
//     return !m_matrix.IsBackfaceVisible();
}

void  Hard3DTransform::GetLocalPos(RECTF* prcLocal)
{
	prcLocal->left = m_fx;
	prcLocal->right = m_fx + m_fWidth;
	prcLocal->top = m_fy;
	prcLocal->bottom = m_fy+ m_fHeight;
}

// 获取对象的变换矩阵
// get_matrix没有考虑自转
// TODO: 将get_matrix与GetWorldMatrix合并成一个函数。增加一个invalid字段
void  Hard3DTransform::GetWorldMatrix(D3DXMATRIX* pMatrix)
{
    float fx, fy, fz;
    GetRotateOrigin(&fx, &fy, &fz);

    D3DXMATRIX matrixRotateCenter;
    D3DXMatrixIdentity(&matrixRotateCenter);
    D3DXMatrixTranslation(&matrixRotateCenter, -fx, -fy, -fz);

    D3DXMATRIX matrixRotateBack;
    D3DXMatrixIdentity(&matrixRotateBack);
    D3DXMatrixTranslation(&matrixRotateBack, fx, fy, fz);

    Update(); // get_matrix

    *pMatrix = matrixRotateCenter * m_matrix;
    *pMatrix = (*pMatrix) * matrixRotateBack;
}