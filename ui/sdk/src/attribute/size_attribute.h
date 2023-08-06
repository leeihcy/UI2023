#pragma once
#include "attributebase.h"

namespace ui
{

class SizeAttribute : public AttributeBase
{
public:
    SizeAttribute();
    ~SizeAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Reset() override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
    void  SetBindValue(void*) override;
    // void  SetBindFuction(void* _this, void* _setter, void* _getter) override;
    void Bind(slot<void(Size*)>&& s, slot<void(Size*)>&& g);

public:
	SizeAttribute*  SetDefault(Size*);
    ISizeAttribute*  GetISizeAttribute();

private:
    void  get(Size*);
    void  set(Size*);

private:
    ISizeAttribute*  m_pISizeAttribute;

    Size*  m_pBindValue;
    Size   m_sizeDefault;

    // void*   _this;
    // pfnSizeSetter  _setter;
    // pfnSizeGetter  _getter;
    signal<void(Size*)>  m_setter;
    signal<void(Size*)>  m_getter;
};

AttributeBase*  CreateSizeAttribute();

}