#pragma once
#include "attributebase.h"
#include "attribute_alias.h"

namespace ui
{

class LongAttribute : public AttributeBase
{
public:
    LongAttribute();
    ~LongAttribute();
    ILongAttribute*  GetILongAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Reset() override;
	virtual void  Editor(SerializeParam* pData, AttributeEditorProxy*, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;

    void  SetBindValue(void*) override;
    // void  SetBindFuction(void* _this, void* _setter, void* _getter) override;
    void Bind(slot<void(long)>&& s, slot<long()>&& g);

public:
	LongAttribute*  SetDefault(long l);
    LongAttribute*  AddAlias(long, const wchar_t*);
	LongAttribute*  SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e);

    long  GetLong();
    void  SetLong(long);

	uint  GetAliasCount();
	long  EnumAlias(pfnEnumAliasCallback, long, long);

protected:
    long*  m_pBindValue;
    long   m_lDefault;

    // void*   _this;
    // pfnLongSetter  _setter;
    // pfnLongGetter  _getter;
    signal<void(long)> m_setter;
    signal<long()> m_getter;

    LongAttributeAlias  m_mapAlias;

	LONGATTR_DPI_SCALE_TYPE  m_eDpiScaleType;
private:
    ILongAttribute*  m_pILongAttribute;
};

AttributeBase*  CreateLongAttribute();

}