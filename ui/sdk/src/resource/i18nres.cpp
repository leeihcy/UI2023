#include "i18nres.h"
#include "include/inc.h"
#include "res_bundle.h"

namespace ui {
I18nRes::I18nRes(Resource *p) {
  m_pSkinRes = p;
  m_pII18nRes = nullptr;
}

I18nRes::~I18nRes() { SAFE_DELETE(m_pII18nRes); }

const char *I18nRes::Map(const char *key) {
  if (!key)
    return nullptr;

  std::map<std::string, std::string>::iterator iter = m_dict.find(std::string(key));
  if (iter == m_dict.end()) {
    Resource *pParentRes = m_pSkinRes->GetParentSkinRes();
    if (pParentRes)
      return pParentRes->GetI18nRes().Map(key);

    return nullptr;
  }

  return iter->second.c_str();
}

// 尝试翻译一个xml中的配置项，如果是一个i18n属性，则翻译，否则直接返回原字符串
const char *I18nRes::MapConfigValue(const char *szConfigText) {
  if (!szConfigText) {
    return "";
  }

  if (!szConfigText[0] || !szConfigText[1])
    return szConfigText;

  if (szConfigText[0] != XML_I18N_PREFIX)
    return szConfigText;

  if (szConfigText[1] == XML_I18N_PREFIX) {
    // 两个##，取消转义
    szConfigText = szConfigText + 1;
    return szConfigText;
  }

  // 后面的为需要解析的国际化字段
  szConfigText = szConfigText + 1;
  const char *szRet = Map(szConfigText);
  if (szRet)
    return szRet;

  UI_LOG_WARN("i18n string not found, key=%s", szConfigText);
  return "";
}

void I18nRes::Add(const char *szId, const char *szValue) {
  if (!szId || !szValue)
    return;

  m_dict[std::string(szId)] = std::string(szValue);
}

void I18nRes::Clear() { m_dict.clear(); }

II18nRes &I18nRes::GetII18nRes() {
  if (!m_pII18nRes)
    m_pII18nRes = new II18nRes(this);

  return *m_pII18nRes;
}

} // namespace ui