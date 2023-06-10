#pragma once
#include "Inc\Interface\iattribute.h"

//
// 别名映射 (对于枚举类型有用)
//
// 另外对于一些特殊数值也有用，例如"auto" --> AUTO
// 

namespace UI
{
class LongAttributeAlias
{
public:
    LPCTSTR  GetAlias(long);
    bool  GetAlias(LPCTSTR, long* pOut);
    void  AddAlias(long, LPCTSTR);

	uint  GetCount();
	long  EnumAlias(pfnEnumAliasCallback, WPARAM, LPARAM);

private:
    typedef std::multimap<long, String>  AliasMap;
    typedef AliasMap::iterator  AliasIter;

    AliasMap  m_mapAlias;
};
}