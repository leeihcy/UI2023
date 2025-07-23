#include "layout_object.h"
#include "src/attribute/attribute.h"
#include "src/object/object.h"
#include "src/layout/canvaslayout.h"

namespace ui {

LayoutObject::LayoutObject(Object &obj) : m_object(obj) {
  m_pLayoutParam = nullptr;

  m_lzOrder = 0;

  m_nMaxWidth = NDEF;
  m_nMaxHeight = NDEF;
  m_lMinWidth = m_lMinHeight = NDEF;
}

LayoutObject::~LayoutObject() {
  SAFE_RELEASE(m_pLayoutParam);
}

void LayoutObject::onSerialize(SerializeParam *pData) {
  if (pData->IsReload()) {
    SAFE_RELEASE(m_pLayoutParam);
  }

  AttributeSerializer s(pData, "LayoutObject");
  s.AddInt(
      XML_ZORDER,
      *(int *)&m_lzOrder); // z序
                           // (注：默认在从xml加载之后，AddChild之前会先解析一次)
  s.AddInt(XML_MAXWIDTH, *(int *)&m_nMaxWidth)
      ->SetDefault(NDEF)
      ->SetDpiScaleType(LONGATTR_DPI_SCALE_GRATETHAN_0);
  s.AddInt(XML_MAXHEIGHT, *(int *)&m_nMaxHeight)
      ->SetDefault(NDEF)
      ->SetDpiScaleType(LONGATTR_DPI_SCALE_GRATETHAN_0);
  s.AddInt(XML_MINWIDTH, m_lMinWidth)
      ->SetDefault(NDEF)
      ->SetDpiScaleType(LONGATTR_DPI_SCALE_GRATETHAN_0);
  s.AddInt(XML_MINHEIGHT, m_lMinHeight)
      ->SetDefault(NDEF)
      ->SetDpiScaleType(LONGATTR_DPI_SCALE_GRATETHAN_0);

  // 布局属性
  if (m_pLayoutParam) {
    m_pLayoutParam->Serialize(pData);
  } else if (pData->IsReload()) {
    CreateLayoutParam();
  }
}

int LayoutObject::GetZorder() { return m_lzOrder; }

void LayoutObject::SetZorderDirect(int z) {
  m_lzOrder = z;

  assert(false);
#if 0 // TODO:
  // bool bOldHasLayer = GetSelfLayer()?true:false;
  bool bPosChanged = false;

  // 更新了zorder，修改自己在parent中的位置
  if (m_pParent) {
    if ((m_pPrev && m_pPrev->m_lzOrder > m_lzOrder) ||
        (m_pNext && m_pNext->m_lzOrder < m_lzOrder)) {
      RemoveMeInTheTree();
      if (IsNcObject())
        m_pParent->AddNcChild(this);
      else
        m_pParent->AddChild(this);

      bPosChanged = true;
    }
  }
#endif

#if 0 // defined(OS_WIN)
  //     update_layer_ptr();
  //
  //     bool bNowHasLayer = GetSelfLayer() ? true : false;
  if (/*bOldHasLayer && bNowHasLayer && */ bPosChanged) {
    m_objLayer.OnObjPosInTreeChanged();
  }
#else
  UIASSERT(false);
#endif
}

int LayoutObject::GetMaxWidth() { return m_nMaxWidth; }

int LayoutObject::GetMaxHeight() { return m_nMaxHeight; }

void LayoutObject::SetMaxWidth(int n) { m_nMaxWidth = n; }
void LayoutObject::SetMaxHeight(int n) { m_nMaxHeight = n; }

int LayoutObject::GetMinWidth() { return m_lMinWidth; }
int LayoutObject::GetMinHeight() { return m_lMinHeight; }
void LayoutObject::SetMinWidth(int n) { m_lMinWidth = n; }
void LayoutObject::SetMinHeight(int n) { m_lMinHeight = n; }


// 自己在树中的位置改变。如在编辑器中，拖拽控件到另一个panel下面
void LayoutObject::position_in_tree_changed() {
  if (!m_pLayoutParam)
    return;

  // 根据父对象，重新生成布局参数
  SAFE_RELEASE(m_pLayoutParam);
  GetSafeLayoutParam();
}

void LayoutObject::SetConfigWidth(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->UUID() == ICanvasLayout::UUID())
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigWidth(n);
  } else {
    // TODO:
    // CanvasLayout::GetObjectLayoutParam(this)->SetConfigWidth(n);
  }
}
void LayoutObject::SetConfigHeight(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->UUID() == ICanvasLayout::UUID())
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigHeight(n);
  } else {
    // TODO:
    // CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigHeight(n);
  }
}

ILayoutParam *LayoutObject::GetLayoutParam() { return m_pLayoutParam; }

ILayout *LayoutObject::GetLayout() {
  Object *obj = m_object.GetParentObject();
  while (obj) {
    GetLayoutMessage msg;
    obj->RouteMessage(&msg);
    if (msg.layout)
      return msg.layout;
    obj = obj->GetParentObject();
  }
  return nullptr;
}

// 如果没有，则创建一个canvas布局
ILayoutParam *LayoutObject::GetSafeLayoutParam() {
  if (m_pLayoutParam)
    return m_pLayoutParam;

  CreateLayoutParam();
  if (m_pLayoutParam)
    return m_pLayoutParam;

  CanvasLayout layout;
  m_pLayoutParam = layout.CreateLayoutParam(m_object.GetIObject());

  SerializeParam data = {0};
  data.pMapAttrib = m_object.GetMapAttribute().get();
  data.pSkinRes = m_object.GetIResource();
  data.nFlags = SERIALIZEFLAG_LOAD;
  m_pLayoutParam->Serialize(&data);

  return m_pLayoutParam;
}

void LayoutObject::CreateLayoutParam() {
  if (m_pLayoutParam)
    return;

  if (!m_object.GetParentObject())
    return;

  ILayout *pLayout = GetLayout();
  if (!pLayout)
    return;

  m_pLayoutParam = pLayout->CreateLayoutParam(m_object.GetIObject());
}

void LayoutObject::DestroyLayoutParam() { SAFE_RELEASE(m_pLayoutParam); }

void LayoutObject::SetLayoutParam(ILayoutParam *p) {
  SAFE_RELEASE(m_pLayoutParam);
  m_pLayoutParam = p;
}


//
//	获取对象自己期望的大小
//
Size LayoutObject::GetDesiredSize() {
  if (!m_pLayoutParam) {
    CreateLayoutParam();

    if (!m_pLayoutParam) // 还是创建不成功，例如Combobox中的Button，它没有父Panel-Layout
    {
      CanvasLayout layout;
      m_pLayoutParam = layout.CreateLayoutParam(m_object.GetIObject());
    }
  }

  return m_pLayoutParam->CalcDesiredSize();
}

void LayoutObject::UpdateLayoutPos() {
  if (m_pLayoutParam) {
    m_pLayoutParam->UpdateByRect();
  }
}


} // namespace ui