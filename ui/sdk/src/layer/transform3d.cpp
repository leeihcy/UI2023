#include "include/inc.h"
#include "transform3d.h"
#include "include/common/math/math.h"

namespace ui {

Transform3D::Transform3D()
{
	identity();
}
Transform3D::~Transform3D()
{

}

Transform3D*  Transform3D::CreateInstance()
{
	return new Transform3D();
}

void  Transform3D::Release()
{
	delete this;
}

TRANSFORM_TYPE  Transform3D::get_type()
{
	return TRANSFORM_SOFT3D;
}

void Transform3D::identity()
{
	m_bInvalidate = true;
    m_backface_visibility = true;

	m_eOriginTypeX = /*TRANSFORM_ROTATE_ORIGIN_TOP*/TRANSFORM_ROTATE_ORIGIN_CENTER;
	m_eOriginTypeY = /*TRANSFORM_ROTATE_ORIGIN_TOP*/TRANSFORM_ROTATE_ORIGIN_CENTER;
	m_fOriginOffsetX = 0;
	m_fOriginOffsetY = 0;
	m_fOriginOffsetZ = 0;
	m_fWidth = 0;
	m_fHeight = 0;
	m_fx = 0;
	m_fy = 0;

	m_xPos = 0;
	m_yPos = 0;
	m_zPos = 0;
	m_xRotate = 0;
	m_yRotate = 0;
	m_zRotate = 0;
	m_xScale = 1;
	m_yScale = 1;
	m_zScale = 1;
	m_perspective = 0;

	m_matrix.Identity();
	m_matrixInverse.Identity();
}



bool  Transform3D::is_identity()
{
	if (m_xPos == 0 &&
		m_yPos == 0 &&
		m_zPos == 0 &&
		m_xScale == 1 &&
		m_yScale == 1 &&
		m_zScale == 1 && 
		IsEmptyDegree(m_xRotate) &&
		IsEmptyDegree(m_yRotate) &&
		IsEmptyDegree(m_zRotate))
	{
		return true;
	}
		
	return false;
		 
	/*if (m_bInvalidate)
		update();

	return m_matrix.IsIdentity();*/
}

void  Transform3D::set_transform_rotate_origin(
	TRANSFORM_ROTATE_ORIGIN eX, float fX,
	TRANSFORM_ROTATE_ORIGIN eY, float fY,
	float fZ)
{
	m_eOriginTypeX = eX;
	m_eOriginTypeY = eY;
	m_fOriginOffsetX = fX;
	m_fOriginOffsetY = fY;
	m_fOriginOffsetZ = fZ;
}

void  Transform3D::set_size(int w, int h)
{
	m_fWidth = (float)w;
	m_fHeight = (float)h;

	invalidate();
}

const double EPSILON = 1.00e-07;
#define FLOAT_EQ(x,v) (((v - EPSILON) < x) && (x <( v + EPSILON)))

// 绕自身中心旋转时，需要知道这个对象在屏幕中的位置，然后才能计算出真正的旋转矩阵。
void  Transform3D::set_pos(int x, int y)
{	
	float fx = (float)x;
	float fy = (float)y;

	if (FLOAT_EQ(m_fx, fx) && FLOAT_EQ(m_fy, fy))
		return;

	m_fx = fx;
	m_fy = fy;
	invalidate();
}

// void  Transform3D::get_matrix(LPMATRIX44 pMatrix)
// {
// 	if (!pMatrix)
// 		return;
// 
// 	if (m_bInvalidate)
// 		update();
// 
// 	m_matrix.CopyTo(pMatrix);
// }

void  Transform3D::mappoint_layer_2_view(Point* ptInLayer)
{
	m_matrix.MapPoint(ptInLayer, ptInLayer);
}
void  Transform3D::mappoint_view_2_layer(Point* ptInLayer)
{
	m_matrixInverse.ProjectPoint(ptInLayer, ptInLayer);
}

// void  Transform3D::maprect_layer_2_view(__in Rect* rcInLayer, __out QUAD* pqInView)
// {
// 	Point& ptA = pqInView->ptA;
// 	Point& ptB = pqInView->ptB;
// 	Point& ptC = pqInView->ptC;
// 	Point& ptD = pqInView->ptD;
// 
// 	ptA.x = rcInLayer->left;
// 	ptA.y = rcInLayer->top;
// 	ptB.x = rcInLayer->right;
// 	ptB.y = rcInLayer->top;
// 	ptC.x = rcInLayer->right;
// 	ptC.y = rcInLayer->bottom;
// 	ptD.x = rcInLayer->left;
// 	ptD.y = rcInLayer->bottom;
// 
// 	mappoint_layer_2_view(&ptA);
// 	mappoint_layer_2_view(&ptB);
// 	mappoint_layer_2_view(&ptC);
// 	mappoint_layer_2_view(&ptD);
// }

void  Transform3D::invalidate()
{
	m_bInvalidate = true;
}

void  Transform3D::update()
{
	if (!m_bInvalidate) 
		return;

	m_bInvalidate = false;
	m_matrix.Identity();

	// 绕中心点旋转
	float fx, fy, fz;
	get_origin(&fx, &fy, &fz);
	m_matrix.Translate(fx, fy, fz);

	// 透视，要放在rotate/scale前面
	if (m_perspective > 0)
		m_matrix.Perspective(m_perspective);

	// 旋转
	if (m_xRotate != 0 || m_yRotate != 0 || m_zRotate != 0)
		m_matrix.Rotate(m_xRotate, m_yRotate, m_zRotate);

	// 移动
	m_matrix.Translate(m_xPos, m_yPos, m_zPos);

	// 缩放
	if (m_xScale != 1 || m_yScale != 1 || m_zScale != 1)
		m_matrix.Scale(m_xScale, m_yScale, m_zScale);

	m_matrix.Translate(-fx, -fy, -fz);

	// 求出反转值
	m_matrix.Inverse(&m_matrixInverse);

#ifdef _DEBUGx
	Point pt= {50,50};
	mappoint_view_2_layer(&pt);
	mappoint_layer_2_view(&pt);
	UIASSERT(pt.x == 50 && pt.y == 50);
#endif
}

void Transform3D::rotate3d(float x, float y, float z)
{
	if (m_xRotate == x && m_yRotate == y && m_zRotate == z)
		return;

	m_xRotate = x;
	m_yRotate = y;
	m_zRotate = z;

	invalidate();
}

void  Transform3D::rotateX(float angle)
{
    if (angle == m_xRotate)
        return;

    m_xRotate = angle;
    invalidate();
}
void  Transform3D::rotateY(float angle)
{
    if (angle == m_yRotate)
        return;

    m_yRotate = angle;
    invalidate();
}
void  Transform3D::rotateZ(float angle)
{
    if (angle == m_zRotate)
        return;

    m_zRotate = angle;
    invalidate();
}

void  Transform3D::translate3d(float x, float y, float z)
{
	if (m_xPos == x && m_yPos == y && m_zPos == z)
		return;

	m_xPos = x;
	m_yPos = y;
	m_zPos = z;

	invalidate();
}

void  Transform3D::scale3d(float x, float y, float z)
{
	if (x == 0)
		x = 1;
	if (y == 0)
		y = 1;
	if (z == 0)
		z = 1;

	if (m_xScale == x && m_yScale == y && m_zScale == z)
		return;

	m_xScale = x;
	m_yScale = y;
	m_zScale = z;

	invalidate();
}
void  Transform3D::scaleX(float x)
{
	if (m_xScale == x)
		return;

	m_xScale = x;
	invalidate();
}
void  Transform3D::scaleY(float y)
{
	if (m_yScale == y)
		return;

	m_yScale = y;
	invalidate();
}
void  Transform3D::scaleZ(float z)
{
	if (m_zScale == z)
		return;

	m_zScale = z;
	invalidate();
}

// 如果不加透视，rotate-y是没有3d效果的，只会显示为内容变窄了。
// http://www.w3.org/TR/css3-transforms/
//
// 指定观察者距离z=0平面的距离，为元素及其内容应用透视变换。当值为0或负值时，无透视变换。
void  Transform3D::perspective(float n)
{
	if (n <= 0)
		m_perspective = 0;
	else
		m_perspective = n;

	invalidate();
}

void  Transform3D::get_origin(float* pfx, float* pfy, float* pfz)
{
	if (pfx)
	{
		switch (m_eOriginTypeX)
		{
		case TRANSFORM_ROTATE_ORIGIN_ABSOLUTE:
		case TRANSFORM_ROTATE_ORIGIN_LEFT:
			*pfx = m_fOriginOffsetX;
			break;

		case TRANSFORM_ROTATE_ORIGIN_RIGHT:
			*pfx = m_fWidth - m_fOriginOffsetX;
			break;

		case TRANSFORM_ROTATE_ORIGIN_CENTER:
			*pfx = m_fWidth/2 + m_fOriginOffsetX;
			break;
        default:
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
			*pfy = m_fOriginOffsetY;
			break;

		case TRANSFORM_ROTATE_ORIGIN_BOTTOM:
			*pfy = m_fHeight - m_fOriginOffsetY;
			break;

		case TRANSFORM_ROTATE_ORIGIN_CENTER:
			*pfy = m_fHeight/2 + m_fOriginOffsetY;
			break;
        default:
            break;
		}

		*pfy += m_fy;
	}

	if (pfz)
	{
		*pfz = m_fOriginOffsetZ;
	}
}

void  Transform3D::set_backface_visibility(bool b)
{
    m_backface_visibility = b;
}
bool  Transform3D::get_backface_visibility()
{
    return m_backface_visibility;
}
bool  Transform3D::is_visible()
{
    if (m_backface_visibility)
        return true;

    update();
    return !m_matrix.IsBackfaceVisible();
}

void Transform3D::get_matrix(MATRIX44* pMatrix)
{
	if (!pMatrix)
		return;

	update();
	*pMatrix = m_matrix;
}

}