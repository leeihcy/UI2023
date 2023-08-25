#pragma once
#include "attribute_alias.h"
#include "attributebase.h"

namespace ui {

class IntAttribute : public AttributeBase {
public:
  IntAttribute();
  ~IntAttribute();
  IIntAttribute *GetIIntAttribute();

  virtual const wchar_t *Get() override;
  virtual void Set(const wchar_t *) override;
  virtual void Reset() override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *,
                      EditorAttributeFlag e) override;
  virtual bool IsDefaultValue() override;

  void SetBindValue(void *) override;
  // void  SetBindFuction(void* _this, void* _setter, void* _getter) override;
  void Bind(slot<void(int)> &&s, slot<int()> &&g);

public:
  IntAttribute *SetDefault(int l);
  IntAttribute *AddAlias(int, const wchar_t *);
  IntAttribute *SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e);

  int GetLong();
  void SetLong(int);

  uint GetAliasCount();
  int EnumAlias(pfnEnumAliasCallback, int, int);

protected:
  int *m_pBindValue;
  int m_lDefault;

  // void*   _this;
  // pfnLongSetter  _setter;
  // pfnLongGetter  _getter;
  signal<void(int)> m_setter;
  signal<int()> m_getter;

  IntAttributeAlias m_mapAlias;

  LONGATTR_DPI_SCALE_TYPE m_eDpiScaleType;

private:
  IIntAttribute *m_pIIntAttribute;
};

AttributeBase *CreateIntAttribute();

} // namespace ui