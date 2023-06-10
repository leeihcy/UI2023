#pragma once
#include "attributebase.h"
#include "attribute_alias.h"

namespace UI
{

class LongAttribute : public AttributeBase
{
public:
    LongAttribute();
    ~LongAttribute();
    ILongAttribute*  GetILongAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy*, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;

    void  SetBindValue(void*);
    void  SetBindFuction(void* _this, void* _setter, void* _getter);

public:
	LongAttribute*  SetDefault(long l);
    LongAttribute*  AddAlias(long, LPCTSTR);
	LongAttribute*  SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e);

    long  GetLong();
    void  SetLong(long);

	uint  GetAliasCount();
	long  EnumAlias(pfnEnumAliasCallback, WPARAM, LPARAM);

protected:
    long*  m_pBindValue;
    long   m_lDefault;

    void*   _this;
    pfnLongSetter  _setter;
    pfnLongGetter  _getter;

    LongAttributeAlias  m_mapAlias;

	LONGATTR_DPI_SCALE_TYPE  m_eDpiScaleType;
private:
    ILongAttribute*  m_pILongAttribute;
};

AttributeBase*  CreateLongAttribute();

}