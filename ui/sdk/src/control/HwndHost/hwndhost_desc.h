#pragma once
#include "include/interface/iobjectdescription.h"
#include "include/interface/ihwndhost.h"

class HwndHostDescription : public IMeta
{
public:
	static IMeta*  Get()
	{
		static HwndHostDescription s;
		return &s;
	}

    virtual void  CreateInstance(UI::IResource* p, void** pp) override
    {
        *pp = IHwndHost::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_HWNDHOST;
	}
	virtual long  GetMinorType() override
	{
		return 0;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_CONTROL; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"HwndHost";
	}
	virtual const GUID&  GetUuid() override
	{
		return __uuidof(IHwndHost);
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