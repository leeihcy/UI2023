#pragma once
#include "int_attribute.h"

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

    virtual const char*  Get() override; 
    virtual void  Set(const char*) override;
	virtual void  Editor(SerializeParam* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;

public:
    EnumAttribute*  AddOption(int, const char*);
	EnumAttribute*  FillGraphicsRenderLibraryData();

private:
    IEnumAttribute*  m_pIEnumAttribute;
};

AttributeBase*  CreateEnumAttribute();

}