#include "stringselect_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "include/interface/iuiapplication.h"
#include "src/application/uiapplication.h"

namespace ui {
AttributeBase *CreateStringEnumAttribute() { return new StringEnumAttribute(); }

StringEnumAttribute::StringEnumAttribute() {
  m_pIStringEnumAttribute = nullptr;
  m_bEditable = false;
}

StringEnumAttribute::~StringEnumAttribute() {
  SAFE_DELETE(m_pIStringEnumAttribute);
}

IStringEnumAttribute *StringEnumAttribute::GetIStringEnumAttribute() {
  if (!m_pIStringEnumAttribute)
    m_pIStringEnumAttribute = new IStringEnumAttribute(this);

  return m_pIStringEnumAttribute;
}

StringEnumAttribute *StringEnumAttribute::Add(const char *szText) {
  if (IsExist(szText))
    return this;

  if (szText)
    m_list.push_back(std::string(szText));

  return this;
}

bool StringEnumAttribute::IsExist(const char *szText) {
  if (!szText)
    return false;

  std::list<std::string>::iterator iter = m_list.begin();
  for (; iter != m_list.end(); ++iter) {
    if ((*iter) == szText)
      return true;
  }

  return false;
}

void StringEnumAttribute::EnumString(pfnEnumStringEnumCallback callback,
                                     llong w, llong l) {
  std::list<std::string>::iterator iter = m_list.begin();
  for (; iter != m_list.end(); ++iter) {
    callback((*iter).c_str(), w, l);
  }
  return;
}

const char *StringEnumAttribute::Get() {
  const char *szValue = StringAttribute::Get();
  if (!szValue)
    return nullptr;

  if (m_bEditable) {
    return szValue;
  } else {
    if (IsExist(szValue))
      return szValue;
    else
      return m_strDefault.c_str();
  }
}

void StringEnumAttribute::Set(const char *szValue) {
  if (!m_bEditable && !IsExist(szValue)) {
    return StringAttribute::Set(m_strDefault.c_str());
  }

  StringAttribute::Set(szValue);
}

bool EnumLayoutTypeCallback(const char *szName, llong wParam, llong) {
  StringEnumAttribute *pThis = (StringEnumAttribute *)wParam;
  pThis->Add(szName);

  return true;
}

// 填充可选的布局类型：
// "canvas" LAYOUT_TYPE_CANVAS
// ...
StringEnumAttribute *StringEnumAttribute::FillLayoutTypeData() {
  Application *pUIApp = GetUIApplication();
  if (pUIApp)
    pUIApp->GetIUIApplication()->EnumLayoutType(EnumLayoutTypeCallback,
                                                (llong)this, 0);

  return this;
}

void EnumRenderBaseNameCallback(const char *szText, llong wParam,
                                llong lParam) {
  StringEnumAttribute *pThis = (StringEnumAttribute *)wParam;
  pThis->Add(szText);
}
StringEnumAttribute *StringEnumAttribute::FillRenderBaseTypeData() {
  m_list.clear();
  this->Add(EMPTYTEXT); // 增加一项默认值，取消当前的选项
  GetUIApplication()->GetIUIApplication()->EnumRenderBaseName(
      EnumRenderBaseNameCallback, (llong)this, 0);
  return this;
}

void EnumTextRenderBaseNameCallback(const char *szText, llong wParam,
                                    llong lParam) {
  StringEnumAttribute *pThis = (StringEnumAttribute *)wParam;
  pThis->Add(szText);
}
StringEnumAttribute *StringEnumAttribute::FillTextRenderBaseTypeData() {
  m_list.clear();
  GetUIApplication()->GetIUIApplication()->EnumTextRenderBaseName(
      EnumTextRenderBaseNameCallback, (llong)this, 0);
  return this;
}

void StringEnumAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                                 EditorAttributeFlag e) {
  p->StringEnum2Editor(this, e);
}

} // namespace ui