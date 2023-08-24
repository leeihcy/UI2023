#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

// 平均分配布局
namespace ui {

class Object;

class AverageLayoutParam : public IAverageLayoutParam {
public:
  AverageLayoutParam(Object*) {}
  Uuid UUID() override { return IAverageLayout::UUID(); }
  void Release() override { delete this; }
};

class AverageLayout : public LayoutImpl<AverageLayout, IAverageLayout, AverageLayoutParam> {
public:
  AverageLayout();
  ~AverageLayout();

  virtual Size Measure() override;
  virtual void DoArrange(ArrangeParam* param) override;
  virtual void Serialize(SerializeParam *) override;
  // virtual void ChildObjectVisibleChanged(IObject *pObj) override;

  void ArrangeObject_H(Object *pChildObj, Rect *prc);
  void ArrangeObject_V(Object *pChildObj, Rect *prc);

public:
  LAYOUT_AVERAGE_DIRECTION direction; // 堆栈排布的方向
  Object *m_pPanel;                   // 与该布局关联的panel
  int m_nGap;                         // 两个子对象之间间隙,
};

} // namespace ui