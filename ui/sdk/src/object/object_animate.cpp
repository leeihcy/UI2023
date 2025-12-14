#include "object_animate.h"
#include "object_layer.h"
#include "object.h"

namespace ui {

// 为动画准备一个layer，如果没有则创建layer
Layer *ObjectAnimate::GetLayerForAnimate() {
  Object& self = *(static_cast<Object*>(this));

  Layer *layer = self.GetSelfLayer();
  if (layer)
    return layer;

  self.CreateLayer();
  return self.GetSelfLayer();
}

unsigned char ObjectAnimate::GetOpacity() const
{
  const Object& self = *(static_cast<const Object*>(this));

	Layer* layer = self.GetSelfLayer();
	if (!layer)
		return 255;

	return layer->GetOpacity();
}
void  ObjectAnimate::SetOpacity(const unsigned char alpha, LayerAnimateParam* param)
{
	GetLayerForAnimate()->SetOpacity(alpha, param);
}
void  ObjectAnimate::TranslateBy(float x, float y, float z, LayerAnimateParam* param)
{
	GetLayerForAnimate()->TranslateBy(x, y, z, param);
}
void  ObjectAnimate::TranslateTo(float x, float y, float z, LayerAnimateParam* param)
{
	GetLayerForAnimate()->TranslateTo(x, y, z, param);
}
void  ObjectAnimate::TranslateXTo(float x, LayerAnimateParam* param)
{
	GetLayerForAnimate()->TranslateXTo(x, param);
}
void  ObjectAnimate::TranslateYTo(float y, LayerAnimateParam* param)
{
  Object& self = *(static_cast<Object*>(this));

	y = y - self.m_rcParent.top;
	GetLayerForAnimate()->TranslateYTo(y, param);
}
void  ObjectAnimate::TranslateToParent(int x, int y, LayerAnimateParam* param)
{
  Object& self = *(static_cast<Object*>(this));

	int xTranslate = x - self.m_rcParent.left;
	int yTranslate = y - self.m_rcParent.top;
	GetLayerForAnimate()->TranslateTo(
		(float)xTranslate, (float)yTranslate, 0, param);
}
void  ObjectAnimate::RotateXTo(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateXTo(degree, param);
}

void  ObjectAnimate::RotateXBy(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateXBy(degree, param);
}
void  ObjectAnimate::RotateYTo(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateYTo(degree, param);
}

void  ObjectAnimate::RotateYBy(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateYBy(degree, param);
}
void  ObjectAnimate::RotateZTo(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateZTo(degree, param);
}

void  ObjectAnimate::RotateZBy(float degree, LayerAnimateParam* param)
{
	GetLayerForAnimate()->RotateZBy(degree, param);
}
void  ObjectAnimate::ScaleTo(float x, float y, LayerAnimateParam* param)
{
	GetLayerForAnimate()->ScaleTo(x, y, param);
}
}