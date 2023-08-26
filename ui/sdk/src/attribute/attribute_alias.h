#pragma once
#include <map>
#include <string>

#include "include/interface/iattribute.h"

//
// 别名映射 (对于枚举类型有用)
//
// 另外对于一些特殊数值也有用，例如"auto" --> AUTO
//

namespace ui {
class IntAttributeAlias {
public:
  const char *GetAlias(int);
  bool GetAlias(const char *, int *pOut);
  void AddAlias(int, const char *);

  uint GetCount();
  int EnumAlias(pfnEnumAliasCallback, int, int);

private:
  typedef std::multimap<int, std::string> AliasMap;
  typedef AliasMap::iterator AliasIter;

  AliasMap m_mapAlias;
};
} // namespace ui