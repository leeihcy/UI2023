#pragma once
#include "attributebase.h"

namespace UI
{

class ColorAttribute : public AttributeBase
{
public:
    ColorAttribute();
	~ColorAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
	void  SetBindValue(void*);
    void  SetBindValue2(void**);
    void  SetBindFuction(void* _this, void* _setter, void* _getter)
    {
        // TODO:
        UIASSERT(0);
        return;
    }

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
