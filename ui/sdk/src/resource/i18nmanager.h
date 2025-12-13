#pragma once
#include "i18nres.h"
#include <list>

namespace ui
{
struct UIElement;
struct IUIElement;
struct IResourceBundle;

class I18nManager
{
public:
	I18nManager(ResourceBundle* p) : m_i18nRes(p) {
		m_resource_bundle = p;
	}
    ~I18nManager();

	I18nRes&  GetI18nRes() {
		return m_i18nRes;
	}

	static int  UIParseI18nTagCallback(IUIElement*, IResourceBundle* resource_bundle);
	int  ParseNewElement(UIElement*);

    void  Reload();
    
private:
    void  parse(UIElement*);

private:
	I18nRes  m_i18nRes;
	ResourceBundle*  m_resource_bundle;

    // xml结点记录，用于动态切换语言（如编辑器中）
    std::list<UIElement*>  m_listElement;
};



}