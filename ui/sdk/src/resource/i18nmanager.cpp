#include "i18nmanager.h"
#include "include/interface/ixmlwrap.h"
#include "include/interface/iresbundle.h"
#include "res_bundle.h"
#include "resource_manager.h"

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

int  I18nManager::UIParseI18nTagCallback(IUIElement* pElem, IResource* pSkinRes)
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
int  I18nManager::ParseNewElement(UIElement* p)
{
    UIASSERT(p);
    p->AddRef();
    m_listElement.push_back(p);

    parse(p);

    return 0;
}

void  I18nManager::parse(UIElement* p)
{
	const char* szLanguage = m_pSkinRes->GetSkinMgr().GetCurrentLanguage();

	UIElementProxy xml = p->FirstChild();
	while (xml)
	{
		std::string strId = xml->GetAttrib(XML_ID);
		std::string strValue = xml->GetAttrib(szLanguage);

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