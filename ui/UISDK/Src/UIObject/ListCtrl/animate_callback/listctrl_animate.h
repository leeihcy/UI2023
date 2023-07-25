#pragma once

// �����ķ�װʵ���� UIAnimate.dll �У�uisdk.dll�������� AnimateBase.dll��
// ����ֱ�ӵ��� UIAnimate ģ��Ľӿڡ������ uisdk ��Ԥ���ص��ӿڣ���ʵ��
// ���� UIAnimate ���С�


namespace ui
{
    class ListItemBase;
}

typedef bool (__stdcall *pfnListItemRectChangedCallback)(
                                IListItemBase& item,
                                LPCRECT prcOld, 
                                LPCRECT prcNew);
class ListCtrlAnimate
{
protected:
    ListCtrlAnimate();
    ~ListCtrlAnimate();

public:
    static ListCtrlAnimate&  Get();

    static bool  HandleItemRectChanged(
                    ListItemBase& item,
                    LPCRECT prcOld, 
                    LPCRECT prcNew);

private:
    pfnListItemRectChangedCallback  m_pfnListItemRectChangedCallback;
};


