#pragma once

namespace UI
{

class Cell;

//
// 网格布局面板
//
class GridPanel : public Panel
{
public:
	GridPanel(void);
	~GridPanel(void);

	UI_DECLARE_OBJECT( GridPanel, OBJ_PANEL )

	UI_BEGIN_MSG_MAP
		UI_ON_WM_PAINT( OnPaint )
		UI_CHAIN_MSG_MAP(Panel)
	UI_END_MSG_MAP

	// 公共方法
public:
	bool             Initialize( );
	bool             AddRow();
	bool             AddCol();

	// 私有方法
private:
	void             UpdateRect();

private:
	Cell*            m_pData;

	vector<int>      m_vWidths;
	vector<int>      m_vHeights;
	vector<String>   m_vWidthInXml;
	vector<String>   m_vHeightInXml;

	bool             m_bDrawBorder;           // 是否绘制边框
	RECT             m_rcMargin;               // 留白间隔
	

	// 消息处理
protected:
	void             OnPaint( HDC hDC );
};


// grid 的单元格定义
class Cell
{
public:
	Cell();

public:
	int       m_nRow;         // 单元格的行号
	int       m_nCol;         // 单元格的列号
	bool      m_bCombined;    // 该单元格是否被合并了

	Object*   m_pObject;      // 该单元格的内容
	Cell*     m_pObjCombined; // 如果该单元格是被合并的，那么是哪个单元格合并了自己


	// 对表格进行遍历
	Cell*     m_pLeft;        
	Cell*     m_pRight;
	Cell*     m_pTop;
	Cell*     m_pBottom;
};

}
