#include "chararray_attribute.h"
#include "attribute.h"
#include "attributex64.h"
#include "include/inc.h"

using namespace ui;

namespace ui {
AttributeBase *CreateCharArrayAttribute() { return new CharArrayAttribute(); }
} // namespace ui

CharArrayAttribute::CharArrayAttribute() {
  m_pICharArrayAttribute = nullptr;
  m_pBindValue = nullptr;
  _this = nullptr;
  _getter = nullptr;
  _setter = nullptr;
  m_bDefaultIsNULL = true;
  m_lBindValueSize = 16;
}

CharArrayAttribute::~CharArrayAttribute() {
  SAFE_DELETE(m_pICharArrayAttribute);
}

ICharArrayAttribute *CharArrayAttribute::GetICharArrayAttribute() {
  if (!m_pICharArrayAttribute)
    m_pICharArrayAttribute = new ICharArrayAttribute(this);

  return m_pICharArrayAttribute;
}

void CharArrayAttribute::SetBindValue(void *p) {
  m_pBindValue = (char *)p;
  m_bDefaultIsNULL = false;
}

void CharArrayAttribute::SetBindValueSize(long lSize) {
  m_lBindValueSize = lSize;
}

// 这种情况下，默认值为nullptr
void CharArrayAttribute::SetBindFuction(void *_this, void *_setter,
                                        void *_getter) {
  this->_this = _this;
  this->_setter = (pfnStringSetter)_setter;
  this->_getter = (pfnStringGetter)_getter;
  m_bDefaultIsNULL = true;
}

const char *CharArrayAttribute::Get() {
  if (m_pBindValue) {
    return m_pBindValue;
  } else if (_this && _getter) {
    const char *szReturn = nullptr;
#ifdef _WIN64
    szReturn = UIx64::GetString(_this, _getter);
#else
    long localVarThis = (long)_this;
    long localVarGetter = (long)_getter;

    __asm
    {
			mov  ecx, localVarThis; // 设置this
			call localVarGetter; // 调用
			mov  szReturn, eax; // 获取返回值
    }
#endif
    return szReturn;
  } else {
    return nullptr;
  }
}

void CharArrayAttribute::Set(const char *szValue) {
  if (m_pBindValue) {
    if (szValue) {
      _tcsncpy(m_pBindValue, szValue, m_lBindValueSize - 1);
      m_pBindValue[m_lBindValueSize - 1] = 0;
    } else {
      m_pBindValue[0] = 0;
    }
  } else if (_this && _setter) {
#ifdef _WIN64
    UIx64::SetString(_this, szValue, _setter);
#else
    long localVarThis = (long)_this;
    long localVarSetter = (long)_setter;

    __asm
    {
			mov eax, dword ptr [szValue] // 压入参数
			push eax;

			mov  ecx, localVarThis; // 设置this
			call localVarSetter; // 调用
    }
#endif
  }
}

void CharArrayAttribute::Reset() { Set(GetDefault()); }

bool CharArrayAttribute::IsDefaultValue() {
  const char *szValue = Get();
  const char *szDefault = GetDefault();

  if (szValue == nullptr && szDefault == nullptr)
    return true;

  if (szValue && szDefault && 0 == strcmp(szValue, szDefault))
    return true;

  return false;
}

CharArrayAttribute *CharArrayAttribute::SetDefault(const char *szValue) {
  if (szValue) {
    m_bDefaultIsNULL = false;
    m_strDefault = szValue;
  } else {
    m_bDefaultIsNULL = true;
    m_strDefault.clear();
  }

  return this;
}

const char *CharArrayAttribute::GetDefault() {
  if (m_bDefaultIsNULL)
    return nullptr;

  return m_strDefault.c_str();
}

void CharArrayAttribute::Editor(SerializeParam *pData,
                                UI::AttributeEditorProxy *p,
                                UI::EditorAttributeFlag e) {
  p->CharArray2Editor(this, e);
}