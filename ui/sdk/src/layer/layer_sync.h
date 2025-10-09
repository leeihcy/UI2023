#ifndef _UI_SDK_SRC_LAYER_LAYER_SYNC_H_
#define _UI_SDK_SRC_LAYER_LAYER_SYNC_H_
#include "layer_sync_op.h"
#include "ui/sdk/include/common/math/rect.h"

// 将layer tree同步一份到render thread，避免跨线程引用。

namespace ui {

class LayerRT {
public:
  LayerRT(LAYERID id);

  bool AddSubLayer(LayerRT *pLayer, LayerRT *pInsertBeforeChild);
  void RemoveMeInTheTree();
  
public:
  LayerRT *m_pParent = nullptr;
  LayerRT *m_pFirstChild = nullptr;
  LayerRT *m_pNext = nullptr;
  LayerRT *m_pPrev = nullptr;

  LAYERID  m_layer_id;
};


} // namespace ui

#endif