#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

namespace ui
{

class Object;
class VertLayout;

class VertLayoutParam : public LayoutParamImpl<IVertLayoutParam>
{
public:
    VertLayoutParam();
    ~VertLayoutParam();

    virtual void  UpdateByRect() override;
    virtual void  Serialize(SERIALIZEDATA* pData) override;
    virtual LAYOUTTYPE  GetLayoutType() override { return LAYOUT_TYPE_VERT; }
    virtual Size  CalcDesiredSize()override ;
    virtual bool  IsSizedByContent()override;

public:
    virtual long  GetConfigWidth() override;
    virtual void  SetConfigWidth(long) override;
    void  LoadConfigWidth(const wchar_t*);
    const wchar_t*  SaveConfigWidth();

    virtual long  GetConfigHeight() override;
    virtual void  SetConfigHeight(long) override;
    void  LoadConfigHeight(const wchar_t*);
    const wchar_t*  SaveConfigHeight();

    virtual void  SetConfigLayoutFlags(long) override;
    virtual long  GetConfigLayoutFlags() override;

protected:
    long  m_nConfigWidth;  
    long  m_nConfigHeight; 
    long  m_nConfigLayoutFlags;

    long  m_eWidthType;
    long  m_eHeightType;
	friend VertLayout;
};

class VertLayout : public LayoutImpl<VertLayout, IVertLayout, VertLayoutParam, LAYOUT_TYPE_VERT>
{
public:
	VertLayout();
    ~VertLayout();

    virtual Size  Measure() override;
	virtual void  DoArrage(IObject* pObjToArrage = nullptr) override;
	virtual void  Serialize(SERIALIZEDATA* pData) override;
    virtual void  ChildObjectVisibleChanged(IObject* pObj) override;
    virtual void  ChildObjectContentSizeChanged(IObject* pObj) override;
    virtual void  SetSpace(int n) override;

protected:

    void  LoadGap(long);
    long  SaveGap();

public:
    long  m_nSpace;  // 两个子对象之间间隙
};



}