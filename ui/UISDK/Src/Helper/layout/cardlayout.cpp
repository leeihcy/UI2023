#include "stdafx.h"
#include "cardlayout.h"
#include "Src\Base\Object\object.h"

using namespace UI;


CardLayout::CardLayout()
{
}

SIZE  CardLayout::Measure()
{
    SIZE size = {0,0};

    Object*  pChild = nullptr;
    while (pChild = this->m_pPanel->EnumChildObject(pChild))
    {
        SIZE  s = pChild->GetDesiredSize();

        if (size.cx < s.cx)
            size.cx = s.cx;
        if (size.cy < s.cy)
            size.cy = s.cy;
    }
    return size;
}
void  CardLayout::DoArrage(IObject* pIObjToArrage)
{
    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

    Object* pObjToArrage = nullptr;
    if (pIObjToArrage)
        pObjToArrage = pIObjToArrage->GetImpl();

    Object* pChild = nullptr;
    while (pChild = m_pPanel->EnumChildObject(pChild))
    {
        if (pObjToArrage && pObjToArrage != pChild)
            continue;

        CRect rcChildObj ;
        int nChildW = nWidth;// - pChild->GetMarginW();
        int nChildH = nHeight;// - pChild->GetMarginH();
        rcChildObj.SetRect(0, 0, nChildW, nChildH );

        pChild->SetObjectPos(&rcChildObj, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);

        if (pObjToArrage && pObjToArrage == pChild)
            break;
    }
}

// 一个对象显示后，将其它对象隐藏掉
void  CardLayout::ChildObjectVisibleChanged(IObject* pIObj)
{
    UIASSERT (pIObj);
	UIASSERT(pIObj->GetParentObject());
	UIASSERT(pIObj->GetParentObject()->GetImpl() == m_pPanel);

    Object* pObj = pIObj->GetImpl();
    if (!pObj->IsSelfVisible())
        return;

    Object* pChild = nullptr;
    while (pChild = m_pPanel->EnumChildObject(pChild))
    {
        if (pChild != pObj && pChild->IsSelfVisible())
        {
            pChild->SetVisible(false);
        }
    }

    // SetDirty(true);
    m_pPanel->Invalidate();
}