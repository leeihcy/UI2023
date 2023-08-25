#pragma once
#include <map>
#include <string>

#include "include/interface/iattribute.h"

//
// 别名映射 (对于枚举类型有用)
//
// 另外对于一些特殊数值也有用，例如"auto" --> AUTO
// 

namespace ui
{
class IntAttributeAlias
{
public:
    const wchar_t*  GetAlias(int);
    bool  GetAlias(const wchar_t*, int* pOut);
    void  AddAlias(int, const wchar_t*);

	uint  GetCount();
	int  EnumAlias(pfnEnumAliasCallback, int, int);

private:
    typedef std::multimap<int, std::wstring>  AliasMap;
    typedef AliasMap::iterator  AliasIter;

    AliasMap  m_mapAlias;
};
}