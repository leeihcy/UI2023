#include "include/inc.h"
#include "rectarray.h"

namespace ui {
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

RECT*  RectArray::GetArrayPtr()
{
    if (m_heapArray)
        return m_heapArray;

    return m_stackArray;
}

const RECT*  RectArray::GetArrayPtr2() const
{
    if (m_heapArray)
        return m_heapArray;

    return m_stackArray;
}

RECT*  RectArray::GetRectPtrAt(uint nIndex)
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

void  RectArray::CopyFromArray(const RECT* pArray, uint nCount)
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

void  RectArray::AddRect(const RECT* prc)
{
	if (m_nCount >= STACK_SIZE || m_heapArray)
	{
		long nCount = m_nCount;
		RECT* pHeap = new RECT[nCount+1];
		memcpy(pHeap, GetArrayPtr(), m_nCount*sizeof(RECT));
		(pHeap+m_nCount)->CopyFrom(*prc);

		Destroy();
		m_heapArray = pHeap;
		m_nCount = nCount+1;
		m_nCapacity = m_nCount;
	}
	else
	{
		m_stackArray[m_nCount].CopyFrom(*prc);
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

    GetRectPtrAt(nIndex)->CopyFrom(*pValue);
    return true;
}

void  RectArray::Offset(int x, int y)
{
    for (uint i = 0; i < m_nCount; i++)
    {
        GetRectPtrAt(i)->Offset(x, y);
    }
}

// 运行完之后，m_heapArray的大小与m_nCount可能不匹配
bool RectArray::IntersectRect(const RECT* prc, bool OnlyTest)
{
    if (0 == m_nCount)
        return false;

    RECT temp = {0};
    unsigned int nNewCount = 0;

    for (unsigned int i = 0; i < m_nCount; i++)
    {
        if (GetRectPtrAt(i)->Intersect(*prc, &temp))
        {
            if (OnlyTest)
            {
                return true;
            }
            else
            {
                GetRectPtrAt(nNewCount)->CopyFrom(temp);
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
        GetRectPtrAt(i)->SetEmpty();
    
    m_nCount = nNewCount;
    return true;
}

// 场景：窗口脏区域逻辑
void  RectArray::UnionDirtyRect(const RECT* prc)
{
    // 1. 检测有没有重叠项，或者有交集的项
	RECT rcTemp = {0};
    for (uint i = 0; i < m_nCount; i++)
    {
        RECT* prcTest = GetRectPtrAt(i);

        if (!prcTest->Intersect(*prc, &rcTemp))
            continue;

        // 有交集，直接求交
        prcTest->Union(*prc, prcTest);
        return;
    }

    // 没有交集，加进来 
    this->AddRect(prc);
}
#if defined(OS_WIN)
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
    for (unsigned int k = 0; k < m_nCount; k++)
    {
        CopyRect(prcBuffer+k, GetRectPtrAt(k));
    }
    HRGN hRgn = ::ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + m_nCount*sizeof(RECT), pRgnData);
    delete [] pRgnData;

    return hRgn;
}
#endif

void  RectArray::GetUnionRect(RECT* prc)
{
    prc->SetEmpty();
    for (uint i = 0; i < m_nCount; i++)
    {
        GetRectPtrAt(i)->Union(*prc, prc);
    }
}
}