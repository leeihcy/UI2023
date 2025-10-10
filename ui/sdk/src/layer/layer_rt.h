#ifndef _UI_SDK_SRC_LAYER_LAYER_SYNC_H_
#define _UI_SDK_SRC_LAYER_LAYER_SYNC_H_
#include "layer_sync_op.h"
#include "transform3d.h"
#include "ui/sdk/include/common/math/rect.h"
#include <memory>

// 将layer tree同步一份到render thread，避免跨线程引用。

namespace ui {
class IGpuLayer;
class GpuLayerCommitContext;

class LayerRT {
public:
  LayerRT(LAYERID id);

  bool AddSubLayer(LayerRT *pLayer, LayerRT *pInsertBeforeChild);
  void RemoveMeInTheTree();

  void BindGpuLayer(std::shared_ptr<IGpuLayer> gpu_layer);
  void HardwareCommit(GpuLayerCommitContext *context);
public:
  LayerRT *m_pParent = nullptr;
  LayerRT *m_pFirstChild = nullptr;
  LayerRT *m_pNext = nullptr;
  LayerRT *m_pPrev = nullptr;

  LAYERID  m_layer_id;
  std::shared_ptr<IGpuLayer> m_gpu_layer;

  LayerTreeProperties m_properties;
  Transform3D m_transfrom3d;
};


} // namespace ui

#endif