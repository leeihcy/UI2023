#include "cardlayout.h"
#include "include/inc.h"
#include "src/object/object.h"

using namespace ui;

CardLayout::CardLayout() {}

Size CardLayout::Measure() {
  Size size = {0, 0};

  Object *pChild = nullptr;
  while ((pChild = this->m_pPanel->EnumChildObject(pChild))) {
    Size s = pChild->GetDesiredSize();

    if (size.width < s.width)
      size.width = s.width;
    if (size.height < s.height)
      size.height = s.height;
  }
  return size;
}
void CardLayout::DoArrange(ArrangeParam *param) {
  IObject *pIObjToArrage = param ? param->obj_to_arrange : nullptr;

  // 调用该函数时，自己的大小已经被求出来了
  Rect rcClient;
  m_pPanel->GetClientRectInObject(&rcClient);
  int nWidth = rcClient.Width();   // m_pPanel->GetWidth();
  int nHeight = rcClient.Height(); // m_pPanel->GetHeight();

  Object *pObjToArrage = nullptr;
  if (pIObjToArrage)
    pObjToArrage = pIObjToArrage->GetImpl();

  Object *pChild = nullptr;
  while ((pChild = m_pPanel->EnumChildObject(pChild))) {
    if (pObjToArrage && pObjToArrage != pChild)
      continue;

    Rect rcChildObj;
    int nChildW = nWidth;  // - pChild->GetMarginW();
    int nChildH = nHeight; // - pChild->GetMarginH();
    rcChildObj.Set(0, 0, nChildW, nChildH);

    SetPositionFlags flags;
    flags.update_layout_pos = false;
    flags.force_send_size_message = true;
    pChild->SetObjectPos(&rcChildObj, flags);

    if (pObjToArrage && pObjToArrage == pChild)
      break;
  }
}

// 一个对象显示后，将其它对象隐藏掉
// void CardLayout::ChildObjectVisibleChanged(IObject *pIObj) {
//   UIASSERT(pIObj);
//   UIASSERT(pIObj->GetParentObject());
//   UIASSERT(pIObj->GetParentObject()->GetImpl() == m_pPanel);

//   Object *pObj = pIObj->GetImpl();
//   if (!pObj->IsSelfVisible())
//     return;

//   Object *pChild = nullptr;
//   while ((pChild = m_pPanel->EnumChildObject(pChild))) {
//     if (pChild != pObj && pChild->IsSelfVisible()) {
//       pChild->SetVisible(false);
//     }
//   }

//   // SetDirty(true);
//   m_pPanel->Invalidate();
// }