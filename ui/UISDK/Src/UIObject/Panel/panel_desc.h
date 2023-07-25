#pragma once
#include "include/interface/iobjectdescription.h"
#include "include/interface/ipanel.h"

class PanelDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static PanelDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IPanel::CreateInstance(p);
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
		return L"Panel";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IPanel);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 100;
        h = 100;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return nullptr;
    }
};

class RoundPanelDescription : public PanelDescription
{
public:
	static IObjectDescription*  Get()
	{
		static RoundPanelDescription s;
		return &s;
	}

	virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
	{
		*pp = IRoundPanel::CreateInstance(p);
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"RoundPanel";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IRoundPanel);
	}
};