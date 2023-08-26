#pragma once
#include "attributebase.h"

namespace ui {

class RectAttribute : public AttributeBase {
public:
  RectAttribute();
  ~RectAttribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *p,
                      EditorAttributeFlag e) override;
  virtual bool IsDefaultValue() override;
  void SetBindValue(void *) override;
  // void  SetBindFuction(void* _this, void* _setter, void* _getter) override;
  void Bind(slot<void(Rect *)> &&s, slot<void(Rect *)> &&g);

public:
  RectAttribute *SetDefault(Rect *);
  IRectAttribute *GetIRectAttribute();

private:
  void get(Rect *);
  void set(Rect *);

private:
  IRectAttribute *m_pIRectAttribute;

  Rect *m_pBindValue;
  Rect m_rcDefault;

  // void*   _this;
  // pfnRectSetter  _setter;
  // pfnRectGetter  _getter;
  signal<void(Rect *)> m_setter;
  signal<void(Rect *)> m_getter;
};

AttributeBase *CreateRectAttribute();

} // namespace ui