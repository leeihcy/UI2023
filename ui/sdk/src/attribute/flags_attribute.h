#pragma once
#include "long_attribute.h"

//
// Àý£º layout.align="left|top"
//
namespace ui
{

class FlagsAttribute : public IntAttribute
{
public:
    FlagsAttribute();
    ~FlagsAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Editor(SerializeParam* pData, AttributeEditorProxy*, EditorAttributeFlag e) override;

public:
    FlagsAttribute*  AddFlag(int, const wchar_t*);
    IFlagsAttribute*  GetIFlagsAttribute();

private:
    IFlagsAttribute*  m_pIFlagsAttribute;
};

AttributeBase*  CreateFlagsAttribute();

}