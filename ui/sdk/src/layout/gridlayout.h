#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

namespace ui {
class Object;

class GridLayoutParam : public IGridLayoutParam {
public:
  GridLayoutParam(Object*);
  ~GridLayoutParam();
  
  Uuid UUID() override { return IGridLayout::UUID(); }
  void Release() override { delete this; }

  void UpdateByRect() override;
  void Serialize(SerializeParam *pData) override;
  Size CalcDesiredSize() override;

public:

#define DECLARE_INT_SETGET(xxx) \
	int  Get##xxx() override { return m_n##xxx; } \
	void Set##xxx( int n ) override { m_n##xxx = n; }

  DECLARE_INT_SETGET(ConfigWidth)
  DECLARE_INT_SETGET(ConfigHeight)
  DECLARE_INT_SETGET(ConfigLayoutFlags)
  DECLARE_INT_SETGET(Row)
  DECLARE_INT_SETGET(Col)
  DECLARE_INT_SETGET(RowSpan)
  DECLARE_INT_SETGET(ColSpan)

private:
  Object *m_pObj;

  int m_nConfigWidth;
  int m_nConfigHeight;
  int m_nConfigLayoutFlags;

  int m_nRow;
  int m_nCol;
  int m_nRowSpan;
  int m_nColSpan;
};

enum GridWHType {
  GWHT_AUTO,
  GWHT_ASTERISK, // *
  GWHT_VALUE
};

// 表格布局的一行或一列的大小 描述
class GridWH {
public:
  GridWH();

  int last;        // 最终大小
  int xml;         // 在xml中的大小描述，该值仅对*类型有用
  GridWHType type; // auto, *, value
};

class GridLayout : public LayoutImpl<GridLayout, IGridLayout, GridLayoutParam> {
public:
  GridLayout();
  ~GridLayout();

  virtual void Serialize(SerializeParam *) override;
  virtual Size Measure() override;
  virtual void DoArrange(ArrangeParam& param) override;
  // virtual void ChildObjectVisibleChanged(IObject *pObj) override;

  int getColPos(unsigned int nCol);
  int getRowPos(unsigned int nRow);

  GridWH *GetWidth(unsigned int nIndex);
  GridWH *GetHeight(unsigned int nIndex);

private:
  void LoadGridWidth(const char *szText);
  const char *SaveGridWidth();
  void LoadGridHeight(const char *szText);
  const char *SaveGridHeight();

private:
  std::vector<GridWH> widths;
  std::vector<GridWH> heights;
};

} // namespace ui