#pragma once
#include "attributebase.h"
#include <functional>

namespace ui
{

class StringAttribute : public AttributeBase
{
public:
    StringAttribute();
    ~StringAttribute();
    IStringAttribute*  GetIStringAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Reset() override;
	virtual void  Editor(SerializeParam* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;

public:
	StringAttribute*  SetDefault(const wchar_t*);
	const wchar_t*  GetDefault();
	// void  Internationalization();

    void  BindReference(String&);
    // void  BindFunction(void* _this, pfnStringSetter s, pfnStringGetter g);
    void Bind(slot<void(const wchar_t*)>&& s, slot<const wchar_t*()>&& g);
    // void  BindFunction(
    //     const std::function<void(const wchar_t*)>& s, 
    //     const std::function<const wchar_t*()>& g);

protected:
    IStringAttribute*  m_pIStringAttribute;
    String   m_strDefault;
    String*  m_pBindValue;

    // void*  _this;
    // pfnStringSetter  _setter;
    // pfnStringGetter  _getter;

    signal<void(const wchar_t*)>  m_setter;
    signal<const wchar_t*()> m_getter;
};

class I18nStringAttribute : public StringAttribute
{
public:

    virtual const wchar_t*  Get() override;
    virtual void  Set(const wchar_t*) override;

    void  BindEx(
        slot<void(const wchar_t*, int)>&& s, 
        slot<const wchar_t*()>&& g);
    // void  BindFunctionEx(void* _this, pfnStringExSetter s, pfnStringGetter g);

    // pfnStringExSetter  _setter_ex;
    signal<void(const wchar_t*, int)>  m_setter_ex;
};

AttributeBase*  CreateStringAttribute();
AttributeBase*  CreateI18nStringAttribute();
}