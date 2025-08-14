#ifndef _UI_SDK_INCLUDE_UTIL_RECTREGION_H_
#define _UI_SDK_INCLUDE_UTIL_RECTREGION_H_
#include "ui/sdk/include/interface.h"
#include "ui/sdk/include/util/rect.h"

namespace ui {
class UIAPI RectRegion {
public:
  RectRegion();
  ~RectRegion();
  RectRegion(const RectRegion &o);
  RectRegion &operator=(const RectRegion &o);

  Rect *RectPtr();
  const Rect *RectPtr2() const;
  Rect *GetRectPtrAt(unsigned int nIndex);
  unsigned int Count() const;
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

  // 运行完之后，m_prcArray的大小与m_count可能不匹配
  bool IntersectRect(const Rect *prc, bool OnlyTest = false);
  bool Contains(const RectRegion& region) const;
  bool Contains(const Rect& rect) const;
  void GetUnionRect(Rect *prc);

  enum { STACK_SIZE = 3 };

private:
  void add_rects_same_topbottom(std::vector<Rect>& rects);
  void add_rect_and_merge_vert(Rect& rc);

private:
  // 优先使用栈数组，放不下时再使用堆数组。类似于string类
  Rect m_stackArray[STACK_SIZE];
  Rect *m_heapArray;

  // 注：m_count小于等于m_prcArray的实际大小，见IntersectRect
  unsigned int m_count;
  unsigned int m_capacity; // TODO
};

typedef RectRegion DirtyRegion;

} // namespace ui

#endif