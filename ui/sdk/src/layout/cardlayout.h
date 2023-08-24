#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

namespace ui{

class Object;

class CardLayoutParam : public ICardLayoutParam
{
public:
    CardLayoutParam(Object*) {}
    Uuid UUID() override { return ICardLayout::UUID(); }
    void Release() override { delete this; }
};

class CardLayout : public LayoutImpl<CardLayout, IGridLayout, CardLayoutParam>
{
public:
    CardLayout();

    void  Serialize(SerializeParam*) override{};
    Size  Measure() override;
    void  DoArrange(ArrangeParam* param) override;
    // void  ChildObjectVisibleChanged(IObject* pObj) override;
};


}