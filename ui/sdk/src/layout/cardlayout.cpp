#include "include/inc.h"
#include "cardlayout.h"
#include "src/object/object.h"

using namespace ui;


CardLayout::CardLayout()
{
}

Size  CardLayout::Measure()
{
    Size size = {0,0};

    Object*  pChild = nullptr;
    while ((pChild = this->m_pPanel->EnumChildObject(pChild)))
    {
        Size  s = pChild->GetDesiredSize();

        if (size.width < s.width)
            size.width = s.width;
        if (size.height < s.height)
            size.height = s.height;
    }
    return size;
}
void  CardLayout::DoArrage(IObject* pIObjToArrage)
{
    // ���øú���ʱ���Լ��Ĵ�С�Ѿ����������
    Rect rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

    Object* pObjToArrage = nullptr;
    if (pIObjToArrage)
        pObjToArrage = pIObjToArrage->GetImpl();

    Object* pChild = nullptr;
    while ((pChild = m_pPanel->EnumChildObject(pChild)))
    {
        if (pObjToArrage && pObjToArrage != pChild)
            continue;

        Rect rcChildObj ;
        int nChildW = nWidth;// - pChild->GetMarginW();
        int nChildH = nHeight;// - pChild->GetMarginH();
        rcChildObj.Set(0, 0, nChildW, nChildH );

        pChild->SetObjectPos(&rcChildObj, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);

        if (pObjToArrage && pObjToArrage == pChild)
            break;
    }
}

// һ��������ʾ�󣬽������������ص�
void  CardLayout::ChildObjectVisibleChanged(IObject* pIObj)
{
    UIASSERT (pIObj);
	UIASSERT(pIObj->GetParentObject());
	UIASSERT(pIObj->GetParentObject()->GetImpl() == m_pPanel);

    Object* pObj = pIObj->GetImpl();
    if (!pObj->IsSelfVisible())
        return;

    Object* pChild = nullptr;
    while ((pChild = m_pPanel->EnumChildObject(pChild)))
    {
        if (pChild != pObj && pChild->IsSelfVisible())
        {
            pChild->SetVisible(false);
        }
    }

    // SetDirty(true);
    m_pPanel->Invalidate();
}