#pragma once

class RectArray
{
public:
    RectArray();
    ~RectArray();
    RectArray(RectArray& o);
    RectArray& operator=(const RectArray& o);

    LPRECT  GetArrayPtr();
    LPCRECT  GetArrayPtr2() const;
    LPRECT  GetRectPtrAt(uint nIndex);
    uint  GetCount() const;
    void  AddRect(LPCRECT);

    void  Destroy();
    void  CopyFrom(const RectArray* po);
    void  CopyFromArray(LPCRECT pArray, uint nCount);
    void  SetSize(uint nCount);
    bool  SetAt(uint nIndex, RECT*  pValue);
    void  Offset(int x, int y);
    
    // 添加一个脏区域，合并到现有的数组当中。
    void  UnionDirtyRect(LPCRECT prc);

    // 运行完之后，m_prcArray的大小与m_nCount可能不匹配
    bool  IntersectRect(LPCRECT prc, bool OnlyTest=false);

    HRGN  CreateRgn();
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
