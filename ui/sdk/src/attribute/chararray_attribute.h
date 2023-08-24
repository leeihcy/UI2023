#pragma once
#include "attributebase.h"

namespace ui
{

class CharArrayAttribute : public AttributeBase
{
public:
    CharArrayAttribute();
    ~CharArrayAttribute();
    ICharArrayAttribute*  GetICharArrayAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Reset() override;
    virtual bool  IsDefaultValue() override;
    void  SetBindValue(void*);
    void  SetBindFuction(void* _this, void* _setter, void* _getter);
	virtual void  Editor(SerializeParam* pData, AttributeEditorProxy *, EditorAttributeFlag) override;
    void  SetBindValueSize(long lSize);
    
public:
	CharArrayAttribute*  SetDefault(const wchar_t*);
	const wchar_t*  GetDefault();

protected:
    ICharArrayAttribute*  m_pICharArrayAttribute;

    wchar_t*  m_pBindValue;
    long  m_lBindValueSize;

    String   m_strDefault;
	bool  m_bDefaultIsNULL;   // Ĭ����nullptr��������""

    void*  _this;
    pfnStringSetter  _setter;
    pfnStringGetter  _getter;
};

AttributeBase*  CreateCharArrayAttribute();

}