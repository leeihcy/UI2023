#pragma once
#include "long_attribute.h"

//
// Àý£º layout.align="left|top"
//
namespace ui
{

class FlagsAttribute : public LongAttribute
{
public:
    FlagsAttribute();
    ~FlagsAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy*, EditorAttributeFlag e) override;

public:
    FlagsAttribute*  AddFlag(long, const wchar_t*);
    IFlagsAttribute*  GetIFlagsAttribute();

private:
    IFlagsAttribute*  m_pIFlagsAttribute;
};

AttributeBase*  CreateFlagsAttribute();

}