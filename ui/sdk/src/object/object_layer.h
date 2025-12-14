#pragma once
#include "src/layer/layer.h"

namespace ui {
class Object;
class Layer;

class ObjectLayer : public IObjectLayerContent {
public:
  ~ObjectLayer();

  void CreateLayer();
  void TryDestroyLayer();
  void DestroyLayer();

  Layer *GetLayer() const { return m_pLayer; }

  void OnSize(uint width, uint height, float scale);
  void OnObjPosInTreeChanged();

protected:
  bool IsChildOf(ILayerContent *) override;
  bool IsLayerVisible() override;
  float GetLayerScale() override;
  void LayerDraw(ui::IRenderTarget *) override;
  void GetLayerWindowRect(Rect *prcOut) override;
  void GetLayerParentWindowRect(Rect *prcOut) override;
  void OnLayerDestory() override;
  Layer *GetParentLayer() override;
  Layer *GetNextLayer() override;

  Object& GetLayerContentObject() override;

protected:
  Layer *m_pLayer = nullptr;
};

} // namespace ui