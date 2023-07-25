#include "i18nmanager.h"
#include "include/interface/ixmlwrap.h"
#include "include/interface/iskinres.h"
#include "skinres.h"
#include "skinmanager.h"

namespace ui {
I18nManager::~I18nManager()
{
	std::list<UIElement*>::iterator iter = m_listElement.begin();
	for (; iter != m_listElement.end(); ++iter)
    {
        (*iter)->Release();
    }
    m_listElement.clear();
}

long  I18nManager::UIParseI18nTagCallback(IUIElement* pElem, ISkinRes* pSkinRes)
{
	I18nManager& mgr = pSkinRes->GetImpl()->GetI18nManager();
	return mgr.ParseNewElement(pElem->GetImpl());
}

/*
  <i18n>
    <string key="ok">
      <string id="ok" zh_cn="È·¶¨" en_us="OK"/>
    </string>
  </i18n>
*/
long  I18nManager::ParseNewElement(UIElement* p)
{
    UIASSERT(p);
    p->AddRef();
    m_listElement.push_back(p);

    parse(p);

    return 0;
}

void  I18nManager::parse(UIElement* p)
{
	const wchar_t* szLanguage = m_pSkinRes->GetSkinMgr().GetCurrentLanguage();

	UIElementProxy xml = p->FirstChild();
	while (xml)
	{
		std::wstring strId = xml->GetAttrib(XML_ID);
		std::wstring strValue = xml->GetAttrib(szLanguage);

		m_i18nRes.Add(strId.c_str(), strValue.c_str());
		xml = xml->NextElement();
	}
}

void  I18nManager::Reload()
{
    m_i18nRes.Clear();

	std::list<UIElement*>::iterator iter = m_listElement.begin();
	for (; iter != m_listElement.end(); ++iter)
    {
        parse(*iter);
    }
}
}