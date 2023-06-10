#include "stdafx.h"
#include "flags_attribute.h"
#include "attribute.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateFlagsAttribute()
    {
        return new FlagsAttribute();
    }
}

FlagsAttribute::FlagsAttribute()
{
    m_pIFlagsAttribute = nullptr;
}
FlagsAttribute::~FlagsAttribute()
{
    SAFE_DELETE(m_pIFlagsAttribute);
}

IFlagsAttribute*  FlagsAttribute::GetIFlagsAttribute()
{
    if (!m_pIFlagsAttribute)
        m_pIFlagsAttribute = new IFlagsAttribute(this);

    return m_pIFlagsAttribute;
}

void  FlagsAttribute::Set(LPCTSTR szValue)
{
    Util::ISplitStringEnum*  pEnum = nullptr;
    UINT nCount = Util::SplitString(szValue, XML_FLAG_SEPARATOR, &pEnum);
    if (!nCount)
    {
        SetLong(m_lDefault);
        return;
    }

    long lValue = 0;
    for (UINT i = 0; i < nCount; i++)
    {
        LPCTSTR  szFlag = pEnum->GetText(i);
        long lFlag = 0;
        if (m_mapAlias.GetAlias(szFlag, &lFlag))
        {
            lValue |= lFlag;
        }
        else
        {
            // 规定flags必须16进制数值
            lValue |= _tcstol(szFlag, nullptr, 16); // _ttoi(szFlag);
        }
    }
    SAFE_RELEASE(pEnum);
    SetLong(lValue);
}

LPCTSTR  FlagsAttribute::Get()
{
	String&  strBuffer = GetTempBufferString();

    long lValue = GetLong();
    long lRemainValue = 0;  // 没有别名的flag值，统一放在最后，用16进制数字显示
	int nSize = sizeof(long)*8;
    for (int i = 0; i < nSize; i++)  // 逐位判断
    {
        long lFlag = 1<<i;
        if (lValue & lFlag)
        {
            LPCTSTR szFlag = m_mapAlias.GetAlias(lFlag);
            if (szFlag)
            {
                if (!strBuffer.empty())
                    strBuffer.push_back(XML_FLAG_SEPARATOR);

                strBuffer.append(szFlag);
            }
            else
            {
                lRemainValue |= lValue;
            }
        }
    }

    if (lRemainValue)
    {
        if (!strBuffer.empty())
            strBuffer.push_back(XML_FLAG_SEPARATOR);

		TCHAR  szText[16] = {0};
        _stprintf(szText, TEXT("%x"), lValue);
        strBuffer.append(szText);
    }

    return strBuffer.c_str();
}

FlagsAttribute*  FlagsAttribute::AddFlag(long l, LPCTSTR sz)
{
    __super::AddAlias(l, sz);
    return this;
}

void  FlagsAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
     p->Flags2Editor(this, e);
}
