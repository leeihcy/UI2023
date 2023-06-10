#pragma once
#include "Inc\Interface\ilayout.h"
#include "Src\Helper\layout\layout.h"

namespace UI{

class Object;

class CardLayoutParam : public LayoutParamImpl<ICardLayoutParam>
{
public:
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_CARD; }
    virtual bool  IsSizedByContent() { return false; }
};

class CardLayout : public LayoutImpl<CardLayout, ICardLayout, CardLayoutParam, LAYOUT_TYPE_CARD>
{
public:
    CardLayout();

    virtual void  Serialize(SERIALIZEDATA*) override{};
    virtual SIZE  Measure() override;
    virtual void  DoArrage(IObject* pObjToArrage = nullptr) override;
    virtual void  ChildObjectVisibleChanged(IObject* pObj) override;
};


}