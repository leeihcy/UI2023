#include "flags_attribute.h"
#include "attribute.h"
#include "include/inc.h"

using namespace ui;

namespace ui {
AttributeBase *CreateFlagsAttribute() { return new FlagsAttribute(); }

FlagsAttribute::FlagsAttribute() { m_pIFlagsAttribute = nullptr; }
FlagsAttribute::~FlagsAttribute() { SAFE_DELETE(m_pIFlagsAttribute); }

IFlagsAttribute *FlagsAttribute::GetIFlagsAttribute() {
  if (!m_pIFlagsAttribute)
    m_pIFlagsAttribute = new IFlagsAttribute(this);

  return m_pIFlagsAttribute;
}

void FlagsAttribute::Set(const char *szValue) {
  util::ISplitStringEnum *pEnum = nullptr;
  unsigned int nCount = util::SplitString(szValue, XML_FLAG_SEPARATOR, &pEnum);
  if (!nCount) {
    SetLong(m_lDefault);
    return;
  }

  int lValue = 0;
  for (unsigned int i = 0; i < nCount; i++) {
    const char *szFlag = pEnum->GetText(i);
    int lFlag = 0;
    if (m_mapAlias.GetAlias(szFlag, &lFlag)) {
      lValue |= lFlag;
    } else {
      // 规定flags必须16进制数值
      lValue |= strtol(szFlag, nullptr, 16); // util::wtoi(szFlag);
    }
  }
  SAFE_RELEASE(pEnum);
  SetLong(lValue);
}

const char *FlagsAttribute::Get() {
  std::string &strBuffer = GetTempBufferString();

  int lValue = GetLong();
  int lRemainValue = 0; // 没有别名的flag值，统一放在最后，用16进制数字显示
  int nSize = sizeof(int) * 8;
  for (int i = 0; i < nSize; i++) // 逐位判断
  {
    int lFlag = 1 << i;
    if (lValue & lFlag) {
      const char *szFlag = m_mapAlias.GetAlias(lFlag);
      if (szFlag) {
        if (!strBuffer.empty())
          strBuffer.push_back(XML_FLAG_SEPARATOR);

        strBuffer.append(szFlag);
      } else {
        lRemainValue |= lValue;
      }
    }
  }

  if (lRemainValue) {
    if (!strBuffer.empty())
      strBuffer.push_back(XML_FLAG_SEPARATOR);

    char szText[16] = {0};
    sprintf(szText, "%x", lValue);
    strBuffer.append(szText);
  }

  return strBuffer.c_str();
}

FlagsAttribute *FlagsAttribute::AddFlag(int l, const char *sz) {
  IntAttribute::AddAlias(l, sz);
  return this;
}

void FlagsAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                            EditorAttributeFlag e) {
  p->Flags2Editor(this, e);
}

} // namespace ui