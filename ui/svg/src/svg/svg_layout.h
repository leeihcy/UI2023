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
  void Serialize(SERIALIZEDATA *pData) override {};

  // 自己是自适应大小，还是固定大小，用于优化updatelayout
  bool IsSizedByContent() override { return false; }

private:
  ui::IObject* m_obj = nullptr;
};

class SvgLayout : public ui::ILayout
{
public:
    static Uuid UUID() { static Uuid s("42423748-3c34-11ee-ba0b-f45c89b0174f"); return s; }

  SvgLayout();
  ~SvgLayout();

  void Release() override { delete this; }

  Size Measure() override { Size s = {0, 0}; return s; }
  void Arrange(IObject *pObjToArrage = nullptr) override;
  void Serialize(SERIALIZEDATA *pData) override { }
  ILayoutParam *CreateLayoutParam(IObject *pObj) override;
  void ChildObjectVisibleChanged(IObject *pObj) override;
  void ChildObjectContentSizeChanged(IObject *pObj) override;

  bool IsDirty() override;
  void SetDirty(bool b) override;

private:
  bool m_dirty = true;

};

}}

#endif