#pragma once
#include "include/interface/ilayout.h"
#include "src/layout/layout.h"

namespace ui {

class Object;

class CanvasLayoutParam : public ICanvasLayoutParam {
public:
  CanvasLayoutParam(Object *pObj);
  ~CanvasLayoutParam();

  Uuid UUID() override { return ICanvasLayout::UUID(); }
  void Release() override { delete this; }

  virtual void UpdateByRect() override;
  virtual void Serialize(SerializeParam *pData) override;
  // virtual LAYOUTTYPE  GetLayoutType() override { return LAYOUT_TYPE_CANVAS; }
  virtual Size CalcDesiredSize() override;

  static int ParseAlignAttr(const char *);
  Object* GetObject() { return m_pObj; }
  
public:
  virtual int GetConfigLeft() override;
  virtual void SetConfigLeft(int) override;
  void LoadConfigLeft(int);
  int SaveConfigLeft();

  virtual int GetConfigTop() override;
  virtual void SetConfigTop(int) override;
  void LoadConfigTop(int);
  int SaveConfigTop();

  virtual int GetConfigRight() override;
  virtual void SetConfigRight(int) override;
  void LoadConfigRight(int);
  int SaveConfigRight();

  virtual int GetConfigBottom() override;
  virtual void SetConfigBottom(int) override;
  void LoadConfigBottom(int);
  int SaveConfigBottom();

  virtual int GetConfigWidth() override;
  virtual void SetConfigWidth(int) override;
  void LoadConfigWidth(int);
  int SaveConfigWidth();

  virtual int GetConfigHeight() override;
  virtual void SetConfigHeight(int) override;
  void LoadConfigHeight(int);
  int SaveConfigHeight();

  virtual void SetConfigLayoutFlags(int) override;
  virtual int GetConfigLayoutFlags() override;

private:
  Object *m_pObj;

  // 对象的宽度，可取值： 数值 | "auto" .
  // （对于window对象，width
  // 是指client区域的大小，不是整个窗口的大小；width包括padding，但不包括margin）
  int m_nConfigWidth; 
                      
  // 对象的高度，可取值： 数值 | "auto" .              
  // （对于window对象，height是指client区域的大小，不是整个窗口的大小；height包括padding，但不包括margin）      
  int m_nConfigHeight; 
                       
  // 在这里需要说明，对象的最终占用的宽度= margin.left + width + margin.right
  // 也就是说这里的width = padding.left + padding.right + content.width
  int m_nConfigLeft;
  int m_nConfigRight;
  int m_nConfigTop;
  int m_nConfigBottom;
  int m_nConfigLayoutFlags;
};

//
//	指定布局离边缘的位置
//
class CanvasLayout : public LayoutImpl<CanvasLayout, ICanvasLayout, CanvasLayoutParam> {
public:
  virtual void Serialize(SerializeParam *) override{};
  virtual Size Measure() override;
  virtual void DoArrange(ArrangeParam& param) override;
  // virtual void ChildObjectVisibleChanged(IObject *pObj) override;

public:
  void ArrangeObject(Object *pChild, const int &nWidth,
                            const int &nHeight, float scale);
};

} // namespace ui
