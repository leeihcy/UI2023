#pragma once
#include "../UISDK/Inc/Interface/irenderlayer.h"
#include "../common/math/quad.h"
#include "../common/math/matrix.h"

namespace UI
{

class Hard3DTransform : public IRenderLayerTransform3D
{
public:
	Hard3DTransform();
	~Hard3DTransform();

public:
	static Hard3DTransform*  CreateInstance();

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
	virtual void  maprect_layer_2_view(__in RECT* rcInLayer, __out QUAD* pqInView);

    virtual void  set_backface_visibility(bool b) override;
    virtual bool  get_backface_visibility() override;
    virtual bool  is_visible() override;

	virtual void  get_matrix(MATRIX44* pMatrix) override;
	virtual void  translate3d(float x, float y, float z) override;
	virtual void  translateX(float x) override;
	virtual void  translateY(float y) override;
	virtual void  translateZ(float z) override;
	virtual void  scale3d(float x, float y, float z) override;
	virtual void  scaleX(float x) override;
	virtual void  scaleY(float y) override;
	virtual void  scaleZ(float z) override;
	virtual void  rotate3d(float x, float y, float z) override;
	virtual void  rotateX(float angle) override;
	virtual void  rotateY(float angle) override;
	virtual void  rotateZ(float angle) override;
	virtual void  perspective(float n) override;
	
public:
	void  GetLocalPos(RECTF* prcLocal);
    void  GetWorldMatrix(D3DXMATRIX*);

protected:
	void  identity();
	
	void  Invalidate();
	void  Update();
	void  GetRotateOrigin(float* pfx, float* pfy, float* pfz);

public:
	D3DXMATRIX  m_matrix;
	D3DXMATRIX  m_matrixInverse;

private:
	bool   m_bInvalidate;
    bool   m_backface_visibility;

	// 旋转中心相关数据
	TRANSFORM_ROTATE_ORIGIN  m_eOriginTypeX;
	TRANSFORM_ROTATE_ORIGIN  m_eOriginTypeY;
	float  m_fAnchorPointX;  // 旋转中心
	float  m_fAnchorPointY;
	float  m_fAnchorPointZ; 

    // 矩形对象参数
    float  m_fx;
    float  m_fy;
	float  m_fWidth;
	float  m_fHeight;

    // 变换参数
	float  m_xTranslate;
	float  m_yTranslate;
	float  m_zTranslate;
	float  m_xRotate;
	float  m_yRotate;
	float  m_zRotate;
	float  m_xScale;
	float  m_yScale;
	float  m_zScale;
	float  m_perspective;
};

}
