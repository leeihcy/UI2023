#pragma once
#include "include/interface/imeta.h"
#include "include/interface/iscrollpanel.h"

class ScrollPanelDescription : public IMeta
{
public:
	static IMeta*  Get()
	{
		static ScrollPanelDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::IResource* p, void** pp) override
    {
        *pp = IScrollPanel::create(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_PANEL;
	}
	virtual long  GetMinorType() override
	{
		return PANEL_SIMPLE;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTAINER; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"ScrollPanel";
	}
	virtual const GUID&  GetUuid() override
	{
		return __uuidof(IScrollPanel);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 200;
        h = 200;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return nullptr;
    }
};
