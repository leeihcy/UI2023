#pragma once
#include "animate_wrap.h"
#include "Inc\Interface\ilistitembase.h"

namespace UI
{
    interface IListCtrlBase;
}

class ListItemAnimateImpl : public AnimateImpl
{
public:
    ListItemAnimateImpl();
    ~ListItemAnimateImpl();

    void  Bind(UI::IListItemBase& item);
    UI::IListCtrlBase*  GetListCtrl();

//	void  SetEndCallback(pfnListItemAnimateEndCallback f, long* pData);

protected:
    virtual bool  PreStart() override;
    virtual UI::IUIApplication*  GetUIApplication() override;
	virtual void  PostEnd() override;
    
    // 由派生类调过来的。
    bool  IsEqual(ListItemAnimateImpl* p);

protected:
    UI::IListItemBase*  m_pListItem;

	//pfnListItemAnimateEndCallback  m_funEndCallback;
	//long*  m_pEndCallbackData;
};
