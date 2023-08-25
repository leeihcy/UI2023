#include "attribute_alias.h"
#include "include/inc.h"

using namespace ui;

void IntAttributeAlias::AddAlias(int l, const wchar_t *sz) {
  if (sz) {
    m_mapAlias.insert(make_pair(l, std::wstring(sz)));
  }
}

const wchar_t *IntAttributeAlias::GetAlias(int l) {
  AliasIter iter = m_mapAlias.find(l);
  if (iter == m_mapAlias.end())
    return nullptr;

  return iter->second.c_str();
}

bool IntAttributeAlias::GetAlias(const wchar_t *sz, int *pOut) {
  if (!sz || !pOut)
    return false;

  AliasIter iter = m_mapAlias.begin();
  for (; iter != m_mapAlias.end(); iter++) {
    if (iter->second == sz) {
      *pOut = iter->first;
      return true;
    }
  }
  return false;
}

int IntAttributeAlias::EnumAlias(pfnEnumAliasCallback callback, int w,
                                   int l) {
  uint nSize = (uint)m_mapAlias.size();
  AliasIter iter = m_mapAlias.begin();
  for (; iter != m_mapAlias.end(); iter++) {
    callback(iter->second.c_str(), iter->first, w, l);
  }

  return nSize;
}

uint IntAttributeAlias::GetCount() { return (uint)m_mapAlias.size(); }