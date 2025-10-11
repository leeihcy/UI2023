#pragma once
#include "attributebase.h"
#include <memory>

namespace ui {

class RadiusAttribute : public AttributeBase {
public:
  RadiusAttribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *p,
                      EditorAttributeFlag e) override;
  virtual bool IsDefaultValue() override;
  void SetBindValue(void *) override;
  void Bind(slot<void(CornerRadius *)> &&s, slot<void(CornerRadius *)> &&g);

public:
  RadiusAttribute *SetDefault(CornerRadius);
  IRadiusAttribute *GetIRadiusAttribute();

private:
  void get(CornerRadius *);
  void set(CornerRadius *);

private:
  std::unique_ptr<IRadiusAttribute> m_pIRadiusAttribute;

  CornerRadius *m_pBindValue;
  CornerRadius m_rcDefault;

  signal<void(CornerRadius *)> m_setter;
  signal<void(CornerRadius *)> m_getter;
};

AttributeBase *CreateRadiusAttribute();

} // namespace ui