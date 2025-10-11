#include "sdk/src/layer/layer_rt.h"
#include "include/macro/helper.h"
#include "gpu/include/api.h"
#include "include/util/rect.h"

namespace ui {

LayerRT::LayerRT(LAYERID id) : m_layer_id(id) {
  memset(&m_properties, 0, sizeof(m_properties));
  m_properties.visible = true;
  m_properties.rect_in_window = ui::Rect::MakeXYWH(0, 0, 200, 200);
}
    
bool LayerRT::AddSubLayer(LayerRT *pLayer, LayerRT *pInsertBeforeChild) {
  if (pLayer->m_pParent)
    return false;

  if (!m_pFirstChild) {
    m_pFirstChild = pLayer;
  } else {
    // 放在最后面
    if (!pInsertBeforeChild) {
      LayerRT *p = m_pFirstChild;
      while (p && p->m_pNext) {
        p = p->m_pNext;
      }

      p->m_pNext = pLayer;
      pLayer->m_pPrev = p;
    } else {
      // 作为第一个子结点
      if (m_pFirstChild == pInsertBeforeChild) {
        m_pFirstChild = pLayer;
      }

      if (pInsertBeforeChild->m_pPrev)
        pInsertBeforeChild->m_pPrev->m_pNext = pLayer;
      pLayer->m_pPrev = pInsertBeforeChild->m_pPrev;

      pInsertBeforeChild->m_pPrev = pLayer;
      pLayer->m_pNext = pInsertBeforeChild;
    }
  }

  pLayer->m_pParent = this;
  return true;
}

void LayerRT::RemoveMeInTheTree() {
  if (m_pPrev) {
    m_pPrev->m_pNext = m_pNext;
  } else {
    if (m_pParent)
      m_pParent->m_pFirstChild = m_pNext;
  }

  if (m_pNext)
    m_pNext->m_pPrev = m_pPrev;

  m_pParent = nullptr;
  m_pPrev = nullptr;
  m_pNext = nullptr;
}

void LayerRT::BindGpuLayer(std::shared_ptr<IGpuLayer> gpu_layer) {
  m_gpu_layer = gpu_layer;
}


void LayerRT::HardwareCommit(GpuLayerCommitContext *pContext) {
  UIASSERT(m_gpu_layer);
  if (!m_gpu_layer) {
    return;
  }
  Rect bound = m_properties.rect_in_window;
  bound.Scale(m_properties.dpi_scale);
  pContext->SetOffset(bound.left, bound.top);

  Rect rect_clip = m_properties.clip_rect;
  rect_clip.Scale(m_properties.dpi_scale);
  pContext->SetClipRect(&rect_clip);

  if (m_properties.opacity != 255) {
    pContext->MultiAlpha(m_properties.opacity);
  }

  // 绕自身中心旋转时，需要知道这个对象在屏幕中的位置，然后才能计算出真正的旋转矩阵。
  // 因此每次使用前设置一次。
  m_transfrom3d.set_pos(bound.left, bound.top);
  m_transfrom3d.set_size(bound.Width(), bound.Height());

  m_transfrom3d.rotateZ(m_properties.m_fzRotate);  
  if (!m_transfrom3d.is_identity()) {
    MATRIX44 mat;
    m_transfrom3d.get_matrix(&mat);
    m_gpu_layer->Compositor(pContext, (float *)&mat);
  } else {
    m_gpu_layer->Compositor(pContext, nullptr);
  }
}

}