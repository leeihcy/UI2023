#include "int_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "src/util/DPI/dpihelper.h"
// #include "attributex64.h"

namespace ui {
AttributeBase *CreateIntAttribute() { return new IntAttribute(); }

IntAttribute::IntAttribute() {
  m_pIIntAttribute = nullptr;
  m_pBindValue = nullptr;
  m_lDefault = 0;

  m_eDpiScaleType = LONGATTR_DPI_SCALE_DISABLE;

  // _this = nullptr;
  // _setter = nullptr;
  // _getter = nullptr;
}

IntAttribute::~IntAttribute() { SAFE_DELETE(m_pIIntAttribute); }

IIntAttribute *IntAttribute::GetIIntAttribute() {
  if (!m_pIIntAttribute)
    m_pIIntAttribute = new IIntAttribute(this);

  return m_pIIntAttribute;
}

void IntAttribute::SetBindValue(void *p) { m_pBindValue = (int *)p; }

void IntAttribute::Bind(slot<void(int)> &&s, slot<int()> &&g) {
  m_setter.connect(std::forward<slot<void(int)>>(s));
  m_getter.connect(std::forward<slot<int()>>(g));
}
// void  IntAttribute::SetBindFuction(void* _this, void* _setter, void*
// _getter)
// {
//     this->_this = _this;
//     this->_setter = (pfnLongSetter)_setter;
//     this->_getter = (pfnLongGetter)_getter;
// }

IntAttribute *IntAttribute::SetDefault(int l) {
  m_lDefault = l;

  return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将IntAttribute的this又mov到ecx了
//
int IntAttribute::GetLong() {
  int lReturn = m_lDefault;
  if (m_pBindValue) {
    lReturn = (*m_pBindValue);
  }
  //     else if (_getter && _this)
  //     {
  //         lReturn = m_lDefault;
  // #ifdef _WIN64
  // 		lReturn = UIx64::GetLong(_this, _getter);
  // #else
  // 		ULONG_PTR localVarThis = (ULONG_PTR)_this;
  // 		ULONG_PTR localVarGetter = (ULONG_PTR)_getter;

  //         __asm
  //         {
  //             mov ecx, localVarThis;  // 设置this
  //             call localVarGetter;    // 调用
  //             mov lReturn, eax;       // 获取返回值
  //         }
  // #endif
  //     }
  else if (!m_getter.empty()) {
    lReturn = m_getter.emit();
  } else {
    lReturn = m_lDefault;
  }

  switch (m_eDpiScaleType) {
  case LONGATTR_DPI_SCALE_ENABLE:
    lReturn = ui::RestoreByDpi(lReturn);
    break;

  case LONGATTR_DPI_SCALE_GRATETHAN_0:
    lReturn = ui::RestoreByDpi_if_gt0(lReturn);
    break;

  default:
    break;
  }

  return lReturn;
}

void IntAttribute::SetLong(int l) {
  switch (m_eDpiScaleType) {
  case LONGATTR_DPI_SCALE_ENABLE:
    l = ui::ScaleByDpi(l);
    break;

  case LONGATTR_DPI_SCALE_GRATETHAN_0:
    l = ui::ScaleByDpi_if_gt0(l);
    break;
  default:
    break;
  }

  if (m_pBindValue) {
    (*m_pBindValue) = l;
  }
  //     else if (_setter && _this)
  //     {
  // #ifdef _WIN64
  // 		UIx64::SetLong(_this, l, _setter);
  // #else
  //         ULONG_PTR localVarThis = (ULONG_PTR)_this;
  // 		ULONG_PTR localVarSetter = (ULONG_PTR)_setter;

  //         __asm
  //         {
  //             mov eax, dword ptr [l]   // 压入参数
  //             push eax;

  //             mov ecx, localVarThis;   // 设置this
  //             call localVarSetter;     // 调用
  //         }
  // #endif
  //     }
  else if (!m_setter.empty()) {
    m_setter.emit(l);
  }
}
const char *IntAttribute::Get() {
  int lValue = GetLong();

  const char *szAlias = m_mapAlias.GetAlias(lValue);
  if (szAlias) {
    return szAlias;
  } else {
    char *szText = GetTempBuffer(64);
    sprintf(szText, "%d", lValue);
    return szText;
  }
}

void IntAttribute::Set(const char *szValue) {
  int lValue = m_lDefault;

  if (m_mapAlias.GetAlias(szValue, &lValue)) {
    SetLong(lValue);
  } else {
    lValue = atoi(szValue);
    SetLong(lValue);
  }
}

void IntAttribute::Reset() { SetLong(m_lDefault); }

bool IntAttribute::IsDefaultValue() { return GetLong() == m_lDefault; }

IntAttribute *IntAttribute::AddAlias(int l, const char *sz) {
  m_mapAlias.AddAlias(l, sz);
  return this;
}

void IntAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                           EditorAttributeFlag e) {
  p->Long2Editor(this, e);
}

int IntAttribute::EnumAlias(pfnEnumAliasCallback c, int w, int l) {
  return m_mapAlias.EnumAlias(c, w, l);
}
uint IntAttribute::GetAliasCount() { return m_mapAlias.GetCount(); }

IntAttribute *IntAttribute::SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e) {
  m_eDpiScaleType = e;
  return this;
}

} // namespace ui