#ifndef _UI_SDK_SRC_OBJECT_OBJECTANIMATE_H_
#define _UI_SDK_SRC_OBJECT_OBJECTANIMATE_H_


namespace ui {
struct LayerAnimateParam;
class Layer;

class ObjectAnimate {
public:
  Layer *GetLayerForAnimate();

public:
  unsigned char GetOpacity() const;
  void SetOpacity(const unsigned char alpha, LayerAnimateParam *param);

  void TranslateBy(float x, float y, float z, LayerAnimateParam *param);
  void TranslateTo(float x, float y, float z, LayerAnimateParam *param);
  void TranslateXTo(float x, LayerAnimateParam *param);
  void TranslateYTo(float y, LayerAnimateParam *param);
  void TranslateToParent(int x, int y, LayerAnimateParam *param);

  void RotateXTo(float degree, LayerAnimateParam *param);
  void RotateXBy(float degree, LayerAnimateParam *param);
  void RotateYTo(float degree, LayerAnimateParam *param);
  void RotateYBy(float degree, LayerAnimateParam *param);
  void RotateZTo(float degree, LayerAnimateParam *param);
  void RotateZBy(float degree, LayerAnimateParam *param);

  void ScaleTo(float x, float y, LayerAnimateParam *param = nullptr);
};

}

#endif