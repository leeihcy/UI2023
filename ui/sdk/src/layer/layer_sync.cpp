#include "sdk/src/layer/layer_sync.h"

namespace ui {

LayerRT::LayerRT(LAYERID id) : m_layer_id(id) {

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

}