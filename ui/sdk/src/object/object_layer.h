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
  Object &GetObject() { return m_obj; }

  void OnSize(uint nWidth, uint nHeight);
  void OnObjPosInTreeChanged();

protected:
  Object &GetObj() override { return m_obj; }

  bool IsChildOf(ILayerContent *) override;
  bool IsVisible() override;
  float GetLayerScale() override;
  void Draw(ui::IRenderTarget *) override;
  void GetWindowRect(Rect *prcOut) override;
  void GetParentWindowRect(Rect *prcOut) override;
  void OnLayerDestory() override;
  Layer *GetParentLayer() override;
  Layer *GetNextLayer() override;

private:
  Object &m_obj;
  Layer *m_pLayer;
};

} // namespace ui