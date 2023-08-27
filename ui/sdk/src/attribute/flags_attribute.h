#pragma once
#include "int_attribute.h"

//
// layout.align="left|top"
//
namespace ui {

class FlagsAttribute : public IntAttribute {
public:
  FlagsAttribute();
  ~FlagsAttribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *,
                      EditorAttributeFlag e) override;

public:
  FlagsAttribute *AddFlag(int, const char *);
  IFlagsAttribute *GetIFlagsAttribute();

private:
  IFlagsAttribute *m_pIFlagsAttribute;
};

AttributeBase *CreateFlagsAttribute();

} // namespace ui