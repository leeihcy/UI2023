#pragma once
#include "attributebase.h"

namespace UI
{

class Region9Attribute : public AttributeBase
{
public:
    Region9Attribute();
    ~Region9Attribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;
    virtual bool  IsDefaultValue() override;
    void  SetBindValue(void*);
    void  SetBindFuction(void* _this, void* _setter, void* _getter);

public:
	Region9Attribute*  SetDefault(C9Region*);
    IRegion9Attribute*  GetIRegion9Attribute();

private:
    void  get(C9Region*);
    void  set(C9Region*);

private:
    IRegion9Attribute*  m_pIRegion9Attribute;

    C9Region*  m_pBindValue;
    C9Region   m_rDefault;

    void*   _this;
    pfn9RegionSetter  _setter;
    pfn9RegionGetter  _getter;
};

AttributeBase*  CreateRegion9Attribute();

}