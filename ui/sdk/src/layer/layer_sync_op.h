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
  bool visible;
  float dpi_scale;
  ui::Rect rect_in_window;
  ui::Rect clip_rect;
  int opacity;

  float m_fxRotate;          // 设置的值
  float m_fyRotate;          // 设置的值
  float m_fzRotate;          // 设置的值
  float m_fxScale;           // 设置的值
  float m_fyScale;           // 设置的值
                             
  // 相对于控件m_rcParent的相对值
  float m_xTranslate;        // 设置的值
  float m_yTranslate;        // 设置的值
  float m_zTranslate;        // 设置的值
};
} // namespace ui

#endif