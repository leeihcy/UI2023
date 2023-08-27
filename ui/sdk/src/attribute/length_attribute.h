#pragma once
#include "attribute_alias.h"
#include "attributebase.h"

namespace ui {

// 长度属性
// 在普通数值的基础上，添加了单位类型，如百分比。
class LengthAttribute : public AttributeBase {
public:
  LengthAttribute();
  ILengthAttribute *GetILengthAttribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *,
                      EditorAttributeFlag e) override;
  virtual bool IsDefaultValue() override;

  void SetBindValue(void *) override;
  void Bind(slot<void(Length)> &&s, slot<Length()> &&g);

public:
  LengthAttribute *SetDefault(Length l);
  LengthAttribute *SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e);

  Length GetLength();
  void SetLength(Length);

protected:
  Length *m_bind_value;
  Length m_default_value;

  signal<void(Length)> m_setter;
  signal<Length()> m_getter;

  LONGATTR_DPI_SCALE_TYPE m_dpi_scale_type;
private:
  std::unique_ptr<ILengthAttribute> m_pILengthAttribute;
};

AttributeBase *CreateLengthAttribute();

} // namespace ui