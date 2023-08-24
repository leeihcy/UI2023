#ifndef _UI_SVG_SRC_SVG_SVG_LAYOUT_H_
#define _UI_SVG_SRC_SVG_SVG_LAYOUT_H_

#include "sdk/include/interface/ilayout.h"

namespace ui { namespace svg {

class SvgLayoutParam : public ui::ILayoutParam {
public:
  SvgLayoutParam(ui::IObject* p);
  void Load();

public:
  // 取代layout type
  Uuid UUID() override;

  void Release() override { delete this; }

  //  在布局前，计算控件期望的大小
  Size CalcDesiredSize() override { return {0, 0}; }

  // 由当前坐RectT反推出布局参数
  void UpdateByRect() override {};

  // 序列化
  void Serialize(SerializeParam *pData) override;

public:
  ui::IObject* m_obj = nullptr;

  int m_x = 0;
  int m_y = 0;
  int m_width = 100;
  int m_height = 100;
};

class SvgLayout : public ui::ILayout
{
public:
  static Uuid UUID() { static Uuid s("42423748-3c34-11ee-ba0b-f45c89b0174f"); return s; }

  SvgLayout(IObject* bind_object);
  ~SvgLayout();

  void Release() override { delete this; }

  Size Measure() override { Size s = {0, 0}; return s; }
  void Arrange(ArrangeParam* param) override;
  void Serialize(SerializeParam *pData) override { }
  ILayoutParam *CreateLayoutParam(IObject *pObj) override;

  bool IsDirty() override;
  void SetDirty(bool b) override;

private:
  bool m_dirty = true;
  ui::IObject* m_bind_object = nullptr;
};

}}

#endif