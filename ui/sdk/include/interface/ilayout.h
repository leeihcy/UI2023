#ifndef _UI_SDK_INCLUDE_INTERFACE_ILAYOUT_H_
#define _UI_SDK_INCLUDE_INTERFACE_ILAYOUT_H_

#include "sdk/include/macro/uidefine.h"
#include "sdk/include/util/rect.h"
#include "sdk/include/common/uuid/uuid.h"
#include "sdk/include/macro/xmldefine.h"

namespace ui {
class Object;
struct IObject;
struct IMapAttribute;

struct ILayoutParam {
  // 取代layout type
  virtual Uuid UUID() = 0;

  virtual void Release() = 0;

  //  在布局前，计算控件期望的大小
  virtual Size CalcDesiredSize() { return {0, 0}; }

  // 由当前坐RectT反推出布局参数
  virtual void UpdateByRect() {};

  // 序列化
  virtual void Serialize(SERIALIZEDATA *pData) {};

  // 自己是自适应大小，还是固定大小，用于优化updatelayout
  virtual bool IsSizedByContent() = 0;
};

struct ICanvasLayoutParam : public ILayoutParam {
  virtual void SetConfigWidth(long) = 0;
  virtual void SetConfigHeight(long) = 0;
  virtual void SetConfigLayoutFlags(long) = 0;
  virtual void SetConfigLeft(long) = 0;
  virtual void SetConfigRight(long) = 0;
  virtual void SetConfigTop(long) = 0;
  virtual void SetConfigBottom(long) = 0;

  virtual long GetConfigWidth() = 0;
  virtual long GetConfigHeight() = 0;
  virtual long GetConfigLayoutFlags() = 0;
  virtual long GetConfigLeft() = 0;
  virtual long GetConfigRight() = 0;
  virtual long GetConfigTop() = 0;
  virtual long GetConfigBottom() = 0;
};

struct IHorzLayoutParam : public ILayoutParam {
  virtual void SetConfigWidth(long) = 0;
  virtual void SetConfigHeight(long) = 0;
  virtual void SetConfigLayoutFlags(long) = 0;

  virtual long GetConfigWidth() = 0;
  virtual long GetConfigHeight() = 0;
  virtual long GetConfigLayoutFlags() = 0;
};
struct IVertLayoutParam : public ILayoutParam {
  virtual void SetConfigWidth(long) = 0;
  virtual void SetConfigHeight(long) = 0;
  virtual void SetConfigLayoutFlags(long) = 0;

  virtual long GetConfigWidth() = 0;
  virtual long GetConfigHeight() = 0;
  virtual long GetConfigLayoutFlags() = 0;
};

struct IGridLayoutParam : public ILayoutParam {
  virtual void SetConfigWidth(int) = 0;
  virtual void SetConfigHeight(int) = 0;
  virtual void SetConfigLayoutFlags(int) = 0;
  virtual void SetRow(int) = 0;
  virtual void SetCol(int) = 0;
  virtual void SetRowSpan(int) = 0;
  virtual void SetColSpan(int) = 0;

  virtual int GetConfigWidth() = 0;
  virtual int GetConfigHeight() = 0;
  virtual int GetConfigLayoutFlags() = 0;
  virtual int GetRow() = 0;
  virtual int GetCol() = 0;
  virtual int GetRowSpan() = 0;
  virtual int GetColSpan() = 0;
};

struct ICardLayoutParam : public ILayoutParam {
};
struct IAverageLayoutParam : public ILayoutParam {
};

struct ILayout {
  virtual void Release() = 0;

  virtual Size Measure() = 0;
  virtual void Arrange(IObject *pObjToArrage = nullptr) = 0;
  virtual void Serialize(SERIALIZEDATA *pData) = 0;
  virtual ILayoutParam *CreateLayoutParam(IObject *pObj) = 0;
  virtual void ChildObjectVisibleChanged(IObject *pObj) = 0;
  virtual void ChildObjectContentSizeChanged(IObject *pObj) = 0;

  virtual bool IsDirty() = 0;
  virtual void SetDirty(bool b) = 0;
};

struct ICanvasLayout : public ILayout {
  static Uuid UUID() { static Uuid s("0fdcdf64-3b72-11ee-8df4-f45c89b0174f"); return s; }
};

struct IHorzLayout : public ILayout {
  static Uuid UUID() { static Uuid s("e3b3b61e-3b72-11ee-bede-f45c89b0174f"); return s; }

  virtual void SetSpace(int n) = 0;
};
struct IVertLayout : public ILayout {
  static Uuid UUID() { static Uuid s("f47c4182-3b72-11ee-a57f-f45c89b0174f"); return s; }

  virtual void SetSpace(int n) = 0;
};

struct IGridLayout : public ILayout {
  static Uuid UUID() { static Uuid s("d97f89a2-3b72-11ee-afd6-f45c89b0174f"); return s; }
};

struct ICardLayout : public ILayout {
  static Uuid UUID() { static Uuid s("18301ba4-3b72-11ee-82f6-f45c89b0174f"); return s; }
};

struct IAverageLayout : public ILayout {
  static Uuid UUID() { static Uuid s("02104c9a-3b72-11ee-94bb-f45c89b0174f"); return s; }
};


typedef bool (*pfnUICreateLayoutPtr)(IObject *pObject, ILayout **ppLayout);

} // namespace ui
#endif