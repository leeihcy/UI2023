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
    void Bind(slot<void(SIZE*)>&& s, slot<void(SIZE*)>&& g);

public:
	SizeAttribute*  SetDefault(SIZE*);
    ISizeAttribute*  GetISizeAttribute();

private:
    void  get(SIZE*);
    void  set(SIZE*);

private:
    ISizeAttribute*  m_pISizeAttribute;

    SIZE*  m_pBindValue;
    SIZE   m_sizeDefault;

    // void*   _this;
    // pfnSizeSetter  _setter;
    // pfnSizeGetter  _getter;
    signal<void(SIZE*)>  m_setter;
    signal<void(SIZE*)>  m_getter;
};

AttributeBase*  CreateSizeAttribute();

}