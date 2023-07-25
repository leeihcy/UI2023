#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

// ƽ�����䲼��
namespace ui
{

class Object;

class AverageLayoutParam : public LayoutParamImpl<IAverageLayoutParam>
{
public:
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_AVERAGE; }
    virtual bool  IsSizedByContent() { return false; }
};


class AverageLayout : public LayoutImpl<AverageLayout, IAverageLayout, AverageLayoutParam, LAYOUT_TYPE_AVERAGE>
{
public:
	AverageLayout();
    ~AverageLayout();

    virtual SIZE  Measure() override;
	virtual void  DoArrage(IObject* pObjToArrage = nullptr) override;
	virtual void  Serialize(SERIALIZEDATA*) override;
    virtual void  ChildObjectVisibleChanged(IObject* pObj) override;

    void  ArrangeObject_H(Object* pChildObj, RECT* prc);
    void  ArrangeObject_V(Object* pChildObj, RECT* prc);

public:
	LAYOUT_AVERAGE_DIRECTION  direction;    // ��ջ�Ų��ķ���
    Object*   m_pPanel;   // ��ò��ֹ�����panel
    int   m_nGap;  // �����Ӷ���֮���϶,
};



}