#pragma once
#include "Inc\Interface\ilayout.h"
#include "Src\Helper\layout\layout.h"

namespace UI
{
class Object;


class GridLayoutParam : public LayoutParamImpl<IGridLayoutParam>
{
public:
    GridLayoutParam();
    ~GridLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_GRID; }
    virtual SIZE  CalcDesiredSize();
    virtual bool  IsSizedByContent();

public:
    DECLARE_INT_SETGET(ConfigWidth)
    DECLARE_INT_SETGET(ConfigHeight)
    DECLARE_INT_SETGET(ConfigLayoutFlags)
    DECLARE_INT_SETGET(Row)
    DECLARE_INT_SETGET(Col)
    DECLARE_INT_SETGET(RowSpan)
    DECLARE_INT_SETGET(ColSpan)

private:
    Object*  m_pObj;

    long  m_nConfigWidth;  
    long  m_nConfigHeight; 
    long  m_nConfigLayoutFlags;

    long  m_nRow;
    long  m_nCol;
    long  m_nRowSpan;
    long  m_nColSpan;
};


enum GridWHType
{
	GWHT_AUTO,
	GWHT_ASTERISK,  // *
	GWHT_VALUE
};

// 表格布局的一行或一列的大小 描述
class GridWH
{
public:
	GridWH();

	int          last;  // 最终大小
	int          xml;   // 在xml中的大小描述，该值仅对*类型有用
	GridWHType   type;  // auto, *, value
};

class GridLayout : public LayoutImpl<GridLayout, IGridLayout, GridLayoutParam, LAYOUT_TYPE_GRID>
{
public:
    GridLayout();
	~GridLayout();

    virtual void  Serialize(SERIALIZEDATA*) override;
    virtual SIZE  Measure() override;
    virtual void  DoArrage(IObject* pObjToArrage=nullptr) override;
    virtual void  ChildObjectVisibleChanged(IObject* pObj) override;

	int  getColPos(unsigned int nCol);    
	int  getRowPos(unsigned int nRow);

    GridWH*  GetWidth(unsigned int nIndex);
    GridWH*  GetHeight(unsigned int nIndex);

private:
    void  LoadGridWidth(LPCTSTR szText);
    LPCTSTR  SaveGridWidth();
    void  LoadGridHeight(LPCTSTR szText);
    LPCTSTR  SaveGridHeight();

private:
	vector<GridWH>   widths;
	vector<GridWH>   heights;
};

}