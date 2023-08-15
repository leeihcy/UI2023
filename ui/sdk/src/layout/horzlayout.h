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
  virtual void Serialize(SERIALIZEDATA *pData) override;
  virtual Size CalcDesiredSize() override;
  virtual bool IsSizedByContent() override;

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

private:
  Object *m_pObj;
  
  long m_nConfigWidth;
  long m_nConfigHeight;
  long m_nConfigLayoutFlags;

  long m_eWidthType;
  long m_eHeightType;

  friend class HorzLayout;
};

class HorzLayout : public LayoutImpl<HorzLayout, IHorzLayout, HorzLayoutParam> {
public:
  HorzLayout();
  ~HorzLayout();

  virtual Size Measure() override;
  virtual void DoArrage(IObject *pObjToArrage = nullptr) override;
  virtual void Serialize(SERIALIZEDATA *pData) override;
  virtual void ChildObjectVisibleChanged(IObject *pObj) override;
  virtual void ChildObjectContentSizeChanged(IObject *pObj) override;

  virtual void SetSpace(int n) override;

protected:
  void ArrangeObject_Left(Object *pChildObj, int &nLeftCustom,
                          int &nRightCustom, Size size);
  void ArrangeObject_Right(Object *pChildObj, int &nLeftCustom,
                           int &nRightCustom, Size size);

  void LoadGap(long);
  long SaveGap();

public:
  long m_nSpace; // 两个子对象之间间隙
};

} // namespace ui