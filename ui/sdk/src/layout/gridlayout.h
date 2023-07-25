#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

namespace ui
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

// ��񲼾ֵ�һ�л�һ�еĴ�С ����
class GridWH
{
public:
	GridWH();

	int          last;  // ���մ�С
	int          xml;   // ��xml�еĴ�С��������ֵ����*��������
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
    void  LoadGridWidth(const wchar_t* szText);
    const wchar_t*  SaveGridWidth();
    void  LoadGridHeight(const wchar_t* szText);
    const wchar_t*  SaveGridHeight();

private:
	std::vector<GridWH>   widths;
	std::vector<GridWH>   heights;
};

}