#pragma once
#include "Inc\Interface\ilayout.h"
#include "Src\Helper\layout\layout.h"

namespace UI{

class Object;

class CanvasLayoutParam : public LayoutParamImpl<ICanvasLayoutParam>
{
public:
    CanvasLayoutParam();
    ~CanvasLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_CANVAS; }
    virtual SIZE  CalcDesiredSize();
    virtual bool  IsSizedByContent();

    static int  ParseAlignAttr(LPCTSTR);

public:
    virtual long  GetConfigLeft() override;
    virtual void  SetConfigLeft(long) override;
    void  LoadConfigLeft(long);
    long  SaveConfigLeft();

    virtual long  GetConfigTop() override;
    virtual void  SetConfigTop(long) override;
    void  LoadConfigTop(long);
    long  SaveConfigTop();

    virtual long  GetConfigRight() override;
    virtual void  SetConfigRight(long) override;
    void  LoadConfigRight(long);
    long  SaveConfigRight();

    virtual long  GetConfigBottom() override;
    virtual void  SetConfigBottom(long) override;
    void  LoadConfigBottom(long);
    long  SaveConfigBottom();

    virtual long  GetConfigWidth() override;
    virtual void  SetConfigWidth(long) override;
    void  LoadConfigWidth(long);
    long  SaveConfigWidth();

    virtual long  GetConfigHeight() override;
    virtual void  SetConfigHeight(long) override;
    void  LoadConfigHeight(long);
    long  SaveConfigHeight();

    virtual void SetConfigLayoutFlags(long) override;
    virtual long GetConfigLayoutFlags() override;

private:
    long  m_nConfigWidth;   // 对象的宽度，可取值： 数值 | "auto" . （对于window对象，width 是指client区域的大小，不是整个窗口的大小；width包括padding，但不包括margin）
    long  m_nConfigHeight;  // 对象的高度，可取值： 数值 | "auto" . （对于window对象，height是指client区域的大小，不是整个窗口的大小；height包括padding，但不包括margin）
    // 在这里需要说明，对象的最终占用的宽度= margin.left + width + margin.right
    // 也就是说这里的width = padding.left + padding.right + content.width
    long  m_nConfigLeft;
    long  m_nConfigRight;
    long  m_nConfigTop;
    long  m_nConfigBottom;
    long  m_nConfigLayoutFlags;
};


//
//	指定布局离边缘的位置
//
class CanvasLayout : public LayoutImpl<CanvasLayout, ICanvasLayout, CanvasLayoutParam, LAYOUT_TYPE_CANVAS>
{
public:
    virtual void  Serialize(SERIALIZEDATA*) override{};
    virtual SIZE  Measure() override;
    virtual void  DoArrage(IObject* pObjToArrage = nullptr) override;
    virtual void  ChildObjectVisibleChanged(IObject* pObj) override;
    virtual void  ChildObjectContentSizeChanged(IObject* pObj) override;

public:
    static void  ArrangeObject(Object*  pChild, const int& nWidth, const int& nHeight);
};

}
