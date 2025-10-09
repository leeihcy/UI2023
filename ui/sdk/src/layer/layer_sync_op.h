#ifndef _UI_SDK_SRC_LAYER_LAYER_SYNC_OP_H_
#define _UI_SDK_SRC_LAYER_LAYER_SYNC_OP_H_
#include "ui/sdk/include/common/math/rect.h"

namespace ui {

typedef unsigned long long LAYERID;

enum class LayerTreeSyncOpType {
  Add,
  Remove, // remove from the tree
  Update,
  Move,
  Delete, // destruct
};

struct LayerTreeSyncOperation {
  LayerTreeSyncOpType type;
  LAYERID id;
  LAYERID parent_id;
  LAYERID next_id;
};

struct LayerTreeProperties {
  ui::Rect rect_in_parent;
};
} // namespace ui

#endif