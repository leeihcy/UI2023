#include "stdafx.h"
#include "attribute_alias.h"

using namespace UI;

void  LongAttributeAlias::AddAlias(long l, LPCTSTR sz)
{
    if (sz)
        m_mapAlias.insert(make_pair(l, String(sz)));
}


LPCTSTR  LongAttributeAlias::GetAlias(long l)
{
    AliasIter iter = m_mapAlias.find(l);
    if (iter == m_mapAlias.end())
        return nullptr;

    return iter->second.c_str();
}

bool  LongAttributeAlias::GetAlias(LPCTSTR sz, long* pOut)
{
    if (!sz || !pOut)
        return false;

    AliasIter iter = m_mapAlias.begin();
    for (; iter != m_mapAlias.end(); iter++)
    {
        if (iter->second == sz)
        {
            *pOut = iter->first;
            return true;
        }
    }
    return false;
}

long  LongAttributeAlias::EnumAlias(pfnEnumAliasCallback callback, WPARAM w, LPARAM l)
{
	uint nSize = (uint)m_mapAlias.size();
	AliasIter iter = m_mapAlias.begin();
	for (; iter != m_mapAlias.end(); iter++)
	{
		callback(iter->second.c_str(), iter->first, w, l);
	}

	return nSize;
}

uint  LongAttributeAlias::GetCount()
{
	return (uint)m_mapAlias.size();
}