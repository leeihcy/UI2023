#include "string_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/resource/i18nres.h"
#include "src/resource/res_bundle.h"

namespace ui {
AttributeBase *CreateStringAttribute() { return new StringAttribute(); }

// TODO:
// 注：为了支持i18n字段的编辑功能，不得不额外增加一个存储空间来存放string id
//    但为了尽量避免破坏非编辑模式下的代码逻辑，目前的做法是将set函数修改为
//    set(const char* text, int size)
//    在编辑模式下，将text分为两段，前一段表示转义之后的内容，后一段内容表示
//    string id，中间用\0分隔。
//    这种方式强制外部需要增加一个新的函数，不是很好，但没想到更好的办法

AttributeBase *CreateI18nStringAttribute() { return new I18nStringAttribute(); }

StringAttribute::StringAttribute() {
  m_pIStringAttribute = nullptr;
  m_pBindValue = nullptr;
  // _this = nullptr;
  // _getter = nullptr;
  // _setter = nullptr;
}

StringAttribute::~StringAttribute() { SAFE_DELETE(m_pIStringAttribute); }

IStringAttribute *StringAttribute::GetIStringAttribute() {
  if (!m_pIStringAttribute)
    m_pIStringAttribute = new IStringAttribute(this);

  return m_pIStringAttribute;
}

void StringAttribute::BindReference(std::string &s) { m_pBindValue = &s; }

// 这种情况下，默认值为nullptr
// void  StringAttribute::BindFunction(
//         void* _this, pfnStringSetter s, pfnStringGetter g)
// {
//     this->_this = _this;
//     this->_setter = s;
//     this->_getter = g;
// }

void StringAttribute::Bind(slot<void(const char *)> &&s,
                           slot<const char *()> &&g) {
  m_setter.connect(std::forward<slot<void(const char *)>>(s));
  m_getter.connect(std::forward<slot<const char *()>>(g));
}
// void  StringAttribute::BindFunction(
//         const std::function<void(const char*)>& s,
//         const std::function<const char*()>& g)
// {
//     _func_setter = s;
//     _func_getter = g;
// }

const char *StringAttribute::Get() {
  const char *szValue = nullptr;
  if (m_pBindValue) {
    szValue = m_pBindValue->c_str();
  }
  //     else if (_this && _getter)
  //     {
  //         const char*  szReturn = nullptr;
  // #ifdef _WIN64
  // 		szReturn = UIx64::GetString(_this, _getter);
  // #else
  // 		long  localVarThis = (long)_this;
  // 		long  localVarGetter = (long)_getter;

  // 		__asm
  // 		{
  // 			mov  ecx, localVarThis;  // 设置this
  // 			call localVarGetter;     // 调用
  // 			mov  szReturn, eax;      // 获取返回值
  // 		}
  // #endif
  // 		szValue = szReturn;
  //     }
  // else if (!_func_getter._Empty())
  else if (!m_getter.empty()) {
    szValue = m_getter.emit();
  }

  return szValue;
}

void StringAttribute::Set(const char *szValue) {
  if (m_pBindValue) {
    if (szValue)
      m_pBindValue->assign(szValue);
    else
      m_pBindValue->clear();
  }
  //     else if (_this && _setter)
  //     {
  // #ifdef _WIN64
  // 		UIx64::SetString(_this, szValue, _setter);
  // #else
  // 		long  localVarThis = (long)_this;
  // 		long  localVarSetter = (long)_setter;

  // 		__asm
  // 		{
  // 			mov eax, dword ptr [szValue] // 压入参数
  // 			push eax;

  // 			mov  ecx, localVarThis;   // 设置this
  // 			call localVarSetter;      // 调用
  // 		}
  // #endif
  //     }
  // else if (!_func_setter._Empty())
  else if (!m_setter.empty()) {
    m_setter.emit(szValue);
  }
}

void StringAttribute::Reset() { Set(GetDefault()); }

bool StringAttribute::IsDefaultValue() {
  const char *szValue = Get();
  const char *szDefault = GetDefault();

  if (m_strDefault.empty()) {
    if (!szValue || !szValue[0])
      return true;
  }

  if (szValue && m_strDefault == szValue)
    return true;

  return false;
}

StringAttribute *StringAttribute::SetDefault(const char *szValue) {
  if (szValue) {
    m_strDefault = szValue;
  } else {
    m_strDefault.clear();
  }

  return this;
}

const char *StringAttribute::GetDefault() { return m_strDefault.c_str(); }

void StringAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                             EditorAttributeFlag e) {
  p->String2Editor(this, e);
}

// void  StringAttribute::Internationalization()
// {
// 	m_bI18n = true;
// }

const char *I18nStringAttribute::Get() {
  const char *szValue = StringAttribute::Get();

  if (szValue) {
    if (GetUIApplication()->IsEditorMode()) {
      // 取出原始字符串id
      const char *szBuffer = szValue;
      int nLength1 = (int)strlen(szValue);
      if (nLength1)
        szValue = szBuffer + (nLength1 + 1);
    }
  }

  return szValue;
}

void I18nStringAttribute::Set(const char *szValue) {
  int nSize = 0;
  if (szValue) {
    int nLength1 = (int)strlen(szValue);
    int nLength2 = 0;

    const char *szValue2 = m_pSkinRes->GetI18nRes().MapConfigValue(szValue);
    if (szValue2)
      nLength2 = (int)strlen(szValue2);

    int length1 = (nLength1);
    int length2 = (nLength2);

    // 在编辑模式下面，不能将原始的id丢掉，因此保存到转译后的字符串后面，在Get时重新
    // 取出来。
    if (GetUIApplication()->IsEditorMode()) {
      char *szBuffer = GetTempBuffer(nLength1 + nLength2 + 2);
      char *p = (char *)szBuffer;

      int nSize1 = sizeof(char) * (length1 + 1);
      int nSize2 = sizeof(char) * (length2 + 1);
      memcpy(p, szValue2, nSize2);
      memcpy(p + nSize2, szValue, nSize1);
      szValue = szBuffer;

      nSize = length1 + length1 + 2;
    } else {
      szValue = szValue2;
      nSize = length1;
    }
  }

  if (m_pBindValue) {
    if (szValue)
      m_pBindValue->assign(szValue, nSize);
    else
      m_pBindValue->clear();
  }
#if 0 // x64TODO
    else if (_this && _setter_ex)
    {
        long  localVarThis = (long)_this;
        long  localVarSetter = (long)_setter_ex;

        __asm
        {
            mov eax, nSize;
            push eax;
            mov eax, dword ptr[szValue]; // 压入参数
            push eax;

            mov  ecx, localVarThis;   // 设置this
            call localVarSetter;      // 调用
        }
    }
#endif
  // else if (!_func_setter_ex._Empty())
  else if (!m_setter_ex.empty()) {
    m_setter_ex.emit(szValue, nSize);
  }
}

// 这种情况下，默认值为nullptr
// void  I18nStringAttribute::BindFunctionEx(
//         void* _this, pfnStringExSetter s, pfnStringGetter g)
// {
//     this->_this = _this;
//     this->_setter_ex = s;
//     this->_getter = g;
// }

void I18nStringAttribute::BindEx(slot<void(const char *, int)> &&s,
                                 slot<const char *()> &&g) {
  m_setter_ex.connect(std::forward<slot<void(const char *, int)>>(s));
  m_getter.connect(std::forward<slot<const char *()>>(g));
}

} // namespace ui