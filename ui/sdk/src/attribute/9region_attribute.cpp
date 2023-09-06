#include "9region_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "src/util/DPI/dpihelper.h"


namespace ui {
AttributeBase *CreateRegion9Attribute() { return new Region9Attribute(); }

Region9Attribute::Region9Attribute() {
  m_pIRegion9Attribute = nullptr;
  m_pBindValue = nullptr;

  // _this = nullptr;
  // _setter = nullptr;
  // _getter = nullptr;
}

Region9Attribute::~Region9Attribute() { SAFE_DELETE(m_pIRegion9Attribute); }

void Region9Attribute::SetBindValue(void *p) { m_pBindValue = (C9Region *)p; }

// void  Region9Attribute::SetBindFuction(void* _this, void* _setter, void*
// _getter)
// {
//     this->_this = _this;
//     this->_setter = (pfn9RegionSetter)_setter;
//     this->_getter = (pfn9RegionGetter)_getter;
// }
void Region9Attribute::Bind(slot<void(C9Region *)> s,
                            slot<void(C9Region *)> g) {
  m_setter.connect(std::forward<slot<void(C9Region *)>>(s));
  m_getter.connect(std::forward<slot<void(C9Region *)>>(g));
}

Region9Attribute *Region9Attribute::SetDefault(C9Region *pr) {
  if (pr)
    m_rDefault.Copy(*pr);
  else
    m_rDefault.Set(0);

  return this;
}

void Region9Attribute::get(C9Region *pr) {
  if (m_pBindValue) {
    pr->Copy(*m_pBindValue);
  }
  //     else if (_getter && _this)
  //     {
  // #ifdef _WIN64
  // 		UIx64::Get9Region(_this, pr, _getter);
  // #else

  //         long localVarThis = (long)_this;
  //         long localVarGetter = (long)_getter;

  //         __asm
  //         {
  //             mov eax, pr
  //             push eax;

  // 			mov ecx, localVarThis;
  // 			call localVarGetter;
  //         }
  // #endif
  //     }
  else if (!m_getter.empty()) {
    m_getter.emit(pr);
  } else {
    pr->Copy(m_rDefault);
  }

  if (pr && DEFAULT_SCREEN_DPI != ui::GetDpiScale()) {
    pr->DpiRestore();
  }
}

void Region9Attribute::set(C9Region *pr) {
  if (pr && DEFAULT_SCREEN_DPI != ui::GetDpiScale()) {
    pr->DpiScale();
  }
  if (m_pBindValue) {
    m_pBindValue->Copy(*pr);
  }
  //     else if (_setter && _this)
  //     {
  // #ifdef _WIN64
  // 		UIx64::Set9Region(_this, pr, _setter);
  // #else
  //         long localVarThis = (long)_this;
  //         long localVarSetter = (long)_setter;

  //         __asm
  //         {
  //             mov eax, pr   // 压入参数
  //             push eax;

  //             mov ecx, localVarThis;   // 设置this
  //             call localVarSetter;     // 调用
  //         }
  // #endif
  //     }
  else if (!m_setter.empty()) {
    m_setter.emit(pr);
  }
}

const char *Region9Attribute::Get() {
  C9Region r;
  get(&r);

  char *szText = GetTempBuffer(64);
  if (r.IsSimpleValue()) {
    sprintf(szText, "%d", r.left);
  } else if (r.IsSimpleRect()) {
    sprintf(szText, "%d,%d,%d,%d", r.left, r.top, r.right, r.bottom);
  } else {
    sprintf(szText, "%d,%d,%d,%d,%d,%d,%d,%d", r.topleft, r.top, r.topright,
            r.left, r.right, r.bottomleft, r.bottom, r.bottomright);
  }
  return szText;
}

void Region9Attribute::Set(const char *szValue) {
  C9Region r;
  if (util::TranslateImage9Region(szValue, &r, XML_SEPARATOR)) {
    set(&r);
  }
}

void Region9Attribute::Reset() { set(&m_rDefault); }

bool Region9Attribute::IsDefaultValue() {
  C9Region r;
  get(&r);

  return r.IsEqual(m_rDefault);
}

void Region9Attribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                              EditorAttributeFlag e) {
  p->Region92Editor(this, e);
}

IRegion9Attribute *Region9Attribute::GetIRegion9Attribute() {
  if (!m_pIRegion9Attribute)
    m_pIRegion9Attribute = new IRegion9Attribute(this);
  return m_pIRegion9Attribute;
}

} // namespace ui