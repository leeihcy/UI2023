#pragma once
#include "i18nres.h"
#include <list>

namespace ui
{
struct UIElement;
struct IUIElement;

class I18nManager
{
public:
	I18nManager(Resource* p) : m_i18nRes(p) {
		m_pSkinRes = p;
	}
    ~I18nManager();

	I18nRes&  GetI18nRes() {
		return m_i18nRes;
	}

	static int  UIParseI18nTagCallback(IUIElement*, IResource* pSkinRes);
	int  ParseNewElement(UIElement*);

    void  Reload();
    
private:
    void  parse(UIElement*);

private:
	I18nRes  m_i18nRes;
	Resource*  m_pSkinRes;

    // xml结点记录，用于动态切换语言（如编辑器中）
    std::list<UIElement*>  m_listElement;
};



}