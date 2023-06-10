#pragma once
#include "Inc\Interface\ilayout.h"
#include "Src\Helper\layout\layout.h"

namespace UI
{

class Object;
class VertLayout;

class VertLayoutParam : public LayoutParamImpl<IVertLayoutParam>
{
public:
    VertLayoutParam();
    ~VertLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_VERT; }
    virtual SIZE  CalcDesiredSize();
    virtual bool  IsSizedByContent();

public:
    virtual long  GetConfigWidth() override;
    virtual void  SetConfigWidth(long) override;
    void  LoadConfigWidth(LPCTSTR);
    LPCTSTR  SaveConfigWidth();

    virtual long  GetConfigHeight() override;
    virtual void  SetConfigHeight(long) override;
    void  LoadConfigHeight(LPCTSTR);
    LPCTSTR  SaveConfigHeight();

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

    virtual SIZE  Measure() override;
	virtual void  DoArrage(IObject* pObjToArrage = nullptr) override;
	virtual void  Serialize(SERIALIZEDATA* pData) override;
    virtual void  ChildObjectVisibleChanged(IObject* pObj) override;
    virtual void  ChildObjectContentSizeChanged(IObject* pObj) override;
    virtual void  SetSpace(int n);

protected:

    void  LoadGap(long);
    long  SaveGap();

public:
    long  m_nSpace;  // 两个子对象之间间隙
};



}