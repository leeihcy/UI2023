#include "rectregion.h"
#include "include/inc.h"
#include <set>

namespace ui {
RectRegion::RectRegion() {
  memset(m_stackArray, 0, sizeof(m_stackArray));
  m_heapArray = 0;
  m_nCount = 0;
  m_nCapacity = 0;
}
RectRegion::~RectRegion() { Destroy(); }

RectRegion::RectRegion(RectRegion &o) {
  m_heapArray = 0;
  m_nCount = 0;
  m_nCapacity = 0;
  this->CopyFrom(&o);
}

RectRegion &RectRegion::operator=(const RectRegion &o) {
  m_heapArray = 0;
  m_nCount = 0;
  m_nCapacity = 0;
  this->CopyFrom(&o);
  return *this;
}

Rect *RectRegion::GetArrayPtr() {
  if (m_heapArray)
    return m_heapArray;

  return m_stackArray;
}

const Rect *RectRegion::GetArrayPtr2() const {
  if (m_heapArray)
    return m_heapArray;

  return m_stackArray;
}

Rect *RectRegion::GetRectPtrAt(unsigned int nIndex) {
  UIASSERT(nIndex < m_nCount);
  if (nIndex >= m_nCount) {
    return nullptr;
  }

  if (m_heapArray)
    return m_heapArray + nIndex;

  UIASSERT(nIndex < STACK_SIZE);
  if (nIndex >= STACK_SIZE) {
    return nullptr;
  }
  return m_stackArray + nIndex;
}

unsigned int RectRegion::GetCount() const { return m_nCount; }

void RectRegion::Destroy() {
  SAFE_ARRAY_DELETE(m_heapArray);
  memset(m_stackArray, 0, sizeof(m_stackArray));
  m_nCount = 0;
  m_nCapacity = 0;
}

void RectRegion::CopyFrom(const RectRegion *po) {
  if (!po)
    return;

  CopyFromArray(((RectRegion *)po)->GetArrayPtr(), po->m_nCount);
}

void RectRegion::CopyFromArray(const Rect *pArray, unsigned int nCount) {
  Destroy();
  if (nCount > STACK_SIZE) {
    m_heapArray = new Rect[nCount];
    m_nCapacity = nCount;
    memcpy(m_heapArray, pArray, sizeof(Rect) * nCount);
  } else if (nCount) {
    memcpy(m_stackArray, pArray, sizeof(Rect) * nCount);
  }
  m_nCount = nCount;
}

void RectRegion::AddRect(const Rect &rc) {
  if (m_nCount >= STACK_SIZE || m_heapArray) {
    long nCount = m_nCount;
    Rect *pHeap = new Rect[nCount + 1];
    memcpy(pHeap, GetArrayPtr(), m_nCount * sizeof(Rect));
    (pHeap + m_nCount)->CopyFrom(rc);

    Destroy();
    m_heapArray = pHeap;
    m_nCount = nCount + 1;
    m_nCapacity = m_nCount;
  } else {
    m_stackArray[m_nCount].CopyFrom(rc);
    m_nCount++;
  }
}

void RectRegion::SetSize(unsigned int nCount) {
  Destroy();
  m_nCount = nCount;

  if (nCount > STACK_SIZE) {
    m_heapArray = new Rect[nCount];
    m_nCapacity = nCount;
    memset(m_heapArray, 0, sizeof(Rect) * nCount);
  }
}

bool RectRegion::SetAt(unsigned int nIndex, Rect *pValue) {
  if (nIndex >= m_nCount)
    return false;
  if (!pValue)
    return false;

  GetRectPtrAt(nIndex)->CopyFrom(*pValue);
  return true;
}

void RectRegion::Offset(int x, int y) {
  for (unsigned int i = 0; i < m_nCount; i++) {
    GetRectPtrAt(i)->Offset(x, y);
  }
}

// 运行完之后，m_heapArray的大小与m_nCount可能不匹配
bool RectRegion::IntersectRect(const Rect *prc, bool OnlyTest) {
  if (0 == m_nCount)
    return false;

  Rect temp = {0};
  unsigned int nNewCount = 0;

  for (unsigned int i = 0; i < m_nCount; i++) {
    if (GetRectPtrAt(i)->Intersect(*prc, &temp)) {
      if (OnlyTest) {
        return true;
      } else {
        GetRectPtrAt(nNewCount)->CopyFrom(temp);
        nNewCount++;
      }
    }
  }

  if (nNewCount == m_nCount)
    return true;

  if (0 == nNewCount) {
    if (OnlyTest)
      return false;

    Destroy();
    return false;
  }

  if (OnlyTest)
    return true;

  // 清空没用的位置
  for (unsigned int i = nNewCount; i < m_nCount; i++)
    GetRectPtrAt(i)->SetEmpty();

  m_nCount = nNewCount;
  return true;
}

// 简单合并策略，会扩大脏区域范围
void RectRegion::UnionSimple(const Rect& rc)
{
  // 1. 检测有没有重叠项，或者有交集的项
  Rect rcTemp = {0};
  bool has_intersect = false;
  for (unsigned int i = 0; i < m_nCount; i++) {
    Rect *prcTest = GetRectPtrAt(i);

    if (!prcTest->Intersect(rc, &rcTemp))
      continue;

    // 有交集，直接求交
    prcTest->Union(rc, prcTest);
    has_intersect = true;
  }

  // 没有交集，加进来
  if (!has_intersect) {
    this->AddRect(rc);
  }
}

// 场景：窗口脏区域逻辑
void RectRegion::Union(const Rect &rc) {
  if (rc.IsEmpty()) {
    return;
  }
  if (m_nCount == 0) {
    AddRect(rc);
    return;
  }

  // 采用扫描线的方式，从上到下进行扫描

  // 1. 将所有纵坐标进行排序
  std::set<int> y_scanlines;

  std::vector<Rect> all_rects;
  for (unsigned int i = 0; i < m_nCount; i++) {
    Rect *prc = GetRectPtrAt(i);
    y_scanlines.insert(prc->top);
    y_scanlines.insert(prc->bottom);
    all_rects.push_back(*GetRectPtrAt(i));
  }
  y_scanlines.insert(rc.top);
  y_scanlines.insert(rc.bottom);
  all_rects.push_back(rc);

  // 清空数组，后面重新创建。
  this->Destroy();

  auto iter = y_scanlines.begin();
  int y1 = *iter; 
  int y2 = y1;

  // 2. 从上到下扫描
  for (++iter; iter != y_scanlines.end(); ++iter) {
    y1 = y2;
    y2 = *iter;

    std::vector<Rect> horz_rects;
    // 3. 从左到右扫描
    for (Rect &i : all_rects) {
      if (i.bottom <= y1) {
        continue;
      }
      if (i.top >= y2) {
        continue;
      }
      horz_rects.push_back({i.left, y1, i.right, y2});
    }
    add_rects_same_topbottom(horz_rects);
  }
}

// 添加同一水平线的矩形，如果能合并则合并
void RectRegion::add_rects_same_topbottom(std::vector<Rect> &horz_rects) {
  if (horz_rects.empty()) {
    return;
  }
  auto lambda_sort = [](const Rect &r1, const Rect &r2) -> bool {
    return r2.left > r1.left;
  };

  // 从左到右排序
  std::sort(horz_rects.begin(), horz_rects.end(), lambda_sort);

  auto xiter = horz_rects.begin();
  Rect rc_left = *xiter;
  for (++xiter; xiter != horz_rects.end(); ++xiter) {
    Rect &rc_compare = *xiter;
    if (rc_compare.left > rc_left.right) {
      // 两个矩形不相连
      add_rect_and_merge_vert(rc_left);
      rc_left = rc_compare;
      continue;
    }
    if (rc_compare.left == rc_left.right) {
      // 两个矩形是连着的，直接合并成一个
      rc_left.right = rc_compare.right;
      continue;
    }
    if (rc_compare.right <= rc_left.right) {
      // 后面这个矩形被左边的包含着，直接忽略
      continue;
    }
    // 这两个矩形相交了，直接合并成一个
    rc_left.right = rc_compare.right;
    continue;
  }
  add_rect_and_merge_vert(rc_left);
}

// 添加一个矩形。
// 如果这个矩形和已有的矩形上下能完全重合起来，则合并
void RectRegion::add_rect_and_merge_vert(Rect& rc) {
  for (unsigned int i = 0; i < m_nCount; i++) {
    Rect *prc = GetRectPtrAt(i);
    if (prc->bottom != rc.top) {
      // 由于是从上往下扫描的，只需要判断是否和已有的区域bottom一致即可。
      continue;
    }
    if (prc->left == rc.left && prc->right == rc.right) {
      prc->bottom = rc.bottom;
      return;
    }
  }
  AddRect(rc);
}

#if 0 // defined(OS_WIN)
HRGN RectRegion::CreateRgn() {
  if (0 == m_nCount || nullptr == GetArrayPtr()) {
    return nullptr;
  }
  if (1 == m_nCount) {
    return CreateRectRgnIndirect(GetRectPtrAt(0));
  }

  RGNDATA *pRgnData =
      (RGNDATA *)new BYTE[sizeof(RGNDATAHEADER) + m_nCount * sizeof(Rect)];
  memset(pRgnData, 0, sizeof(RGNDATAHEADER));
  pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
  pRgnData->rdh.iType = RDH_RECTANGLES;
  pRgnData->rdh.nCount = m_nCount;
  pRgnData->rdh.nRgnSize = m_nCount * sizeof(Rect);

  Rect *prcBuffer = (Rect *)pRgnData->Buffer;
  for (unsigned int k = 0; k < m_nCount; k++) {
    CopyRect(prcBuffer + k, GetRectPtrAt(k));
  }
  HRGN hRgn = ::ExtCreateRegion(
      nullptr, sizeof(RGNDATAHEADER) + m_nCount * sizeof(Rect), pRgnData);
  delete[] pRgnData;

  return hRgn;
}
#endif

void RectRegion::GetUnionRect(Rect *prc) {
  prc->SetEmpty();
  for (unsigned int i = 0; i < m_nCount; i++) {
    GetRectPtrAt(i)->Union(*prc, prc);
  }
}
} // namespace ui