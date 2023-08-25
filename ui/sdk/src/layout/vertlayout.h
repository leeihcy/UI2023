#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

namespace ui {

class Object;
class VertLayout;

class VertLayoutParam : public IVertLayoutParam {
public:
  VertLayoutParam(Object *pObj);
  ~VertLayoutParam();

  Uuid UUID() override { return IVertLayout::UUID(); }
  void Release() override { delete this; }

  virtual void UpdateByRect() override;
  virtual void Serialize(SerializeParam *pData) override;
  virtual Size CalcDesiredSize() override;

public:
  virtual int GetConfigWidth() override;
  virtual void SetConfigWidth(int) override;
  void LoadConfigWidth(const wchar_t *);
  const wchar_t *SaveConfigWidth();

  virtual int GetConfigHeight() override;
  virtual void SetConfigHeight(int) override;
  void LoadConfigHeight(const wchar_t *);
  const wchar_t *SaveConfigHeight();

  virtual void SetConfigLayoutFlags(int) override;
  virtual int GetConfigLayoutFlags() override;

  bool IsSizedByContent();

protected:
  Object *m_pObj;
  
  int m_nConfigWidth;
  int m_nConfigHeight;
  int m_nConfigLayoutFlags;

  int m_eWidthType;
  int m_eHeightType;
  friend VertLayout;
};

class VertLayout : public LayoutImpl<VertLayout, IVertLayout, VertLayoutParam> {
public:
  VertLayout();
  ~VertLayout();

  virtual Size Measure() override;
  virtual void DoArrange(ArrangeParam* param) override;
  virtual void Serialize(SerializeParam *pData) override;
  // virtual void ChildObjectVisibleChanged(IObject *pObj) override;
  virtual void SetSpace(int n) override;

protected:
  void LoadGap(int);
  int SaveGap();

public:
  int m_nSpace; // 两个子对象之间间隙
};

} // namespace ui