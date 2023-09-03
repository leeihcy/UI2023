#include "rect_attribute.h"
#include "attribute.h"
#include "include/inc.h"

namespace ui {
AttributeBase *CreateRectAttribute() { return new RectAttribute(); }

RectAttribute::RectAttribute() {
  m_pIRectAttribute = nullptr;
  m_pBindValue = nullptr;
  m_rcDefault.SetEmpty();
}

RectAttribute::~RectAttribute() { SAFE_DELETE(m_pIRectAttribute); }

void RectAttribute::SetBindValue(void *p) { m_pBindValue = (Rect *)p; }

void RectAttribute::Bind(slot<void(Rect *)> &&s, slot<void(Rect *)> &&g) {
  m_setter.connect(std::forward<slot<void(Rect *)>>(s));
  m_getter.connect(std::forward<slot<void(Rect *)>>(g));
}

RectAttribute *RectAttribute::SetDefault(Rect rc) {
  m_rcDefault.CopyFrom(rc);
  return this;
}

void RectAttribute::get(Rect *prc) {
  if (m_pBindValue) {
    prc->CopyFrom(*m_pBindValue);
  }
  else if (!m_getter.empty()) {
    m_getter.emit(prc);
  } else {
    prc->CopyFrom(m_rcDefault);
  }
}

void RectAttribute::set(Rect *prc) {
  if (!prc)
    prc = &m_rcDefault;

  if (m_pBindValue) {
    m_pBindValue->CopyFrom(*prc);
  }
  else if (!m_setter.empty()) {
    m_setter.emit(prc);
  }
}

const char *RectAttribute::Get() {
  Rect rc = {0};
  get(&rc);

  char *szText = GetTempBuffer(64);
  sprintf(szText, "%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);

  return szText;
}

void RectAttribute::Set(const char *szValue) {
  Rect rc = {0};
  if (util::TranslateRECT(szValue, &rc, XML_SEPARATOR)) {
    set(&rc);
  } else {
    set(nullptr);
  }
}

void RectAttribute::Reset() { set(&m_rcDefault); }

bool RectAttribute::IsDefaultValue() {
  Rect rc = {0};
  get(&rc);

  return m_rcDefault == rc ? true : false;
}

void RectAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                           EditorAttributeFlag e) {
  p->Rect2Editor(this, e);
}

IRectAttribute *RectAttribute::GetIRectAttribute() {
  if (!m_pIRectAttribute)
    m_pIRectAttribute = new IRectAttribute(this);
  return m_pIRectAttribute;
}
} // namespace ui