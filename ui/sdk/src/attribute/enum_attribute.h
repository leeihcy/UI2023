#pragma once
#include "long_attribute.h"

//
// layout="stack" <--> 2
//
namespace ui
{

class EnumAttribute : public IntAttribute
{
public:
    EnumAttribute();
    ~EnumAttribute();
    IEnumAttribute*  GetIEnumAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Editor(SerializeParam* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;

public:
    EnumAttribute*  AddOption(int, const wchar_t*);
	EnumAttribute*  FillGraphicsRenderLibraryData();

private:
    IEnumAttribute*  m_pIEnumAttribute;
};

AttributeBase*  CreateEnumAttribute();

}