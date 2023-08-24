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
  virtual long GetConfigWidth() override;
  virtual void SetConfigWidth(long) override;
  void LoadConfigWidth(const wchar_t *);
  const wchar_t *SaveConfigWidth();

  virtual long GetConfigHeight() override;
  virtual void SetConfigHeight(long) override;
  void LoadConfigHeight(const wchar_t *);
  const wchar_t *SaveConfigHeight();

  virtual void SetConfigLayoutFlags(long) override;
  virtual long GetConfigLayoutFlags() override;

  bool IsSizedByContent();

protected:
  Object *m_pObj;
  
  long m_nConfigWidth;
  long m_nConfigHeight;
  long m_nConfigLayoutFlags;

  long m_eWidthType;
  long m_eHeightType;
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
  void LoadGap(long);
  long SaveGap();

public:
  long m_nSpace; // 两个子对象之间间隙
};

} // namespace ui