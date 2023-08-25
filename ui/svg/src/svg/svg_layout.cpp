#include "svg_layout.h"
#include "sdk/include/interface/iattribute.h"
#include "sdk/include/interface/iobject.h"

namespace ui {
namespace svg {

SvgLayoutParam::SvgLayoutParam(ui::IObject *p) { m_obj = p; }

void SvgLayoutParam::Load() {

  IMapAttribute *pMapAttr = m_obj->GetMapAttribute();
  SerializeParam data = {0};
  data.pUIApplication = m_obj->GetUIApplication();
  data.pSkinRes = m_obj->GetResource();
  data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
  data.pMapAttrib = pMapAttr;

  Serialize(&data);

  // 节省内存
  if (pMapAttr->GetAttrCount() == 0) {
    m_obj->ClearMapAttribute();
  }
}

Uuid SvgLayoutParam::UUID() { return SvgLayout::UUID(); }

void SvgLayoutParam::Serialize(SerializeParam *pData) {
  AttributeSerializerWrap s(pData, L"SvgLayoutParam");
  s.AddInt(L"x", m_x);
  s.AddInt(L"y", m_y);
  s.AddInt(L"width", m_width);
  s.AddInt(L"height", m_height);
}

SvgLayout::SvgLayout(IObject *bind_object) { m_bind_object = bind_object; }
SvgLayout::~SvgLayout() {}

// 自己在布局的时候调用
SvgLayoutParam *GetObjectLayoutParam(IObject *pObj) {
  if (!pObj)
    return nullptr;

  ILayoutParam *pParam = pObj->GetLayoutParam();
  if (pParam && pParam->UUID() == SvgLayout::UUID()) {
    return static_cast<SvgLayoutParam *>(pParam);
  }

  SvgLayoutParam *param = new SvgLayoutParam(pObj);
  param->Load();
  pObj->SetLayoutParam(param);

  return static_cast<SvgLayoutParam *>(param);
}

void SvgLayout::Arrange(ArrangeParam *param) {
  m_dirty = false;

  IObject *obj_to_arrange = param ? param->obj_to_arrange : nullptr;

  ui::IObject *child = nullptr;
  while ((child = m_bind_object->EnumChildObject(child))) {
    if (obj_to_arrange && obj_to_arrange != child)
      continue;

    SvgLayoutParam *pParam = GetObjectLayoutParam(child);
    if (!pParam) {
      return;
    }

    Rect rect;
    rect.left = pParam->m_x;
    rect.top = pParam->m_y;
    rect.right = rect.left + pParam->m_width;
    rect.bottom = rect.top + pParam->m_height;

    child->SetObjectPos(&rect, SWP_NOREDRAW | SWP_NOUPDATELAYOUTPOS |
                                    SWP_FORCESENDSIZEMSG);
  }

  // Size s = pParam->CalcDesiredSize();

  // if (s.width < 0)
  //   s.width = 0;
  // if (s.height < 0)
  //   s.height = 0;

  // Rect rcChildObj;
  // int nChildW = s.width;  // - pChild->GetMarginW();
  // int nChildH = s.height; // - pChild->GetMarginH();
  // rcChildObj.Set(0, 0, nChildW, nChildH);

  // rcChildObj.Offset(x, y);
  // pChild->SetObjectPos(&rcChildObj, SWP_NOREDRAW | SWP_NOUPDATELAYOUTPOS |
  //                                       SWP_FORCESENDSIZEMSG);
}

ILayoutParam *SvgLayout::CreateLayoutParam(IObject *pObj) {
  auto* p = new SvgLayoutParam(pObj);
  p->Load();
  return p;
}

bool SvgLayout::IsDirty() { return m_dirty; }
void SvgLayout::SetDirty(bool b) { m_dirty = b; }

} // namespace svg
} // namespace ui