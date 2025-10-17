#include "textrenderbase_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/object/object.h"

namespace ui {
AttributeBase *CreateTextRenderBaseAttribute() {
  return new TextRenderBaseAttribute();
}

TextRenderBaseAttribute::TextRenderBaseAttribute() {
  ReloadOnChanged();

  m_pITextRenderBaseAttribute = nullptr;
  m_ppBindValue = nullptr;
  m_pObject = nullptr;
  //     _this = nullptr;
  //     _setter = nullptr;
  //     _getter = nullptr;
}

TextRenderBaseAttribute::~TextRenderBaseAttribute() {
  SAFE_DELETE(m_pITextRenderBaseAttribute);
}

void TextRenderBaseAttribute::SetBindValue(void *p) {
  m_ppBindValue = (std::shared_ptr<ITextRenderBase>*)p;
}

void TextRenderBaseAttribute::Set(const char *szType) {
  if (!m_ppBindValue)
    return;

  if (!m_pObject) {
    UIASSERT(0);
    return;
  }

  Reset();
  *m_ppBindValue = GetUIApplication()->GetIUIApplication()->CreateTextRenderBaseByName(
      szType, m_pObject->GetIObject());
}

void TextRenderBaseAttribute::Reset() {
  if (m_ppBindValue) {
    if (*m_ppBindValue) {
      *m_ppBindValue = nullptr;
    }
  } else {
  }
}

const char *TextRenderBaseAttribute::Get() {
  if (!m_ppBindValue)
    return nullptr;

  if (!*m_ppBindValue)
    return nullptr;

  TEXTRENDER_TYPE eType = (*m_ppBindValue)->GetType();
  TextRenderFactory &factory = GetUIApplication()->GetTextRenderFactroy();

  const char *szType = factory.GetTextRenderBaseName(eType);
  return szType;
}

bool TextRenderBaseAttribute::IsDefaultValue() {
  if (m_ppBindValue) {
    if (*m_ppBindValue == nullptr)
      return true;

    //         if ((*m_ppBindValue)->GetType() == TEXTRENDER_TYPE_SIMPLE)
    //             return true;
  }

  return false;
}

void TextRenderBaseAttribute::do_child_action(SerializeParam *pData) {
  // 子属性序列化
  if (m_ppBindValue && *m_ppBindValue) {
    const char *szOldPrefix = pData->szPrefix;
    const char *szOldParentKey = pData->szParentKey;

    pData->szPrefix = m_strPrefix.c_str();
    pData->szParentKey = GetKey();

    (*m_ppBindValue)->Serialize(pData);

    pData->szPrefix = szOldPrefix;
    pData->szParentKey = szOldParentKey;
  }
}

void TextRenderBaseAttribute::Load(SerializeParam *pData) {
  // 保存自己的类型
  AttributeBase::Load(pData);

  // 子属性序列化
  do_child_action(pData);
}
void TextRenderBaseAttribute::Save(SerializeParam *pData) {
  // 保存自己的类型
  AttributeBase::Save(pData);

  // 子属性序列化
  do_child_action(pData);
}

void TextRenderBaseAttribute::Editor(SerializeParam *pData,
                                     AttributeEditorProxy *p,
                                     EditorAttributeFlag e) {
  p->TextRenderBase2Editor(this, e);

  if (pData && e == EDITOR_ATTRIBUTE_ADD) {
    // 子属性序列化
    do_child_action(pData);
  }
}

ITextRenderBaseAttribute *
TextRenderBaseAttribute::GetITextRenderBaseAttribute() {
  if (!m_pITextRenderBaseAttribute)
    m_pITextRenderBaseAttribute = new ITextRenderBaseAttribute(this);

  return m_pITextRenderBaseAttribute;
}

} // namespace ui