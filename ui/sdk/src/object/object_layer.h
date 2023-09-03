#pragma once
#include "src/layer/layer.h"

namespace ui {
class Object;
class Layer;

class ObjectLayer : public IObjectLayerContent {
public:
  ObjectLayer(Object &o);
  ~ObjectLayer();

  void CreateLayer();
  void TryDestroyLayer();
  void DestroyLayer();

  Layer *GetLayer() const { return m_pLayer; }
  Object &GetObjet() { return m_obj; }

  void OnSize(uint nWidth, uint nHeight);
  void OnObjPosInTreeChanged();

protected:
  Object &GetObj() override { return m_obj; }

  virtual bool IsChildOf(ILayerContent *);
  virtual bool IsVisible();
  virtual void Draw(ui::IRenderTarget *);
  virtual void GetWindowRect(Rect *prcOut);
  virtual void GetParentWindowRect(Rect *prcOut);
  virtual void OnLayerDestory();
  virtual Layer *GetParentLayer();
  virtual Layer *GetNextLayer();

private:
  Object &m_obj;
  Layer *m_pLayer;
};

} // namespace ui