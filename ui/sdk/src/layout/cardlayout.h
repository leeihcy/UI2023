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
    bool IsSizedByContent() override { return false; }
};

class CardLayout : public LayoutImpl<CardLayout, IGridLayout, CardLayoutParam>
{
public:
    CardLayout();

    void  Serialize(SERIALIZEDATA*) override{};
    Size  Measure() override;
    void  DoArrage(IObject* pObjToArrage = nullptr) override;
    void  ChildObjectVisibleChanged(IObject* pObj) override;
};


}