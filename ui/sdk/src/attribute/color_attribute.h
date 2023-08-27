#pragma once
#include "attributebase.h"

namespace ui {

class ColorAttribute : public AttributeBase {
public:
  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *p,
                      EditorAttributeFlag e) override;
  virtual bool IsDefaultValue() override;
  void SetBindValue(void *) override;
  // void SetBindValue2(void **);

public:
  IColorAttribute *GetIColorAttribute();

private:
  Color parse_color(const char* text);

private:
  std::unique_ptr<IColorAttribute> m_pIColorAttribute;

  // Color **m_ppBindValue;
  Color *m_bind_value = nullptr; // raw_ptr
};

AttributeBase *CreateColorAttribute();

} // namespace ui
