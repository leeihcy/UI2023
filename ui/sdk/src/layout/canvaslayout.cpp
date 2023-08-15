#include "canvaslayout.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/flags_attribute.h"
#include "src/attribute/long_attribute.h"
#include "src/object/object.h"
#include "src/util/DPI/dpihelper.h"

namespace ui {

Size CanvasLayout::Measure() {
  Size size = {0, 0};

  // 通过子控件来获取自己所需要的大小
  Object *pChild = nullptr;
  while ((pChild = this->m_pPanel->EnumChildObject(pChild))) {
    CanvasLayoutParam *pParam = GetObjectLayoutParam(pChild);
    if (!pParam) {
      continue;
    }
    Size s = pChild->GetDesiredSize();
    s.width += pChild->GetMarginW();
    s.height += pChild->GetMarginH();

    if (NDEF != pParam->GetConfigLeft()) {
      s.width += pParam->GetConfigLeft();
    }
    if (NDEF != pParam->GetConfigRight()) {
      s.width += pParam->GetConfigRight();
    }
    if (NDEF != pParam->GetConfigTop()) {
      s.height += pParam->GetConfigTop();
    }
    if (NDEF != pParam->GetConfigBottom()) {
      s.height += pParam->GetConfigBottom();
    }

    if (size.width < s.width) {
      size.width = s.width;
    }
    if (size.height < s.height) {
      size.height = s.height;
    }
  }
  return size;
}
void CanvasLayout::DoArrage(IObject *pIObjToArrage) {
  Object *pObjToArrage = nullptr;
  if (pIObjToArrage)
    pObjToArrage = pIObjToArrage->GetImpl();

  // 调用该函数时，自己的大小已经被求出来了
  Rect rcClient;
  m_pPanel->GetClientRectInObject(&rcClient);
  int nWidth = rcClient.Width();   // m_pPanel->GetWidth();
  int nHeight = rcClient.Height(); // m_pPanel->GetHeight();

  Object *pChild = nullptr;
  while ((pChild = m_pPanel->EnumChildObject(pChild))) {
    if (pObjToArrage && pObjToArrage != pChild)
      continue;

    if (pChild->IsSelfCollapsed()) {
      // 隐藏的控件，在编辑器中也需要加载布局属性。
      if (m_pPanel->GetUIApplication()->IsEditorMode())
        GetObjectLayoutParam(pChild);

      continue;
    }

    this->ArrangeObject(pChild, nWidth, nHeight);

    if (pObjToArrage && pObjToArrage == pChild)
      break;
  }
}

void CanvasLayout::ArrangeObject(Object *pChild, const int &nWidth,
                                 const int &nHeight) {
  int x = 0, y = 0; // pChild最终在parent中的坐标

  CanvasLayoutParam *pParam = GetObjectLayoutParam(pChild);
  if (!pParam) {
    return;
  }

  //////////////////////////////////////////////////////////////////////////
  // 计算出 pChild 的 rectP的宽和高
  Size s = pParam->CalcDesiredSize();

  // 先CalcDesiredSize，再getconfigleft,防止在CalcDesiredSize中修改config left
  int left = pParam->GetConfigLeft();
  int top = pParam->GetConfigTop();
  int right = pParam->GetConfigRight();
  int bottom = pParam->GetConfigBottom();
  // int nConfigWidth = pParam->GetConfigWidth();
  // int nConfigHeight = pParam->GetConfigHeight();
  int nConfigFlag = pParam->GetConfigLayoutFlags();

  // 如果同时指定了left/right,则忽略width属性
  if (0 == (nConfigFlag & LAYOUT_ITEM_ALIGN_CENTER)) {
    if (left != NDEF && right != NDEF) {
      s.width = nWidth - left - right;
    }
  }
  // 如果同时指定了top/bottom，则忽略height属性
  if (0 == (nConfigFlag & LAYOUT_ITEM_ALIGN_VCENTER)) {
    if (top != NDEF && bottom != NDEF) {
      s.height = nHeight - top - bottom;
    }
  }
  if (s.width < 0)
    s.width = 0;
  if (s.height < 0)
    s.height = 0;

  Rect rcChildObj;
  int nChildW = s.width;  // - pChild->GetMarginW();
  int nChildH = s.height; // - pChild->GetMarginH();
  rcChildObj.Set(0, 0, nChildW, nChildH);

  // 计算出坐标，若left/right,top/bottom中有一个未指定的，那么取0（但在DesktopLayout中是取居中）
  if (nConfigFlag & LAYOUT_ITEM_ALIGN_CENTER) {
    x = (nWidth - s.width) / 2; // 居中
    if (left != NDEF)
      x -= left;
    if (right != NDEF)
      x += right;
  } else {
    if (left != NDEF) {
      x = left;
      x += pChild->GetMarginL();
    } else {
      if (right != NDEF) {
        x = nWidth - right - nChildW;
        x -= pChild->GetMarginR();
      } else {
        x = 0;
        x += pChild->GetMarginL();
      }
    }
  }

  if (nConfigFlag & LAYOUT_ITEM_ALIGN_VCENTER) {
    y = (nHeight - s.height) / 2; // 居中
    if (top != NDEF)
      y -= top;
    if (bottom != NDEF)
      y += bottom;
  } else {
    if (top != NDEF) {
      y = top;
      y += pChild->GetMarginT();
    } else {
      if (bottom != NDEF) {
        y = nHeight - bottom - nChildH;
        y -= pChild->GetMarginB();
      } else {
        y = 0;
        y += pChild->GetMarginT();
      }
    }
  }

  rcChildObj.Offset(x, y);
  pChild->SetObjectPos(&rcChildObj, SWP_NOREDRAW | SWP_NOUPDATELAYOUTPOS |
                                        SWP_FORCESENDSIZEMSG);
}

void CanvasLayout::ChildObjectVisibleChanged(IObject *pObj) {
  UIASSERT(pObj);
  UIASSERT(pObj->GetParentObject());
  UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

  // TODO: 优化为只布局该pObj，并且仅在collapsed的情况下需要布局
  // SetDirty(true);<-- 不能仅调该函数，可能后面没有触发invalidate
  DoArrage(pObj);

  Rect rc = {0};
  pObj->GetParentRect(&rc);
  m_pPanel->Invalidate(&rc);

  // pObj隐藏了，无法刷新，通过父对象来刷新
  // pObj->Invalidate();
}

void CanvasLayout::ChildObjectContentSizeChanged(IObject *pObj) {
  UIASSERT(pObj);
  UIASSERT(pObj->GetParentObject());
  UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

  // TODO: 优化为只布局该pObj，并且仅在collapsed的情况下需要布局
  // SetDirty(true);<-- 不能仅调该函数，可能后面没有触发invalidate
  DoArrage(pObj);

  pObj->Invalidate();
}

//////////////////////////////////////////////////////////////////////////

CanvasLayoutParam::CanvasLayoutParam(Object *pObj) {
  m_pObj = pObj;
  
  m_nConfigWidth = m_nConfigHeight = AUTO;
  m_nConfigLeft = m_nConfigRight = m_nConfigTop = m_nConfigBottom = NDEF;
  m_nConfigLayoutFlags = 0;
}
CanvasLayoutParam::~CanvasLayoutParam() {}

Size CanvasLayoutParam::CalcDesiredSize() {
  Size size = {0, 0};

  if (IsSizedByContent()) {
    // 获取子对象所需要的空间
    m_pObj->SendMessage(UI_MSG_GETDESIREDSIZE, (long)&size);

    // 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
    if (this->m_nConfigWidth != AUTO)
      size.width = this->m_nConfigWidth;
    if (this->m_nConfigHeight != AUTO)
      size.height = this->m_nConfigHeight;
  } else {
    size.width = this->m_nConfigWidth;
    size.height = this->m_nConfigHeight;
  }

  // 计算 margin 的大小
  //     size.width += m_pObj->GetMarginW();
  //     size.height += m_pObj->GetMarginH();

  return size;
}

void CanvasLayoutParam::UpdateByRect() {
  Object *pParent = m_pObj->GetParentObject();
  if (!pParent)
    return;

  Rect rcParent;
  Rect rcPanel;
  m_pObj->GetParentRect(&rcParent);
  pParent->GetClientRectInObject(&rcPanel);

  bool bSetX = false;
  if (m_nConfigLeft >= 0) {
    bSetX = true;
    m_nConfigLeft = rcParent.left;
  }
  if (m_nConfigRight >= 0) {
    bSetX = true;
    m_nConfigRight = rcPanel.Width() - rcParent.right;
  }
  if (!bSetX) {
    m_nConfigLeft = rcParent.left; // 当left/right都没有指定时，强制设置一个
  }

  bool bSetY = false;
  if (m_nConfigTop >= 0) {
    bSetY = true;
    m_nConfigTop = rcParent.top;
  }
  if (m_nConfigBottom >= 0) {
    bSetY = true;
    m_nConfigBottom = rcPanel.Height() - rcParent.bottom;
  }
  if (!bSetY) {
    m_nConfigTop = rcParent.top; // 当top/bottom都没有指定时，强制设置一个
  }

  if (m_nConfigWidth >= 0) {
    m_nConfigWidth = rcParent.Width();
  }
  if (m_nConfigHeight >= 0) {
    m_nConfigHeight = rcParent.Height();
  }
}
void CanvasLayoutParam::Serialize(SERIALIZEDATA *pData) {
  AttributeSerializer s(pData, TEXT("CanvasLayoutParam"));

  // width=""，也支持 layout.width=""
  s.AddLong(XML_WIDTH, Slot(&CanvasLayoutParam::LoadConfigWidth, this),
            Slot(&CanvasLayoutParam::SaveConfigWidth, this))
      ->AddAlias(AUTO, XML_AUTO)
      ->AddAlias(NDEF, XML_NDEF)
      ->SetDefault(AUTO)
      ->SetCompatibleKey(XML_LAYOUT_PREFIX XML_WIDTH);

  s.AddLong(XML_HEIGHT, Slot(&CanvasLayoutParam::LoadConfigHeight, this),
            Slot(&CanvasLayoutParam::SaveConfigHeight, this))
      ->AddAlias(AUTO, XML_AUTO)
      ->AddAlias(NDEF, XML_NDEF)
      ->SetDefault(AUTO)
      ->SetCompatibleKey(XML_LAYOUT_PREFIX XML_HEIGHT);

  s.AddLong(XML_LAYOUT_ITEM_LEFT,
            Slot(&CanvasLayoutParam::LoadConfigLeft, this),
            Slot(&CanvasLayoutParam::SaveConfigLeft, this))
      ->AddAlias(NDEF, XML_NDEF)
      ->SetDefault(NDEF);

  s.AddLong(XML_LAYOUT_ITEM_TOP, Slot(&CanvasLayoutParam::LoadConfigTop, this),
            Slot(&CanvasLayoutParam::SaveConfigTop, this))
      ->AddAlias(NDEF, XML_NDEF)
      ->SetDefault(NDEF);

  s.AddLong(XML_LAYOUT_ITEM_RIGHT,
            Slot(&CanvasLayoutParam::LoadConfigRight, this),
            Slot(&CanvasLayoutParam::SaveConfigRight, this))
      ->AddAlias(NDEF, XML_NDEF)
      ->SetDefault(NDEF);

  s.AddLong(XML_LAYOUT_ITEM_BOTTOM,
            Slot(&CanvasLayoutParam::LoadConfigBottom, this),
            Slot(&CanvasLayoutParam::SaveConfigBottom, this))
      ->AddAlias(NDEF, XML_NDEF)
      ->SetDefault(NDEF);

  s.AddFlags(XML_LAYOUT_ITEM_ALIGN, m_nConfigLayoutFlags)
      ->AddFlag(LAYOUT_ITEM_ALIGN_LEFT, XML_LAYOUT_ITEM_ALIGN_LEFT)
      ->AddFlag(LAYOUT_ITEM_ALIGN_RIGHT, XML_LAYOUT_ITEM_ALIGN_RIGHT)
      ->AddFlag(LAYOUT_ITEM_ALIGN_TOP, XML_LAYOUT_ITEM_ALIGN_TOP)
      ->AddFlag(LAYOUT_ITEM_ALIGN_BOTTOM, XML_LAYOUT_ITEM_ALIGN_BOTTOM)
      ->AddFlag(LAYOUT_ITEM_ALIGN_CENTER, XML_LAYOUT_ITEM_ALIGN_CENTER)
      ->AddFlag(LAYOUT_ITEM_ALIGN_VCENTER, XML_LAYOUT_ITEM_ALIGN_VCENTER)
      /*->AddFlag(LAYOUT_FLAG_FORCE_DESIREDSIZE,
         XML_LAYOUT_ALIGN_FORCE_DESIREDSIZE)*/
      ;
}

long CanvasLayoutParam::GetConfigLeft() { return m_nConfigLeft; }
void CanvasLayoutParam::LoadConfigLeft(long n) {
  m_nConfigLeft = ScaleByDpi_if_gt0(n);
}
long CanvasLayoutParam::SaveConfigLeft() {
  return RestoreByDpi_if_gt0(m_nConfigLeft);
}
void CanvasLayoutParam::SetConfigLeft(long n) { m_nConfigLeft = n; }

long CanvasLayoutParam::GetConfigTop() { return m_nConfigTop; }
void CanvasLayoutParam::LoadConfigTop(long n) {
  m_nConfigTop = ScaleByDpi_if_gt0(n);
}
long CanvasLayoutParam::SaveConfigTop() {
  return RestoreByDpi_if_gt0(m_nConfigTop);
}
void CanvasLayoutParam::SetConfigTop(long n) { m_nConfigTop = n; }

long CanvasLayoutParam::GetConfigRight() { return m_nConfigRight; }
void CanvasLayoutParam::LoadConfigRight(long n) {
  m_nConfigRight = ScaleByDpi_if_gt0(n);
}
long CanvasLayoutParam::SaveConfigRight() {
  return RestoreByDpi_if_gt0(m_nConfigRight);
}
void CanvasLayoutParam::SetConfigRight(long n) { m_nConfigRight = n; }

long CanvasLayoutParam::GetConfigBottom() { return m_nConfigBottom; }
void CanvasLayoutParam::LoadConfigBottom(long n) {
  m_nConfigBottom = ScaleByDpi_if_gt0(n);
}
long CanvasLayoutParam::SaveConfigBottom() {
  return RestoreByDpi_if_gt0(m_nConfigBottom);
}
void CanvasLayoutParam::SetConfigBottom(long n) { m_nConfigBottom = n; }

long CanvasLayoutParam::GetConfigWidth() { return m_nConfigWidth; }
void CanvasLayoutParam::SetConfigWidth(long n) { m_nConfigWidth = n; }
void CanvasLayoutParam::LoadConfigWidth(long n) {
  m_nConfigWidth = ScaleByDpi_if_gt0(n);
}
long CanvasLayoutParam::SaveConfigWidth() {
  return RestoreByDpi_if_gt0(m_nConfigWidth);
}

long CanvasLayoutParam::GetConfigHeight() { return m_nConfigHeight; }
void CanvasLayoutParam::SetConfigHeight(long n) { m_nConfigHeight = n; }
void CanvasLayoutParam::LoadConfigHeight(long n) {
  m_nConfigHeight = ScaleByDpi_if_gt0(n);
}
long CanvasLayoutParam::SaveConfigHeight() {
  return RestoreByDpi_if_gt0(m_nConfigHeight);
}

void CanvasLayoutParam::SetConfigLayoutFlags(long n) {
  m_nConfigLayoutFlags = n;
}
long CanvasLayoutParam::GetConfigLayoutFlags() { return m_nConfigLayoutFlags; }

int CanvasLayoutParam::ParseAlignAttr(const wchar_t *szAttr) {
  util::ISplitStringEnum *pEnum = nullptr;
  int nCount = util::SplitString(szAttr, XML_FLAG_SEPARATOR, &pEnum);

  int nRet = 0;
  for (int i = 0; i < nCount; i++) {
    const wchar_t *szFlag = pEnum->GetText(i);
    if (0 == wcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_LEFT))
      nRet |= LAYOUT_ITEM_ALIGN_LEFT;
    else if (0 == wcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_RIGHT))
      nRet |= LAYOUT_ITEM_ALIGN_RIGHT;
    else if (0 == wcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_TOP))
      nRet |= LAYOUT_ITEM_ALIGN_TOP;
    else if (0 == wcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_BOTTOM))
      nRet |= LAYOUT_ITEM_ALIGN_BOTTOM;
    else if (0 == wcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_CENTER))
      nRet |= LAYOUT_ITEM_ALIGN_CENTER;
    else if (0 == wcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_VCENTER))
      nRet |= LAYOUT_ITEM_ALIGN_VCENTER;
    // 		else if (0 == wcscmp(szFlag,
    // XML_LAYOUT_ALIGN_FORCE_DESIREDSIZE)) 			nRet |=
    // LAYOUT_FLAG_FORCE_DESIREDSIZE;
  }
  SAFE_RELEASE(pEnum);

  return nRet;
}

bool CanvasLayoutParam::IsSizedByContent() {
  bool bWidthNotConfiged = m_nConfigWidth == AUTO;
  bool bHeightNotConfiged = m_nConfigHeight == AUTO;

  // 	if (bWidthNotConfiged)
  // 	{
  // 		if (m_nConfigLeft != NDEF && m_nConfigRight != NDEF)
  // 			bWidthNotConfiged = false;
  // 	}
  //
  // 	if (bHeightNotConfiged)
  // 	{
  // 		if (m_nConfigTop != NDEF && m_nConfigBottom != NDEF)
  // 			bHeightNotConfiged = false;
  // 	}

  if (bWidthNotConfiged || bHeightNotConfiged)
    return true;

  //     if (m_nConfigLayoutFlags & LAYOUT_FLAG_FORCE_DESIREDSIZE)
  //         return true;

  return false;
}

} // namespace ui