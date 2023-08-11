#pragma once
#include "sdk/include/interface/iobjectdescription.h"
#include "include/interface/irect.h"
#include "src/private_inc.h"

namespace ui { namespace svg {

class RectDescription : public ui::IObjectDescription
{
public:
	static IObjectDescription*  Get()
	{
		static RectDescription s;
		return &s;
	}

    virtual void  CreateInstance(ui::IResBundle* p, void** pp) override
    {
        *pp = IRect::CreateInstance(p);
    }

	virtual OBJ_TYPE  GetMajorType() override
	{
		return OBJ_CONTROL;
	}
	virtual long  GetMinorType() override
	{
		return CONTROL_SVG;
	}
	virtual const wchar_t*  GetCategory() override
	{
		return CATEGORY_SVG; 
	}
	virtual const wchar_t*  GetTagName() override
	{
		return L"Rect";
	}
	virtual const Guid&  GetGuid() override
	{
		return __guid(IRect);
	}

    virtual void  GetDefaultSize(long& w, long& h) override
    {
        w = 100;
        h = 40;
    }
    virtual HBITMAP  LoadIcon() override 
    {
        return 0;
    }
};

}}