#include "bool_attribute.h"
#include "attribute.h"

namespace ui {
AttributeBase *CreateBoolAttribute() { return new BoolAttribute(); }

BoolAttribute::BoolAttribute() {
  m_pIBoolAttribute = nullptr;
  m_pBindValue = nullptr;
  m_bDefault = false;

  // _this = nullptr;
  // _setter = nullptr;
  // _getter = nullptr;
}

BoolAttribute::~BoolAttribute() { SAFE_DELETE(m_pIBoolAttribute); }
IBoolAttribute *BoolAttribute::GetIBoolAttribute() {
  if (!m_pIBoolAttribute)
    m_pIBoolAttribute = new IBoolAttribute(this);

  return m_pIBoolAttribute;
}

void BoolAttribute::SetBindValue(void *p) { m_pBindValue = (bool *)p; }

void BoolAttribute::Bind(slot<void(bool)> &&s, slot<bool()> &&g) {
  m_setter.connect(std::forward<slot<void(bool)>>(s));
  m_getter.connect(std::forward<slot<bool()>>(g));
}
// void  BoolAttribute::SetBindFuction(void* _this, void* _setter, void*
// _getter)
// {
//     this->_this = _this;
//     this->_setter = (pfnBoolSetter)_setter;
//     this->_getter = (pfnBoolGetter)_getter;
// }

BoolAttribute *BoolAttribute::SetDefault(bool b) {
  m_bDefault = b;
  return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将BoolAttribute的this又mov到ecx了
//
bool BoolAttribute::GetBool() {
  if (m_pBindValue) {
    return (*m_pBindValue);
  }
  //     else if (_getter && _this)
  //     {
  //         bool bReturn = m_bDefault;
  // #ifdef _WIN64
  // 		bReturn = UIx64::GetBool(_this, _getter);
  // #else
  //         long localVarThis = (long)_this;
  //         long localVarGetter = (long)_getter;

  //         __asm mov ecx, localVarThis;  // 设置this
  //         __asm call localVarGetter;    // 调用
  //         __asm mov bReturn, al;        // 获取返回值
  // #endif
  //         return bReturn;
  //     }
  else if (!m_getter.empty()) {
    return m_getter.emit();
  } else {
    return m_bDefault;
  }
}
void BoolAttribute::SetBool(bool b) {
  if (m_pBindValue) {
    (*m_pBindValue) = b;
  }
  //     else if (_setter && _this)
  //     {
  // #ifdef _WIN64
  // 		UIx64::SetBool(_this, b, _setter);
  // #else
  //         long localVarThis = (long)_this;
  //         long localVarSetter = (long)_setter;

  //         __asm
  //         {
  //             movzx eax, byte ptr [b]   // 压入参数
  //             push eax;

  //             mov ecx, localVarThis;   // 设置this
  //             call localVarSetter;     // 调用
  //         }
  // #endif
  //     }
  else if (!m_setter.empty()) {
    m_setter.emit(b);
  }
}
const char *BoolAttribute::Get() {
  bool b = GetBool();

  char *szText = GetTempBuffer(64);
  strcpy(szText, b ? XML_BOOL_VALUE_1 : XML_BOOL_VALUE_0);

  return szText;
}

void BoolAttribute::Set(const char *szValue) {
  bool b = false;
  if (0 == strcmp(szValue, XML_BOOL_VALUE_TRUE) ||
      0 == strcmp(szValue, XML_BOOL_VALUE_1) ||
      0 == strcmp(szValue, XML_BOOL_VALUE_YES)) {
    b = true;
  }
  //  else if (0 == strcmp(szValue, XML_BOOL_VALUE_FALSE) ||
  //      0 == strcmp(szValue, XML_BOOL_VALUE_0)      ||
  //      0 == strcmp(szValue, XML_BOOL_VALUE_NO))
  //  {
  //      b = false;
  //  }

  SetBool(b);
}

void BoolAttribute::Reset() { SetBool(m_bDefault); }

bool BoolAttribute::IsDefaultValue() { return GetBool() == m_bDefault; }

void BoolAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                           EditorAttributeFlag e) {
  p->Bool2Editor(this, e);
}

} // namespace ui