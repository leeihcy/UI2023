#pragma once
#include "attributebase.h"

namespace ui
{

class RectAttribute : public AttributeBase
{
public:
    RectAttribute();
    ~RectAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Reset() override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
    void  SetBindValue(void*) override;
    // void  SetBindFuction(void* _this, void* _setter, void* _getter) override;
    void Bind(slot<void(RECT*)>&& s, slot<void(RECT*)>&& g);

public:
	RectAttribute*  SetDefault(RECT*);
    IRectAttribute*  GetIRectAttribute();

private:
    void  get(RECT*);
    void  set(RECT*);

private:
    IRectAttribute*  m_pIRectAttribute;

    RECT*  m_pBindValue;
    RECT   m_rcDefault;

    // void*   _this;
    // pfnRectSetter  _setter;
    // pfnRectGetter  _getter;
    signal<void(RECT*)> m_setter;
    signal<void(RECT*)> m_getter;
};

AttributeBase*  CreateRectAttribute();

}