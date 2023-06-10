#pragma once
#include "Inc\Interface\iobjectdescription.h"
#include "Inc\Interface\iwindow.h"
#include "Inc\Interface\icustomwindow.h"

class WindowDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static WindowDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = IWindow::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_WINDOW;
	}
	virtual long  GetMinorType() override
	{
		return WINDOW_SIMPLE;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_WINDOW; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"Window";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(IWindow);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 500;
        h = 500;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return nullptr;
    }
};

class CustomWindowDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static CustomWindowDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::ISkinRes* p, void** pp) override
    {
        *pp = UI::ICustomWindow::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_WINDOW;
	}
	virtual long  GetMinorType() override
	{
		return WINDOW_CUSTOM;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_WINDOW; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"CustomWindow";
	}
	virtual const GUID&  GetGUID() override
	{
		return __uuidof(ICustomWindow);
	}

	virtual void  GetDefaultSize(long& w, long& h) override
	{
		w = 500;
		h = 500;
	}
	virtual HBITMAP  LoadIcon() override 
	{
		return nullptr;
	}
};