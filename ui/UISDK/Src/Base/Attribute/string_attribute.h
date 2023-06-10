#pragma once
#include "attributebase.h"

namespace UI
{

class StringAttribute : public AttributeBase
{
public:
    StringAttribute();
    ~StringAttribute();
    IStringAttribute*  GetIStringAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;

public:
	StringAttribute*  SetDefault(LPCTSTR);
	LPCTSTR  GetDefault();
	// void  Internationalization();

    void  BindReference(String&);
    void  BindFunction(void* _this, pfnStringSetter s, pfnStringGetter g);
    void  BindFunction(const function<void(LPCTSTR)>& s, const function<LPCTSTR()>& g);

protected:
    IStringAttribute*  m_pIStringAttribute;
    String   m_strDefault;
    String*  m_pBindValue;

    void*  _this;
    pfnStringSetter  _setter;
    pfnStringGetter  _getter;

    std::function<void(LPCTSTR)>  _func_setter;
    std::function<LPCTSTR()>  _func_getter;
};

class I18nStringAttribute : public StringAttribute
{
public:

    virtual LPCTSTR  Get() override;
    virtual void  Set(LPCTSTR) override;

    void  BindFunctionEx(const function<void(LPCTSTR, int)>& s, const function<LPCTSTR()>& g);
    // void  BindFunctionEx(void* _this, pfnStringExSetter s, pfnStringGetter g);

    // pfnStringExSetter  _setter_ex;
    std::function<void(LPCTSTR, int)>  _func_setter_ex;
};

AttributeBase*  CreateStringAttribute();
AttributeBase*  CreateI18nStringAttribute();
}