#pragma once
#include <map>
#include <string>

#include "include/interface/iattribute.h"

//
// ����ӳ�� (����ö����������)
//
// �������һЩ������ֵҲ���ã�����"auto" --> AUTO
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