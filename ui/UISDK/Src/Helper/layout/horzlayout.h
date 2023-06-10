#pragma once
#include "Inc\Interface\ilayout.h"
#include "Src\Helper\layout\layout.h"

namespace UI
{

class Object;

class HorzLayoutParam : public LayoutParamImpl<IHorzLayoutParam>
{
public:
    HorzLayoutParam();
    ~HorzLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_HORZ; }
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

    virtual void SetConfigLayoutFlags(long) override;
    virtual long GetConfigLayoutFlags() override;

private:
    long  m_nConfigWidth;  
    long  m_nConfigHeight; 
    long  m_nConfigLayoutFlags;

    long  m_eWidthType;
    long  m_eHeightType;

	friend class HorzLayout;
};

class HorzLayout : public LayoutImpl<HorzLayout, IHorzLayout, HorzLayoutParam, LAYOUT_TYPE_HORZ>
{
public:
	HorzLayout();
    ~HorzLayout();

    virtual SIZE  Measure() override;
	virtual void  DoArrage(IObject* pObjToArrage = nullptr) override;
	virtual void  Serialize(SERIALIZEDATA* pData) override;
    virtual void  ChildObjectVisibleChanged(IObject* pObj) override;
    virtual void  ChildObjectContentSizeChanged(IObject* pObj) override;

    virtual void  SetSpace(int n);

protected:
    void  ArrangeObject_Left(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size);
    void  ArrangeObject_Right(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size);

    void  LoadGap(long);
    long  SaveGap();

public:
    long  m_nSpace;  // 两个子对象之间间隙
};



}