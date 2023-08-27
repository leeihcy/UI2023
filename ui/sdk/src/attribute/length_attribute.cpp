#include "attribute.h"
#include "include/inc.h"
#include "int_attribute.h"
#include "src/util/DPI/dpihelper.h"

namespace ui {
AttributeBase *CreateLengthAttribute() { return new LengthAttribute(); }

LengthAttribute::LengthAttribute() {
  m_bind_value = nullptr;
  m_default_value = {0, Length::Unit::Number};

  m_dpi_scale_type = LONGATTR_DPI_SCALE_DISABLE;
}

ILengthAttribute *LengthAttribute::GetILengthAttribute() {
  if (!m_pILengthAttribute)
    m_pILengthAttribute = std::make_unique<ILengthAttribute>(this);

  return m_pILengthAttribute.get();
}

void LengthAttribute::SetBindValue(void *p) { m_bind_value = (Length *)p; }

void LengthAttribute::Bind(slot<void(Length)> &&s, slot<Length()> &&g) {
  m_setter.connect(std::forward<slot<void(Length)>>(s));
  m_getter.connect(std::forward<slot<Length()>>(g));
}

LengthAttribute *LengthAttribute::SetDefault(Length l) {
  m_default_value = l;
  return this;
}

Length LengthAttribute::GetLength() {
  Length lReturn = m_default_value;
  if (m_bind_value) {
    lReturn = (*m_bind_value);
  } else if (!m_getter.empty()) {
    lReturn = m_getter.emit();
  } else {
    lReturn = m_default_value;
  }

  switch (m_dpi_scale_type) {
  case LONGATTR_DPI_SCALE_ENABLE:
    lReturn.value = ui::RestoreByDpi(lReturn.value);
    break;

  case LONGATTR_DPI_SCALE_GRATETHAN_0:
    lReturn.value = ui::RestoreByDpi_if_gt0(lReturn.value);
    break;

  default:
    break;
  }

  return lReturn;
}

void LengthAttribute::SetLength(Length l) {
  switch (m_dpi_scale_type) {
  case LONGATTR_DPI_SCALE_ENABLE:
    l.value = ui::ScaleByDpi(l.value);
    break;

  case LONGATTR_DPI_SCALE_GRATETHAN_0:
    l.value = ui::ScaleByDpi_if_gt0(l.value);
    break;
  default:
    break;
  }

  if (m_bind_value) {
    (*m_bind_value) = l;
  } else if (!m_setter.empty()) {
    m_setter.emit(l);
  }
}
const char *LengthAttribute::Get() {
  Length lValue = GetLength();

  char *szText = GetTempBuffer(64);
  switch (lValue.unit) {
  case Length::Unit::Percentage:
    sprintf(szText, "%d%%", lValue.value);
    break;
  default:
    sprintf(szText, "%d", lValue.value);
    break;
  }

  return szText;
}

void LengthAttribute::Set(const char *szValue) {
  if (!szValue) {
    return;
  }
  int length = strlen(szValue);
  if (length < 1) {
    return;
  }

  Length lValue = m_default_value;
  lValue.value = atoi(szValue);

  if (szValue[length-1] == '%') {
    lValue.unit = Length::Unit::Percentage;
  } else {
    lValue.unit = Length::Unit::Number;
  }
  SetLength(lValue);
}

void LengthAttribute::Reset() { SetLength(m_default_value); }

bool LengthAttribute::IsDefaultValue() {
  return GetLength() == m_default_value;
}

void LengthAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                             EditorAttributeFlag e) {
  UIASSERT(0);
  // p->Long2Editor(this, e);
}

LengthAttribute *LengthAttribute::SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e) {
  m_dpi_scale_type = e;
  return this;
}

} // namespace ui