#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

// 平均分配布局
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
	LAYOUT_AVERAGE_DIRECTION  direction;    // 堆栈排布的方向
    Object*   m_pPanel;   // 与该布局关联的panel
    int   m_nGap;  // 两个子对象之间间隙,
};



}