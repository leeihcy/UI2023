#ifndef _UI_SDK_SRC_OBJECT_LAYOUT_LAYOUT_OBJECT_H_
#define _UI_SDK_SRC_OBJECT_LAYOUT_LAYOUT_OBJECT_H_

#include "include/util/rect.h"
#include <memory>

namespace ui {
class Object;
struct ILayoutParam;
struct ILayout;
struct SerializeParam;

class ObjectLayout {
public:
  ObjectLayout();
  ~ObjectLayout();

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

  void onZOrderChanged(int z_order);

  void position_in_tree_changed();

  ILayoutParam *GetLayoutParam();
  ILayoutParam *GetSafeLayoutParam();
  ILayout *GetLayout();
  void DestroyLayoutParam();
  void CreateLayoutParam();
  void SetLayoutParam(ILayoutParam *p);

  Size GetLayoutDesiredSize();
  void UpdateLayoutPos();
  void SerializeLayout(SerializeParam *pData);

protected:
  ILayoutParam* m_pLayoutParam = nullptr;
  // std::unique_ptr<ILayoutParam> m_layout_param;
  // std::unique_ptr<ILayout> m_layout;
};

} // namespace ui

#endif