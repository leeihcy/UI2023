#pragma once
#include "include/util/rect.h"
#include <vector>

namespace ui {
class RectRegion {
public:
  RectRegion();
  ~RectRegion();
  RectRegion(RectRegion &o);
  RectRegion &operator=(const RectRegion &o);

  RECT *GetArrayPtr();
  const RECT *GetArrayPtr2() const;
  RECT *GetRectPtrAt(unsigned int nIndex);
  unsigned int GetCount() const;
  void AddRect(const RECT&);

  void Destroy();
  void CopyFrom(const RectRegion *po);
  void CopyFromArray(const RECT *pArray, unsigned int nCount);
  void SetSize(unsigned int nCount);
  bool SetAt(unsigned int nIndex, RECT *pValue);
  void Offset(int x, int y);

  // 添加一个脏区域，合并到现有的数组当中。
  void Union(const RECT& rc);
  void UnionSimple(const RECT& rc);

  // 运行完之后，m_prcArray的大小与m_nCount可能不匹配
  bool IntersectRect(const RECT *prc, bool OnlyTest = false);
#if defined(OS_WIN)
  HRGN CreateRgn();
#endif
  void GetUnionRect(RECT *prc);

  enum { STACK_SIZE = 3 };

private:
  void add_rects_same_topbottom(std::vector<RECT>& rects);
  void add_rect_and_merge_vert(RECT& rc);

private:
  // 优先使用栈数组，放不下时再使用堆数组。类似于string类
  RECT m_stackArray[STACK_SIZE];
  RECT *m_heapArray;

  // 注：m_nCount小于等于m_prcArray的实际大小，见IntersectRect
  unsigned int m_nCount;
  unsigned int m_nCapacity; // TODO
};
} // namespace ui