#pragma once
#include "animate_wrap.h"
#include "include/interface/ilistitembase.h"

namespace ui
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
    virtual UI::IApplication*  GetUIApplication() override;
	virtual void  PostEnd() override;
    
    // 由派生类调过来的。
    bool  IsEqual(ListItemAnimateImpl* p);

protected:
    UI::IListItemBase*  m_pListItem;

	//pfnListItemAnimateEndCallback  m_funEndCallback;
	//long*  m_pEndCallbackData;
};
