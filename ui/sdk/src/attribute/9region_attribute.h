#pragma once
#include "attributebase.h"
#include "sdk/include/util/struct.h"

namespace ui {

class Region9Attribute : public AttributeBase {
public:
  Region9Attribute();
  ~Region9Attribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *p,
                      EditorAttributeFlag e) override;
  virtual bool IsDefaultValue() override;
  void SetBindValue(void *) override;
  // void  SetBindFuction(void* _this, void* _setter, void* _getter) override;
  void Bind(slot<void(C9Region *)> s, slot<void(C9Region *)> g);

public:
  Region9Attribute *SetDefault(C9Region *);
  IRegion9Attribute *GetIRegion9Attribute();

private:
  void get(C9Region *);
  void set(C9Region *);

private:
  IRegion9Attribute *m_pIRegion9Attribute;

  C9Region *m_pBindValue;
  C9Region m_rDefault;

  signal<void(C9Region *)> m_setter;
  signal<void(C9Region *)> m_getter;

  // void*   _this;
  // pfn9RegionSetter  _setter;
  // pfn9RegionGetter  _getter;
};

AttributeBase *CreateRegion9Attribute();

} // namespace ui