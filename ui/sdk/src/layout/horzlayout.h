#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

namespace ui {

class Object;

class HorzLayoutParam : public IHorzLayoutParam {
public:
  HorzLayoutParam(Object *pObj);
  ~HorzLayoutParam();

  Uuid UUID() override { return IHorzLayout::UUID(); }
  void Release() override { delete this; }

  virtual void UpdateByRect() override;
  virtual void Serialize(SerializeParam *pData) override;
  virtual Size CalcDesiredSize() override;

public:
  virtual int GetConfigWidth() override;
  virtual void SetConfigWidth(int) override;
  void LoadConfigWidth(const char *);
  const char *SaveConfigWidth();

  virtual int GetConfigHeight() override;
  virtual void SetConfigHeight(int) override;
  void LoadConfigHeight(const char *);
  const char *SaveConfigHeight();

  virtual void SetConfigLayoutFlags(int) override;
  virtual int GetConfigLayoutFlags() override;

  bool IsSizedByContent();
private:
  Object *m_pObj;
  
  int m_nConfigWidth;
  int m_nConfigHeight;
  int m_nConfigLayoutFlags;

  int m_eWidthType;
  int m_eHeightType;

  friend class HorzLayout;
};

class HorzLayout : public LayoutImpl<HorzLayout, IHorzLayout, HorzLayoutParam> {
public:
  HorzLayout();
  ~HorzLayout();

  virtual Size Measure() override;
  virtual void DoArrange(ArrangeParam* param) override;
  virtual void Serialize(SerializeParam *pData) override;
  // virtual void ChildObjectVisibleChanged(IObject *pObj) override;

  virtual void SetSpace(int n) override;

protected:
  void ArrangeObject_Left(Object *pChildObj, int &nLeftCustom,
                          int &nRightCustom, Size size);
  void ArrangeObject_Right(Object *pChildObj, int &nLeftCustom,
                           int &nRightCustom, Size size);

  void LoadGap(int);
  int SaveGap();

public:
  int m_nSpace; // 两个子对象之间间隙
};

} // namespace ui