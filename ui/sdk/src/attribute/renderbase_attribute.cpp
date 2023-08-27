#include "renderbase_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/object/object.h"

namespace ui {
AttributeBase *CreateRenderBaseAttribute() { return new RenderBaseAttribute(); }

RenderBaseAttribute::RenderBaseAttribute() {
  ReloadOnChanged();

  m_pIRenderBaseAttribute = nullptr;
  m_ppBindValue = nullptr;
  m_pObject = nullptr;
  //     _this = nullptr;
  //     _setter = nullptr;
  //     _getter = nullptr;
}

RenderBaseAttribute::~RenderBaseAttribute() {
  SAFE_DELETE(m_pIRenderBaseAttribute);
}

void RenderBaseAttribute::SetBindValue(void *p) {
  m_ppBindValue = (IRenderBase **)p;
}

void RenderBaseAttribute::Set(const char *szType) {
  if (!m_ppBindValue)
    return;

  if (!m_pObject) {
    UIASSERT(0);
    return;
  }

  Reset();
  GetUIApplication()->CreateRenderBaseByName(szType, m_pObject->GetIObject(),
                                           m_ppBindValue);
}

void RenderBaseAttribute::Reset() {
  if (m_ppBindValue && *m_ppBindValue) {
    *m_ppBindValue = nullptr;
  } else {
  }
}

const char *RenderBaseAttribute::Get() {
  if (!m_ppBindValue)
    return nullptr;
  if (!*m_ppBindValue)
    return nullptr;

  RENDER_TYPE eType = (*m_ppBindValue)->GetType();
  RenderBaseFactory &factory = GetUIApplication()->GetRenderBaseFactory();
  const char *szType = factory.GetRenderBaseName(eType);
  return szType;
}

void RenderBaseAttribute::do_child_action(SerializeParam *pData) {
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

void RenderBaseAttribute::Load(SerializeParam *pData) {
  // 保存自己的类型
  AttributeBase::Load(pData);

  do_child_action(pData);
}

void RenderBaseAttribute::Save(SerializeParam *pData) {
  // 保存自己的类型
  AttributeBase::Save(pData);

  do_child_action(pData);
}

bool RenderBaseAttribute::IsDefaultValue() {
  if (m_ppBindValue) {
    if (*m_ppBindValue == nullptr)
      return true;
  }

  return false;
}

void RenderBaseAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                                 EditorAttributeFlag e) {
  p->RenderBase2Editor(this, e);

  if (pData && e == EDITOR_ATTRIBUTE_ADD) {
    // 子属性序列化
    do_child_action(pData);
  }
}

IRenderBaseAttribute *RenderBaseAttribute::GetIRenderBaseAttribute() {
  if (!m_pIRenderBaseAttribute)
    m_pIRenderBaseAttribute = new IRenderBaseAttribute(this);

  return m_pIRenderBaseAttribute;
}

} // namespace ui