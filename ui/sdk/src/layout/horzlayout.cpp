#include "horzlayout.h"
#include "include/inc.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/flags_attribute.h"
#include "src/object/object.h"
#include "src/util/DPI/dpihelper.h"

namespace ui {

HorzLayout::HorzLayout() {
  m_nSpace = 0;
  //	this->m_eDirection = LAYOUT_Horz_LEFTTORIGHT;   // 默认堆栈是从左到右
}
HorzLayout::~HorzLayout() {}
void HorzLayout::Serialize(SerializeParam *pData) {
  //     AttributeSerializer s(pData, "HorzLayout");
  //
  //     s.AddInt(XML_LAYOUT_HORZ_GAP, this,
  //         memfun_cast<pfnLongSetter>(&HorzLayout::LoadGap),
  //         memfun_cast<pfnLongGetter>(&HorzLayout::SaveGap));
  //
}

void HorzLayout::LoadGap(int n) { m_nSpace = ScaleByDpi_if_gt0(n); }
int HorzLayout::SaveGap() { return RestoreByDpi_if_gt0(m_nSpace); }

void HorzLayout::SetSpace(int n) { m_nSpace = n; }

Size HorzLayout::Measure() {
  Size s = {0};

  int nMaxHeight = 0;

  Object *pChild = nullptr;
  while ((pChild = m_pPanel->EnumChildObject(pChild))) {
    // 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
    HorzLayoutParam *pParam = GetObjectLayoutParam(pChild);
    if (!pParam)
      continue;

    if (pChild->IsSelfCollapsed())
      continue;

    if (pParam->m_eWidthType == WH_AUTO) {
      s.width += pChild->GetDesiredSize().width;
    } else {
      s.width += pParam->m_nConfigWidth;
    }

    Rect rMargin = {0};
    pChild->GetMarginRegion(&rMargin);

    s.width += rMargin.left + rMargin.right;

    int nHeight = rMargin.top + rMargin.bottom;

    if (pParam->m_eHeightType == WH_AUTO) {
      nHeight += pChild->GetDesiredSize().height;
    } else {
      nHeight += pParam->m_nConfigHeight;
    }

    nMaxHeight = std::max(nMaxHeight, nHeight);
  }
  s.height = nMaxHeight;

  return s;
}

// 布局顺序：
// 1. 计算出所有已知和期望的大小
// 2. 计算出剩余宽度
// 3. 计算出布局顺序

struct ObjLayoutInfo {
  Object *pObj;
  int width;  // 不包含margin
  int height; // 不包含margin
};

void HorzLayout::DoArrange(ArrangeParam& param) {
  // 计算每个子控件需要的宽度及其布局顺序
  int nChildCount = m_pPanel->GetChildCount();
  if (nChildCount <= 0)
    return;

  // 父控件内间距
  Rect rcPadding = {0};
  m_pPanel->GetPaddingRegion(&rcPadding);

  Rect rcParent;
  m_pPanel->GetObjectClientRect(&rcParent);

  // 计算每个子控件需要的宽度及其布局顺序
  std::vector<ObjLayoutInfo> vecInfo(nChildCount);
  memset(&vecInfo[0], 0, sizeof(ObjLayoutInfo) * nChildCount);

  int nLeftCursor = 0; // 用于指定这个控件的布局顺序，居右对齐的得反着排序
  int nRightCursor = nChildCount - 1;

  int nNeedWidth = 0;
  int nAvgCount = 0;

  Object *pChild = nullptr;
  while ((pChild = m_pPanel->EnumChildObject(pChild))) {
    // 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
    HorzLayoutParam *pParam = GetObjectLayoutParam(pChild);
    if (!pParam)
      continue;

    if (pChild->IsSelfCollapsed())
      continue;

    //  计算布局顺序
    int nIndex = nLeftCursor;
    if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_RIGHT) {
      nIndex = nRightCursor;
      nRightCursor--;
    } else {
      nIndex = nLeftCursor;
      nLeftCursor++;
    }

    // 计算所需要的宽高
    int nObjWidth = 0;
    int nObjHeight = 0;

    if (pParam->IsSizedByContent()) {
      Size s = pChild->GetDesiredSize();

      nObjWidth = s.width;   // - pChild->GetMarginW();
      nObjHeight = s.height; // - pChild->GetMarginH();
    }

    if (pParam->m_eWidthType == WH_AVG) {
      // 本次循环结束后再计算
      // 平均值中，不包含margin
      nAvgCount++;
      nObjWidth = 0;
    } else if (pParam->m_eWidthType == WH_PERCENT) {

#if 0 // 比例中包含margin
            nObjWidth = rcParent.Width() * pParam->m_nConfigWidth / 100;
            nObjWidth -= pChild->GetMarginW();

#else // 比例中不包含margin
      nObjWidth = rcParent.Width() * pParam->m_nConfigWidth / 100;
#endif
    } else if (pParam->m_eWidthType != WH_AUTO) {
      nObjWidth = pParam->m_nConfigWidth;
    }

    if (pParam->m_eHeightType == WH_SET) {
      nObjHeight = pParam->m_nConfigHeight;
    } else if (pParam->m_eHeightType == WH_PERCENT) {
      nObjHeight = rcParent.Height() * pParam->m_nConfigHeight / 100;
    }

    vecInfo[nIndex].pObj = pChild;
    vecInfo[nIndex].width = nObjWidth;
    vecInfo[nIndex].height = nObjHeight;

    nNeedWidth += nObjWidth + pChild->GetMarginW();
  }

  // 计算平均宽度
  int nAvgWidth = 0;
  int nAvgDiff = 0; // 误差补足
  if (nAvgCount > 0) {
    int nTotal = (rcParent.Width() - nNeedWidth);
    nAvgWidth = (nTotal / nAvgCount);
    nAvgDiff = nTotal - (nAvgWidth * nAvgCount);
  }

  // 开始布局
  int nLeftConsume = 0;
  int nRightConsume = 0;
  for (int i = 0; i < nChildCount; i++) {
    ObjLayoutInfo &info = vecInfo[i];
    pChild = info.pObj;
    if (!pChild)
      continue;

    // 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
    HorzLayoutParam *pParam = GetObjectLayoutParam(pChild);
    if (!pParam)
      continue;

    if (pChild->IsSelfCollapsed())
      continue;

    Rect rMargin = {0};
    pChild->GetMarginRegion(&rMargin);

    Rect rcObj;

    // 如果是平均宽度，为其宽度赋值
    if (pParam->m_eWidthType == WH_AVG) {
      info.width = nAvgWidth + nAvgDiff;
      nAvgDiff = 0;
    }

    // 计算x坐标

    if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_RIGHT) {
      nRightConsume += rMargin.right;

      rcObj.right = rcParent.right - nRightConsume;
      rcObj.left = rcObj.right - info.width;

      nRightConsume += info.width + rMargin.left;
    } else // if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_LEFT)
    {
      nLeftConsume += rMargin.left;

      rcObj.left = rcParent.left + nLeftConsume;
      rcObj.right = rcObj.left + info.width;

      nLeftConsume += info.width + rMargin.right;
    }

    // 计算y坐标
    if ((pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_TOP) &&
        (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM)) {
      rcObj.top = rcParent.top + rMargin.top;
      rcObj.bottom = rcParent.bottom - rMargin.bottom;
    } else if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM) {
      rcObj.bottom = rcParent.bottom - rMargin.bottom;
      rcObj.top = rcObj.bottom - info.height;
    } else if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_VCENTER) {
      rcObj.top = rcParent.top + (rcParent.Height() - info.height) / 2;
      rcObj.top = rcObj.top + rMargin.top - rMargin.bottom;
      rcObj.bottom = rcObj.top + info.height;
    } else // LAYOUT_ITEM_ALIGN_TOP:
    {
      rcObj.top = rcParent.top + rMargin.top;
      rcObj.bottom = rcObj.top + info.height;
    }

    SetPositionFlags flags;
    flags.update_layout_pos = false;
    flags.force_send_size_message = true;
    pChild->SetObjectPos(&rcObj, flags);
  }
}

// void HorzLayout::ChildObjectVisibleChanged(IObject *pObj) {
//   UIASSERT(pObj);
//   UIASSERT(pObj->GetParentObject());
//   UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

//   SetDirty(true);
//   m_pPanel->Invalidate();
// }

// void HorzLayout::ChildObjectContentSizeChanged(IObject *pObj) {
//   UIASSERT(pObj);
//   UIASSERT(pObj->GetParentObject());
//   UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

//   SetDirty(true);
//   m_pPanel->Invalidate();
// }

HorzLayoutParam::HorzLayoutParam(Object *pObj) {
  m_nConfigWidth = AUTO;
  m_nConfigHeight = AUTO;
  m_nConfigLayoutFlags = 0;
  m_eWidthType = WH_SET;
  m_eHeightType = WH_SET;
  m_pObj = pObj;
}

HorzLayoutParam::~HorzLayoutParam() {}

void HorzLayoutParam::UpdateByRect() {
  Rect rcParent;
  m_pObj->GetParentRect(&rcParent);

  if (m_eWidthType == WH_SET) {
    m_nConfigWidth = rcParent.Width();
  }
  if (m_eHeightType == WH_SET) {
    m_nConfigHeight = rcParent.Height();
  }
}
void HorzLayoutParam::Serialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "HorzLayoutParam");

  s.AddString(XML_WIDTH, Slot(&HorzLayoutParam::LoadConfigWidth, this),
              Slot(&HorzLayoutParam::SaveConfigWidth, this))
      ->SetDefault(XML_AUTO)
      ->SetCompatibleKey(XML_LAYOUT_PREFIX XML_WIDTH);

  s.AddString(XML_HEIGHT, Slot(&HorzLayoutParam::LoadConfigHeight, this),
              Slot(&HorzLayoutParam::SaveConfigHeight, this))
      ->SetDefault(XML_AUTO)
      ->SetCompatibleKey(XML_LAYOUT_PREFIX XML_HEIGHT);
  ;

  s.AddFlags(XML_LAYOUT_ITEM_ALIGN, m_nConfigLayoutFlags)
      ->AddFlag(LAYOUT_ITEM_ALIGN_LEFT, XML_LAYOUT_ITEM_ALIGN_LEFT)
      ->AddFlag(LAYOUT_ITEM_ALIGN_RIGHT, XML_LAYOUT_ITEM_ALIGN_RIGHT)
      ->AddFlag(LAYOUT_ITEM_ALIGN_TOP, XML_LAYOUT_ITEM_ALIGN_TOP)
      ->AddFlag(LAYOUT_ITEM_ALIGN_BOTTOM, XML_LAYOUT_ITEM_ALIGN_BOTTOM)
      ->AddFlag(LAYOUT_ITEM_ALIGN_CENTER, XML_LAYOUT_ITEM_ALIGN_CENTER)
      ->AddFlag(LAYOUT_ITEM_ALIGN_VCENTER, XML_LAYOUT_ITEM_ALIGN_VCENTER)
      ->AddFlag(LAYOUT_ITEM_ALIGN_FILL, XML_LAYOUT_ITEM_ALIGN_FILL);
}

int HorzLayoutParam::GetConfigWidth() { return m_nConfigWidth; }
void HorzLayoutParam::SetConfigWidth(int n) { m_nConfigWidth = n; }

void HorzLayoutParam::LoadConfigWidth(const char *szText) {
  LoadConfigWH(szText, m_nConfigWidth, m_eWidthType);
}

const char *HorzLayoutParam::SaveConfigWidth() {
  return SaveConfigWH(m_nConfigWidth, m_eWidthType);
}

int HorzLayoutParam::GetConfigHeight() { return m_nConfigHeight; }
void HorzLayoutParam::SetConfigHeight(int n) { m_nConfigHeight = n; }
void HorzLayoutParam::LoadConfigHeight(const char *szText) {
  LoadConfigWH(szText, m_nConfigHeight, m_eHeightType);
}
const char *HorzLayoutParam::SaveConfigHeight() {
  return SaveConfigWH(m_nConfigHeight, m_eHeightType);
}

void HorzLayoutParam::SetConfigLayoutFlags(int n) { m_nConfigLayoutFlags = n; }
int HorzLayoutParam::GetConfigLayoutFlags() { return m_nConfigLayoutFlags; }

Size HorzLayoutParam::CalcDesiredSize() {
  Size size = {0, 0};

  if (IsSizedByContent()) {
    // 获取子对象所需要的空间
    GetDesiredSizeMessage msg;
    m_pObj->RouteMessage(&msg);
    size = msg.size;

    // 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
    if (this->m_eWidthType != AUTO)
      size.width = this->m_nConfigWidth;
    if (this->m_eHeightType != AUTO)
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

bool HorzLayoutParam::IsSizedByContent() {
  if (m_eWidthType != AUTO && m_eHeightType != AUTO)
    return false;

  return true;
}
} // namespace ui