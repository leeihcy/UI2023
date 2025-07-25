#ifndef _UI_SDK_SRC_OBJECT_LAYOUT_LAYOUT_OBJECT_H_
#define _UI_SDK_SRC_OBJECT_LAYOUT_LAYOUT_OBJECT_H_

#include "include/util/rect.h"

namespace ui {
class Object;
struct ILayoutParam;
struct ILayout;
struct SerializeParam;

class LayoutObject {
public:
  LayoutObject(Object &obj);
  virtual ~LayoutObject();

  int GetMaxWidth();
  int GetMaxHeight();
  void SetMaxWidth(int);
  void SetMaxHeight(int);
  int GetMinWidth();
  int GetMinHeight();
  void SetMinWidth(int);
  void SetMinHeight(int);
  
  // Canvas布局专用接口
  int GetConfigWidth();
  int GetConfigHeight();
  int GetConfigLayoutFlags();
  int GetConfigLeft();
  int GetConfigRight();
  int GetConfigTop();
  int GetConfigBottom();
  void SetConfigWidth(int n);
  void SetConfigHeight(int n);
  void SetConfigLayoutFlags(int n);
  void SetConfigLeft(int n);
  void SetConfigRight(int n);
  void SetConfigTop(int n);
  void SetConfigBottom(int n);

  int GetZorder();
  void SetZorderDirect(int z);
  void position_in_tree_changed();

  ILayoutParam *GetLayoutParam();
  ILayoutParam *GetSafeLayoutParam();
  ILayout *GetLayout();
  void DestroyLayoutParam();
  void CreateLayoutParam();
  void SetLayoutParam(ILayoutParam *p);

  Size GetDesiredSize();
  void UpdateLayoutPos();
  void onSerialize(SerializeParam *pData);

protected:
  Object &m_object;

// ui::unique_ptr<ILayoutParam> m_pLayoutParam; // 布局参数。由Object负责释放
  ILayoutParam* m_pLayoutParam = nullptr;

  int m_lzOrder; // 控件z序，用于实现控件重叠时的刷新判断依据

  // 控件的最大尺寸限制
  int m_nMaxWidth;
  int m_nMaxHeight;
  int m_lMinWidth;
  int m_lMinHeight;
};

} // namespace ui

#endif