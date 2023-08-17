#include "svg_layout.h"
#include "sdk/include/interface/iobject.h"

namespace ui {
namespace svg {

SvgLayoutParam::SvgLayoutParam(ui::IObject *p) { m_obj = p; }

void SvgLayoutParam::Load() {
  IMapAttribute *pMapAttr = m_obj->GetMapAttribute();
  SERIALIZEDATA data = {0};
  data.pUIApplication = m_obj->GetUIApplication();
  data.pSkinRes = m_obj->GetResBundle();
  data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
  data.pMapAttrib = pMapAttr;

  // Serialize(&data);

  // 节省内存
  if (pMapAttr->GetAttrCount() == 0) {
    m_obj->ClearMapAttribute();
  }
}

Uuid SvgLayoutParam::UUID() { return SvgLayout::UUID(); }

SvgLayout::SvgLayout() {}
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

void SvgLayout::Arrange(IObject *pObjToArrage) {
  m_dirty = false;

  // SvgLayoutParam *pParam = GetObjectLayoutParam(pChild);
  // if (!pParam) {
  //   return;
  // }

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
  return new SvgLayoutParam(pObj);
}

void SvgLayout::ChildObjectVisibleChanged(IObject *pObj) {}

void SvgLayout::ChildObjectContentSizeChanged(IObject *pObj) {}

bool SvgLayout::IsDirty() { return m_dirty; }
void SvgLayout::SetDirty(bool b) { m_dirty = b; }

} // namespace svg
} // namespace ui