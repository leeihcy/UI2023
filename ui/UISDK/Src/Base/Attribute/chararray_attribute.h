#pragma once
#include "attributebase.h"

namespace UI
{

class CharArrayAttribute : public AttributeBase
{
public:
    CharArrayAttribute();
    ~CharArrayAttribute();
    ICharArrayAttribute*  GetICharArrayAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
    virtual bool  IsDefaultValue() override;
    void  SetBindValue(void*);
    void  SetBindFuction(void* _this, void* _setter, void* _getter);
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy *, EditorAttributeFlag) override;
    void  SetBindValueSize(long lSize);
    
public:
	CharArrayAttribute*  SetDefault(LPCTSTR);
	LPCTSTR  GetDefault();

protected:
    ICharArrayAttribute*  m_pICharArrayAttribute;

    wchar_t*  m_pBindValue;
    long  m_lBindValueSize;

    String   m_strDefault;
	bool  m_bDefaultIsNULL;   // 默认是nullptr，而不是""

    void*  _this;
    pfnStringSetter  _setter;
    pfnStringGetter  _getter;
};

AttributeBase*  CreateCharArrayAttribute();

}