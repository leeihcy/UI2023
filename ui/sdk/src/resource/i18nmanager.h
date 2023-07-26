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
	I18nManager(ResBundle* p) : m_i18nRes(p) {
		m_pSkinRes = p;
	}
    ~I18nManager();

	I18nRes&  GetI18nRes() {
		return m_i18nRes;
	}

	static long  UIParseI18nTagCallback(IUIElement*, IResBundle* pSkinRes);
	long  ParseNewElement(UIElement*);

    void  Reload();
    
private:
    void  parse(UIElement*);

private:
	I18nRes  m_i18nRes;
	ResBundle*  m_pSkinRes;

    // xml结点记录，用于动态切换语言（如编辑器中）
    std::list<UIElement*>  m_listElement;
};



}