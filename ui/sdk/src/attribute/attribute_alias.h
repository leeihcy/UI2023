#pragma once
#include "include/interface/iattribute.h"
#include <map>
//
// ����ӳ�� (����ö����������)
//
// �������һЩ������ֵҲ���ã�����"auto" --> AUTO
// 

namespace ui
{
class LongAttributeAlias
{
public:
    const wchar_t*  GetAlias(long);
    bool  GetAlias(const wchar_t*, long* pOut);
    void  AddAlias(long, const wchar_t*);

	uint  GetCount();
	long  EnumAlias(pfnEnumAliasCallback, long, long);

private:
    typedef std::multimap<long, String>  AliasMap;
    typedef AliasMap::iterator  AliasIter;

    AliasMap  m_mapAlias;
};
}