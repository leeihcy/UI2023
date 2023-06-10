#include "stdafx.h"
#include "rectarray.h"

RectArray::RectArray()
{
    memset(m_stackArray, 0, sizeof(m_stackArray));
    m_heapArray = 0;
    m_nCount = 0;
	m_nCapacity = 0;
}
RectArray::~RectArray()
{
    Destroy();
}

RectArray::RectArray(RectArray& o)
{
    m_heapArray = 0;
    m_nCount = 0;
	m_nCapacity = 0;
    this->CopyFrom(&o);
}

RectArray& RectArray::operator=(const RectArray& o)
{
    m_heapArray = 0;
    m_nCount = 0;
	m_nCapacity = 0;
    this->CopyFrom(&o);
    return *this;
}

LPRECT  RectArray::GetArrayPtr()
{
    if (m_heapArray)
        return m_heapArray;

    return m_stackArray;
}

LPCRECT  RectArray::GetArrayPtr2() const
{
    if (m_heapArray)
        return m_heapArray;

    return m_stackArray;
}

LPRECT  RectArray::GetRectPtrAt(uint nIndex)
{
    UIASSERT (nIndex < m_nCount);

    if (m_heapArray)
        return m_heapArray + nIndex;

	UIASSERT(nIndex < STACK_SIZE);
    return m_stackArray + nIndex;
}

uint  RectArray::GetCount() const 
{
    return m_nCount;
}

void  RectArray::Destroy()
{
    SAFE_ARRAY_DELETE(m_heapArray);
    memset(m_stackArray, 0, sizeof(m_stackArray));
    m_nCount = 0;
	m_nCapacity = 0;
}

void  RectArray::CopyFrom(const RectArray* po)
{
    if (!po)
        return;

    CopyFromArray(
        ((RectArray*)po)->GetArrayPtr(),
        po->m_nCount);
}

void  RectArray::CopyFromArray(LPCRECT pArray, uint nCount)
{
    Destroy();
    if (nCount > STACK_SIZE)
    {
        m_heapArray = new RECT[nCount];
		m_nCapacity = nCount;
        memcpy(m_heapArray, pArray, sizeof(RECT)*nCount);
    }
    else if (nCount)
    {
        memcpy(m_stackArray, pArray, sizeof(RECT)*nCount);
    }
    m_nCount = nCount;
}

void  RectArray::AddRect(LPCRECT prc)
{
	if (m_nCount >= STACK_SIZE || m_heapArray)
	{
		long nCount = m_nCount;
		RECT* pHeap = new RECT[nCount+1];
		memcpy(pHeap, GetArrayPtr(), m_nCount*sizeof(RECT));
		CopyRect(pHeap+m_nCount, prc);

		Destroy();
		m_heapArray = pHeap;
		m_nCount = nCount+1;
		m_nCapacity = m_nCount;
	}
	else
	{
		CopyRect(&m_stackArray[m_nCount], prc);
		m_nCount ++;
	}

	
}

void  RectArray::SetSize(uint nCount)
{
    Destroy();
    m_nCount = nCount;

    if (nCount > STACK_SIZE)
    {
        m_heapArray = new RECT[nCount];
		m_nCapacity = nCount;
        memset(m_heapArray, 0, sizeof(RECT)*nCount);
    }
}

bool  RectArray::SetAt(uint nIndex, RECT*  pValue)
{
    if (nIndex >= m_nCount)
        return false;
    if (!pValue)
        return false;

    CopyRect(GetRectPtrAt(nIndex), pValue);
    return true;
}

void  RectArray::Offset(int x, int y)
{
    for (uint i = 0; i < m_nCount; i++)
    {
        OffsetRect(GetRectPtrAt(i), x, y);
    }
}

// 运行完之后，m_heapArray的大小与m_nCount可能不匹配
bool RectArray::IntersectRect(LPCRECT prc, bool OnlyTest)
{
    if (0 == m_nCount)
        return false;

    RECT temp = {0};
    UINT nNewCount = 0;

    for (UINT i = 0; i < m_nCount; i++)
    {
        if (::IntersectRect(&temp, GetRectPtrAt(i), prc))
        {
            if (OnlyTest)
            {
                return true;
            }
            else
            {
                CopyRect(GetRectPtrAt(nNewCount), &temp);
                nNewCount++;
            }
        }
    }

    if (nNewCount == m_nCount)
        return true;

    if (0 == nNewCount)
    {
        if (OnlyTest)
            return false;

        Destroy();
        return false;
    }

    if (OnlyTest)
        return true;

    // 清空没用的位置
    for (uint i = nNewCount; i < m_nCount; i++)
        SetRectEmpty(GetRectPtrAt(i));
    
    m_nCount = nNewCount;
    return true;
}

// 场景：窗口脏区域逻辑
void  RectArray::UnionDirtyRect(LPCRECT prc)
{
    // 1. 检测有没有重叠项，或者有交集的项
	RECT rcTemp = {0};
    for (uint i = 0; i < m_nCount; i++)
    {
        LPRECT prcTest = GetRectPtrAt(i);

        if (!::IntersectRect(&rcTemp, prcTest, prc))
            continue;

        // 有交集，直接求交
        ::UnionRect(prcTest, prcTest, prc);
        return;
    }

    // 没有交集，加进来 
    this->AddRect(prc);
}

HRGN  RectArray::CreateRgn()
{
    if (0 == m_nCount || nullptr == GetArrayPtr())
    {
        return nullptr;
    }
    if (1 == m_nCount)
    {
        return CreateRectRgnIndirect(GetRectPtrAt(0));
    }

    RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + m_nCount*sizeof(RECT) ];
    memset(pRgnData, 0, sizeof(RGNDATAHEADER));
    pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
    pRgnData->rdh.iType      = RDH_RECTANGLES;
    pRgnData->rdh.nCount     = m_nCount;
    pRgnData->rdh.nRgnSize   = m_nCount*sizeof(RECT);

    RECT* prcBuffer = (RECT*)pRgnData->Buffer;
    for (UINT k = 0; k < m_nCount; k++)
    {
        CopyRect(prcBuffer+k, GetRectPtrAt(k));
    }
    HRGN hRgn = ::ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + m_nCount*sizeof(RECT), pRgnData);
    delete [] pRgnData;

    return hRgn;
}

void  RectArray::GetUnionRect(RECT* prc)
{
    SetRectEmpty(prc);
    for (uint i = 0; i < m_nCount; i++)
    {
        UnionRect(prc, GetRectPtrAt(i), prc);
    }
}