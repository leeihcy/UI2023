#pragma once
#include "string_attribute.h"

//
// ���� layout="stack"
//
namespace ui
{

class StringEnumAttribute : public StringAttribute
{
public:
    StringEnumAttribute();
    ~StringEnumAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;

public:
    StringEnumAttribute*  Add(const wchar_t*);
    bool  IsExist(const wchar_t*);
    void  EnumString(pfnEnumStringEnumCallback, long, long);
    
    StringEnumAttribute*  FillRenderBaseTypeData();
    StringEnumAttribute*  FillTextRenderBaseTypeData();

    IStringEnumAttribute*  GetIStringEnumAttribute();

private:
    IStringEnumAttribute*  m_pIStringEnumAttribute;

    bool  m_bEditable;  // ���˴��б���ѡ��һ��������Լ��༭һ��ֵ
    std::list<String>  m_list;
};

AttributeBase*  CreateStringEnumAttribute();

}