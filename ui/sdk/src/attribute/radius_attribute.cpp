#include "radius_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "include/util/rect.h"
#include <memory>

namespace ui {
AttributeBase *CreateRadiusAttribute() { return new RadiusAttribute(); }

RadiusAttribute::RadiusAttribute() {
  m_pIRadiusAttribute = nullptr;
  m_pBindValue = nullptr;
  m_rcDefault.SetAll(0);
}

void RadiusAttribute::SetBindValue(void *p) { m_pBindValue = (CornerRadius *)p; }

void RadiusAttribute::Bind(slot<void(CornerRadius *)> &&s, slot<void(CornerRadius *)> &&g) {
  m_setter.connect(std::forward<slot<void(CornerRadius *)>>(s));
  m_getter.connect(std::forward<slot<void(CornerRadius *)>>(g));
}

RadiusAttribute *RadiusAttribute::SetDefault(CornerRadius rc) {
  m_rcDefault = rc;

  return this;
}

void RadiusAttribute::get(CornerRadius *prc) {
  if (m_pBindValue) {
    *prc = *m_pBindValue;
  }
  else if (!m_getter.empty()) {
    m_getter.emit(prc);
  } else {
    *prc = m_rcDefault;
  }
}

void RadiusAttribute::set(CornerRadius *prc) {
  if (!prc)
    prc = &m_rcDefault;

  if (m_pBindValue) {
    *m_pBindValue = *prc;
  }
  else if (!m_setter.empty()) {
    m_setter.emit(prc);
  }
}

const char *RadiusAttribute::Get() {
  CornerRadius rc;
  get(&rc);

  char *szText = GetTempBuffer(64);
  snprintf(szText, 64, "%d,%d,%d,%d", rc.top_left, rc.top_right, rc.bottom_right, rc.bottom_left);

  return szText;
}

void RadiusAttribute::Set(const char *szValue) {
  Rect rc = {0};
  if (util::TranslateRECT(szValue, &rc, XML_SEPARATOR)) {
    CornerRadius r;
    r.top_left = rc.left;
    r.top_right = rc.top;
    r.bottom_right = rc.right;
    r.bottom_left = rc.bottom;
    set(&r);
    return;
  }
  
  set(nullptr);
}

void RadiusAttribute::Reset() { set(&m_rcDefault); }

bool RadiusAttribute::IsDefaultValue() {
  CornerRadius rc;
  get(&rc);

  return m_rcDefault == rc;
}

void RadiusAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                           EditorAttributeFlag e) {
  p->CornerRadius2Editor(this, e);
}

IRadiusAttribute *RadiusAttribute::GetIRadiusAttribute() {
  if (!m_pIRadiusAttribute)
    m_pIRadiusAttribute = std::make_unique<IRadiusAttribute>(this);
  return m_pIRadiusAttribute.get();
}
} // namespace ui