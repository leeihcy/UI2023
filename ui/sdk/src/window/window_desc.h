#pragma once
#include "include/interface/iobjectdescription.h"
#include "include/interface/iwindow.h"
// #include "include/interface/icustomwindow.h"

namespace ui {

class WindowDescription : public IObjectDescription {
public:
  static IObjectDescription *Get() {
    static WindowDescription s;
    return &s;
  }

  virtual void CreateInstance(IResBundle *p, void **pp) override {
    *pp = IWindow::CreateInstance(p);
  }

  virtual OBJ_TYPE GetMajorType() override { return OBJ_WINDOW; }
  virtual long GetMinorType() override { return WINDOW_SIMPLE; }
  virtual const wchar_t *GetCategory() override { return CATEGORY_WINDOW; }
  virtual const wchar_t *GetTagName() override { return L"Window"; }
  virtual Uuid GetGuid() override { 
    return IWindow::UUID();
  }

  virtual void GetDefaultSize(long &w, long &h) override {
    w = 500;
    h = 500;
  }
  virtual HBITMAP LoadIcon() override { return 0; }
};
#if 0
class CustomWindowDescription : public IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static CustomWindowDescription s;
		return &s;
	}

    virtual void  CreateInstance(IResBundle* p, void** pp) override
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
	virtual const UUID&  GetGuid() override
	{
		return __guid(ICustomWindow);
	}

	virtual void  GetDefaultSize(long& w, long& h) override
	{
		w = 500;
		h = 500;
	}
	virtual HBITMAP  LoadIcon() override 
	{
		return 0;
	}
};
#endif
} // namespace ui