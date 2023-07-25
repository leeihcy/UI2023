#pragma once
#include "attributebase.h"

namespace ui
{

class BoolAttribute : public AttributeBase
{
public:
    BoolAttribute();
    ~BoolAttribute();
    IBoolAttribute*  GetIBoolAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Reset() override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;

    void  SetBindValue(void*) override;
    // void  SetBindFuction(void* _this, void* _setter, void* _getter) override;
    void Bind(slot<void(bool)>&& s, slot<bool()>&& g);

public:
	BoolAttribute*  SetDefault(bool b);

    bool  GetBool();
    void  SetBool(bool);
    bool  GetDefaultBool() { return m_bDefault; }

private:
    IBoolAttribute*  m_pIBoolAttribute;

    bool*  m_pBindValue;
    bool   m_bDefault;

    // void*   _this;
    // pfnBoolSetter  _setter;
    // pfnBoolGetter  _getter;
    signal<void(bool)> m_setter;
    signal<bool()> m_getter;
};

AttributeBase*  CreateBoolAttribute();

}