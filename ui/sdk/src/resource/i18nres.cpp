#include "include/inc.h"
#include "i18nres.h"
#include "skinres.h"

I18nRes::I18nRes(SkinRes* p)
{
	m_pSkinRes = p;
	m_pII18nRes = nullptr;
}

I18nRes::~I18nRes()
{
	SAFE_DELETE(m_pII18nRes);
}

const wchar_t* I18nRes::Map(const wchar_t* key)
{
	if (!key)
		return nullptr;

	map<String, String>::iterator iter = m_dict.find(String(key));
    if (iter == m_dict.end())
    {
        SkinRes* pParentRes = m_pSkinRes->GetParentSkinRes();
        if (pParentRes)
            return pParentRes->GetI18nRes().Map(key);

        return nullptr;
    }

	return iter->second.c_str();
}

// ���Է���һ��xml�е�����������һ��i18n���ԣ����룬����ֱ�ӷ���ԭ�ַ���
const wchar_t*  I18nRes::MapConfigValue(const wchar_t* szConfigText)
{
    if (!szConfigText)
    {
        return TEXT("");
    }

    if (!szConfigText[0] || !szConfigText[1])
        return szConfigText;

    if (szConfigText[0] != XML_I18N_PREFIX)
        return szConfigText;
        
    if (szConfigText[1] == XML_I18N_PREFIX)
    {
        // ����##��ȡ��ת��
        szConfigText = szConfigText + 1;
        return szConfigText;
    }

    // �����Ϊ��Ҫ�����Ĺ��ʻ��ֶ�
    szConfigText = szConfigText + 1;
    const wchar_t* szRet = Map(szConfigText);
    if (szRet)
        return szRet;

    UI_LOG_WARN(TEXT("i18n string not found, key=%s"), szConfigText);
    return TEXT("");
}

void  I18nRes::Add(const wchar_t* szId, const wchar_t* szValue)
{
	if (!szId || !szValue)
		return;

	m_dict[String(szId)] = String(szValue);
}

void  I18nRes::Clear()
{
    m_dict.clear();
}

II18nRes&  I18nRes::GetII18nRes()
{
	if (!m_pII18nRes)
		m_pII18nRes = new II18nRes(this);

	return *m_pII18nRes;
}