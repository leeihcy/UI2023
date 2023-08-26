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

    virtual const char*  Get() override; 
    virtual void  Set(const char*) override;
	virtual void  Editor(SerializeParam* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;

public:
    StringEnumAttribute*  Add(const char*);
    bool  IsExist(const char*);
    void  EnumString(pfnEnumStringEnumCallback, llong, llong);
    
    StringEnumAttribute*  FillRenderBaseTypeData();
    StringEnumAttribute*  FillTextRenderBaseTypeData();
    StringEnumAttribute*  FillLayoutTypeData();

    IStringEnumAttribute*  GetIStringEnumAttribute();

private:
    IStringEnumAttribute*  m_pIStringEnumAttribute;

    bool  m_bEditable;  // ���˴��б���ѡ��һ��������Լ��༭һ��ֵ
    std::list<std::string>  m_list;
};

AttributeBase*  CreateStringEnumAttribute();

}