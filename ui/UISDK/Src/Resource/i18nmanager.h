#pragma once
#include "i18nres.h"

namespace UI
{
interface UIElement;
interface IUIElement;

class I18nManager
{
public:
	I18nManager(SkinRes* p) : m_i18nRes(p) {
		m_pSkinRes = p;
	}
    ~I18nManager();

	I18nRes&  GetI18nRes() {
		return m_i18nRes;
	}

	static HRESULT  UIParseI18nTagCallback(IUIElement*, ISkinRes* pSkinRes);
	HRESULT  ParseNewElement(UIElement*);

    void  Reload();
    
private:
    void  parse(UIElement*);

private:
	I18nRes  m_i18nRes;
	SkinRes*  m_pSkinRes;

    // xml结点记录，用于动态切换语言（如编辑器中）
    list<UIElement*>  m_listElement;
};



}