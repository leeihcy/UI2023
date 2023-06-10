#ifndef _ILAYOUT_H_
#define _ILAYOUT_H_

namespace UI
{
class Object;
interface IMapAttribute;


interface ILayoutParam : public IRootInterface
{
    virtual void  Release() = 0;

    //  在布局前，计算控件期望的大小
    virtual SIZE  CalcDesiredSize() = 0;

    // 由当前坐标RECT反推出布局参数
    virtual void  UpdateByRect() = 0;

    // 序列化
    virtual void  Serialize(SERIALIZEDATA* pData) = 0;

    // 接口类型判断
    virtual LAYOUTTYPE  GetLayoutType() = 0;

    // 自己是自适应大小，还是固定大小，用于优化updatelayout
    virtual bool  IsSizedByContent() = 0;
};


interface ICanvasLayoutParam : public ILayoutParam
{
    virtual void SetConfigWidth(long) = 0;
    virtual void SetConfigHeight(long) = 0;
    virtual void SetConfigLayoutFlags(long) = 0;
    virtual void SetConfigLeft(long) = 0;
    virtual void SetConfigRight(long) = 0;
    virtual void SetConfigTop(long) = 0;
    virtual void SetConfigBottom(long) = 0;

    virtual long GetConfigWidth() = 0;
    virtual long GetConfigHeight() = 0;
    virtual long GetConfigLayoutFlags() = 0;
    virtual long GetConfigLeft() = 0;
    virtual long GetConfigRight() = 0;
    virtual long GetConfigTop() = 0;
    virtual long GetConfigBottom() = 0;
};

interface IHorzLayoutParam : public ILayoutParam
{
    virtual void SetConfigWidth(long) = 0;
    virtual void SetConfigHeight(long) = 0;
    virtual void SetConfigLayoutFlags(long) = 0;

    virtual long GetConfigWidth() = 0;
    virtual long GetConfigHeight() = 0;
    virtual long GetConfigLayoutFlags() = 0;
};
interface IVertLayoutParam : public ILayoutParam
{
	virtual void SetConfigWidth(long) = 0;
	virtual void SetConfigHeight(long) = 0;
	virtual void SetConfigLayoutFlags(long) = 0;

	virtual long GetConfigWidth() = 0;
	virtual long GetConfigHeight() = 0;
	virtual long GetConfigLayoutFlags() = 0;
};

interface IGridLayoutParam : public ILayoutParam
{
    virtual void SetConfigWidth(int) = 0;
    virtual void SetConfigHeight(int) = 0;
    virtual void SetConfigLayoutFlags(int) = 0;
    virtual void SetRow(int) = 0;
    virtual void SetCol(int) = 0;
    virtual void SetRowSpan(int) = 0;
    virtual void SetColSpan(int) = 0;

    virtual int GetConfigWidth() = 0;
    virtual int GetConfigHeight() = 0;
    virtual int GetConfigLayoutFlags() = 0;
    virtual int GetRow() = 0;
    virtual int GetCol() = 0;
    virtual int GetRowSpan() = 0;
    virtual int GetColSpan() = 0;
};

interface ICardLayoutParam : public ILayoutParam
{

};
interface IAverageLayoutParam : public ILayoutParam
{

};

interface ILayout : public IRootInterface
{
    virtual void  Release() = 0;
    
    virtual LAYOUTTYPE  GetLayoutType() = 0;
    virtual SIZE  Measure() = 0;
    virtual void  Arrange(IObject* pObjToArrage=nullptr) = 0;
    virtual void  Serialize(SERIALIZEDATA* pData) = 0;
    virtual ILayoutParam*  CreateLayoutParam(IObject* pObj) = 0;  
    virtual void  ChildObjectVisibleChanged(IObject* pObj) = 0;
    virtual void  ChildObjectContentSizeChanged(IObject* pObj) = 0;
    virtual bool  IsDirty() = 0;
	virtual void  SetDirty(bool b) = 0;
};

interface ICanvasLayout : public ILayout
{

};

interface IHorzLayout : public ILayout
{
    virtual void  SetSpace(int n) PURE;
};
interface IVertLayout : public ILayout
{
	virtual void  SetSpace(int n) PURE;
};

interface IGridLayout : public ILayout
{

};

interface ICardLayout : public ILayout
{

};

interface IAverageLayout : public ILayout
{

};

}
#endif