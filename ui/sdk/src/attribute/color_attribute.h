#pragma once
#include "attributebase.h"

namespace ui
{

class ColorAttribute : public AttributeBase
{
public:
    ColorAttribute();
	~ColorAttribute();

    virtual const wchar_t*  Get() override; 
    virtual void  Set(const wchar_t*) override;
	virtual void  Reset() override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
	void  SetBindValue(void*) override;
    void  SetBindValue2(void**);

public:
    IColorAttribute*  GetIColorAttribute();

private:
    IColorAttribute*  m_pIColorAttribute;

    Color**  m_ppBindValue;
	Color*   m_pBindValue;

//     void*  _this;
//     pfnLongSetter  _setter;
//     pfnLongGetter  _getter;
};

AttributeBase*  CreateColorAttribute();

}
