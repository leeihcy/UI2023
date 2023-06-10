#pragma once
#include "../common/math/matrix.h"
#include "Inc/Interface/irenderlayer.h"

namespace UI
{

class Transform3D : public IRenderLayerTransform3D
{
public:
	Transform3D();
	~Transform3D();

public:
	static Transform3D*  CreateInstance();

	virtual void  Release() override;
	virtual TRANSFORM_TYPE  get_type() override;

	// anchorPoint
	virtual void  set_transform_rotate_origin(
					TRANSFORM_ROTATE_ORIGIN eX, float fX,
					TRANSFORM_ROTATE_ORIGIN eY, float fY,
					float fZ) override;
	virtual void  set_pos(int x, int y) override;
	virtual void  set_size(int w, int h) override;
	virtual void  mappoint_layer_2_view(__inout POINT* ptInLayer) override;
	virtual void  mappoint_view_2_layer(__inout POINT* ptInLayer) override;
	//virtual void  maprect_layer_2_view(__in RECT* rcInLayer, __out QUAD* pqInView);

    virtual void  set_backface_visibility(bool b) override;
    virtual bool  get_backface_visibility() override;
    virtual bool  is_visible() override;

	virtual void  get_matrix(MATRIX44* pMatrix) override;;
	virtual void  translate3d(float x, float y, float z) override;
	virtual void  translateX(float x) override {};
	virtual void  translateY(float y) override {};
	virtual void  translateZ(float z) override {};
	virtual void  scale3d(float x, float y, float z) override;
	virtual void  scaleX(float x) override;
	virtual void  scaleY(float y) override;
	virtual void  scaleZ(float z) override;
	virtual void  rotate3d(float x, float y, float z) override;
	virtual void  rotateX(float angle) override;
	virtual void  rotateY(float angle) override;
	virtual void  rotateZ(float angle) override;
	virtual void  perspective(float n) override;
	
    float  get_translateX() {
                return m_xPos; }
    float  get_translateY() {
                return m_yPos; }
    float  get_translateZ() {
                return m_zPos; }
	float  get_rotateX() { return m_xRotate; }
	float  get_rotateY() { return m_yRotate; }
	float  get_rotateZ() { return m_zRotate; }
	float  get_scaleX() { return m_xScale; }
	float  get_scaleY() { return m_yScale; }

	void  identity();
	bool  is_identity();

protected:
	void  invalidate();
	void  update();
	void  get_origin(float* pfx, float* pfy, float* pfz);

private:
	Matrix44  m_matrix;
	Matrix44  m_matrixInverse;
	bool   m_bInvalidate;
    bool   m_backface_visibility;

	// 旋转中心相关数据
	TRANSFORM_ROTATE_ORIGIN  m_eOriginTypeX;
	TRANSFORM_ROTATE_ORIGIN  m_eOriginTypeY;
	float  m_fOriginOffsetX;
	float  m_fOriginOffsetY;
	float  m_fOriginOffsetZ; 

	float  m_fx;
	float  m_fy;
	float  m_fWidth;
	float  m_fHeight;

	float  m_xPos;
	float  m_yPos;
	float  m_zPos;
	float  m_xRotate;
	float  m_yRotate;
	float  m_zRotate;
	float  m_xScale;
	float  m_yScale;
	float  m_zScale;
	float  m_perspective;
};

}
