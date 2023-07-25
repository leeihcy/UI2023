#pragma once

namespace ui {
class RectArray
{
public:
    RectArray();
    ~RectArray();
    RectArray(RectArray& o);
    RectArray& operator=(const RectArray& o);

    RECT*  GetArrayPtr();
    const RECT*  GetArrayPtr2() const;
    RECT*  GetRectPtrAt(uint nIndex);
    uint  GetCount() const;
    void  AddRect(const RECT*);

    void  Destroy();
    void  CopyFrom(const RectArray* po);
    void  CopyFromArray(const RECT* pArray, uint nCount);
    void  SetSize(uint nCount);
    bool  SetAt(uint nIndex, RECT*  pValue);
    void  Offset(int x, int y);
    
    // 添加一个脏区域，合并到现有的数组当中。
    void  UnionDirtyRect(const RECT* prc);

    // 运行完之后，m_prcArray的大小与m_nCount可能不匹配
    bool  IntersectRect(const RECT* prc, bool OnlyTest=false);
#if defined(OS_WIN)
    HRGN  CreateRgn();
#endif
    void  GetUnionRect(RECT* prc);

    enum {
        STACK_SIZE = 3
    };
private:
    // 优先使用栈数组，放不下时再使用堆数组。类似于string类
    RECT  m_stackArray[STACK_SIZE];
    RECT*  m_heapArray;

    // 注：m_nCount小于等于m_prcArray的实际大小，见IntersectRect
    uint   m_nCount;
	uint   m_nCapacity;  // TODO
};
}