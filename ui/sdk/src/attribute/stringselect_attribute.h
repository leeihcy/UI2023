#pragma once
#include "string_attribute.h"

//
// 例： layout="stack"
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

    bool  m_bEditable;  // 除了从列表中选择一项，还可以自己编辑一个值
    std::list<String>  m_list;
};

AttributeBase*  CreateStringEnumAttribute();

}