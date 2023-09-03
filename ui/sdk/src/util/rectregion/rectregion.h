#ifndef _UI_SDK_SRC_UTIL_RECTREGION_RECTREGION_H_
#define _UI_SDK_SRC_UTIL_RECTREGION_RECTREGION_H_
#include "include/interface.h"
#include "include/util/rect.h"
#include <vector>

namespace ui {
class RectRegion {
public:
  RectRegion();
  ~RectRegion();
  RectRegion(RectRegion &o);
  RectRegion &operator=(const RectRegion &o);

  Rect *GetArrayPtr();
  const Rect *GetArrayPtr2() const;
  Rect *GetRectPtrAt(unsigned int nIndex);
  uint GetCount() const;
  void AddRect(const Rect&);

  void Destroy();
  void CopyFrom(const RectRegion *po);
  void CopyFromArray(const Rect *pArray, unsigned int nCount);
  void SetSize(unsigned int nCount);
  bool SetAt(unsigned int nIndex, Rect *pValue);
  void Offset(int x, int y);

  // 添加一个脏区域，合并到现有的数组当中。
  void Union(const Rect& rc);
  void UnionSimple(const Rect& rc);

  // 运行完之后，m_prcArray的大小与m_nCount可能不匹配
  bool IntersectRect(const Rect *prc, bool OnlyTest = false);
#if 0 // defined(OS_WIN)
  HRGN CreateRgn();
#endif
  void GetUnionRect(Rect *prc);

  enum { STACK_SIZE = 3 };

private:
  void add_rects_same_topbottom(std::vector<Rect>& rects);
  void add_rect_and_merge_vert(Rect& rc);

private:
  // 优先使用栈数组，放不下时再使用堆数组。类似于string类
  Rect m_stackArray[STACK_SIZE];
  Rect *m_heapArray;

  // 注：m_nCount小于等于m_prcArray的实际大小，见IntersectRect
  unsigned int m_nCount;
  unsigned int m_nCapacity; // TODO
};
} // namespace ui

#endif