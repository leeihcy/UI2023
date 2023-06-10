#pragma once
#include "attributebase.h"

namespace UI
{
class RenderBaseAttribute : public AttributeBase
{
public:
    RenderBaseAttribute();
	~RenderBaseAttribute();

    virtual LPCTSTR  Get() override; 
    virtual void  Set(LPCTSTR) override;
	virtual void  Reset() override;
    virtual bool  IsDefaultValue() override;
    void  SetBindValue(void*);
    void  SetBindFuction(void* _this, void* _setter, void* _getter)
    {
        // TODO:
        UIASSERT(0);
        return;
    }

	virtual void  Load(SERIALIZEDATA*) override;
	virtual void  Save(SERIALIZEDATA*) override;
	virtual void  Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e) override;

public:
    void  SetObjectPtr(Object* p)
    {
        m_pObject = p; 
    }
    void  SetPrefix(LPCTSTR szPrefix)
    {
        if (szPrefix)
            m_strPrefix = szPrefix;
        else
            m_strPrefix.clear();
    }
    IRenderBaseAttribute*  GetIRenderBaseAttribute();

private:
	void  do_child_action(SERIALIZEDATA*);

private:
    IRenderBaseAttribute*  m_pIRenderBaseAttribute;

    IRenderBase**  m_ppBindValue;
    Object*  m_pObject;

    String  m_strPrefix;

//     void*  _this;
//     pfnStringSetter  _setter;
//     pfnStringGetter  _getter;
};

AttributeBase*  CreateRenderBaseAttribute();
}