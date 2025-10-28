#pragma once
#include "listitem_animate.h"


class ListItemMoveAnimateImpl : public ListItemAnimateImpl
{
public:
    ListItemMoveAnimateImpl();
    ~ListItemMoveAnimateImpl();

    enum TIMELINE_INDEX
    {
        LEFT,TOP,RIGHT,BOTTOM
    };
    enum MAGIC
    {
        CLASSTYPE = 163091241,
    };

public:
    void  SetFrom(LPCRECT);
    void  SetTo(LPCRECT);
    void  SetEaseType(UIA::EaseType);

    virtual bool  StartReq(UIA::IStoryboard*) override;
    virtual bool  IsEqual(AnimateImpl* p) override;
    virtual void  OnEnd(UIA::eAnimateEndReason) override;
    virtual UIA::eAnimateTickResult  OnTick(UIA::IStoryboard*) override;

	static void  Cancel(UI::ListItemBase& item);
protected:

protected:
    CRect  m_rcFrom;
    CRect  m_rcTo;
    UIA::EaseType  m_easeType; 
    bool  m_bFloatItem;
};