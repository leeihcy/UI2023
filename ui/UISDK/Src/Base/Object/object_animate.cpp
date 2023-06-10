#include "stdafx.h"
#include "object.h"
#include "Src\Layer\layer.h"
#include "object_layer.h"

unsigned char Object::GetOpacity() const
{
	Layer* layer = GetSelfLayer();
	if (!layer)
		return 255;

	return layer->GetOpacity();
}
void  Object::SetOpacity(const unsigned char alpha, LayerAnimateParam* param)
{
	GetLayerForAnimate()->SetOpacity(alpha, param);
}
void  Object::TranslateBy(float x, float y, float z, LayerAnimateParam* param)
{
	GetLayerForAnimate()->TranslateBy(x, y, z, param);
}
void  Object::TranslateTo(float x, float y, float z, LayerAnimateParam* param)
{
	GetLayerForAnimate()->TranslateTo(x, y, z, param);
}
void  Object::TranslateXTo(float x, LayerAnimateParam* param)
{
	GetLayerForAnimate()->TranslateXTo(x, param);
}
void  Object::TranslateYTo(float y, LayerAnimateParam* param)
{
	y = y - m_rcParent.top;
	GetLayerForAnimate()->TranslateYTo(y, param);
}
void  Object::TranslateToParent(int x, int y, LayerAnimateParam* param)
{
	int xTranslate = x - m_rcParent.left;
	int yTranslate = y - m_rcParent.top;
	GetLayerForAnimate()->TranslateTo(
		(float)xTranslate, (float)yTranslate, 0, param);
}
void  Object::RotateXTo(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateXTo(degree, param);
}

void  Object::RotateXBy(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateXBy(degree, param);
}
void  Object::RotateYTo(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateYTo(degree, param);
}

void  Object::RotateYBy(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateYBy(degree, param);
}
void  Object::RotateZTo(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateZTo(degree, param);
}

void  Object::RotateZBy(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateZBy(degree, param);
}
void  Object::ScaleTo(float x, float y, LayerAnimateParam* param)
{
	GetLayerForAnimate()->ScaleTo(x, y, param);
}