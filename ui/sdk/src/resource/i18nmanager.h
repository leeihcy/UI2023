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

    // xml����¼�����ڶ�̬�л����ԣ���༭���У�
    std::list<UIElement*>  m_listElement;
};



}