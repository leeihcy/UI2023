#pragma once
#include "attributebase.h"

namespace ui {

class ColorAttribute : public AttributeBase {
public:
  ColorAttribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *p,
                      EditorAttributeFlag e) override;
  virtual bool IsDefaultValue() override;
  void SetBindValue(void *) override;
  void Bind(slot<void(Color)>&& setter, slot<Color()>&& getter);
  
public:
  IColorAttribute *GetIColorAttribute();
  void SetDefault(Color c);

private:
  Color parse_color(const char* text);
  
  void SetColor(Color c);
  Color GetColor();
  
private:
  std::unique_ptr<IColorAttribute> m_pIColorAttribute;

  bool m_default_configed = false;
  // 仅在m_default_configed=true是有效。
  Color m_default;

  Color *m_bind_value = nullptr; // raw_ptr

  signal<void(Color)> m_setter;
  signal<Color()> m_getter;
};

AttributeBase *CreateColorAttribute();

} // namespace ui
