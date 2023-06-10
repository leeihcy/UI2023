#pragma once
#include "Inc\Interface\iuires.h"

namespace UI
{
class I18nRes : public II18nCallback
{
public:
	I18nRes(SkinRes*);
	~I18nRes();

public:
	void  Add(LPCTSTR szId, LPCTSTR szValue);

    LPCTSTR  MapConfigValue(LPCTSTR configText);
    void  Clear();

	II18nRes&  GetII18nRes();

private:
    virtual LPCTSTR Map(LPCTSTR key) override;

private:
	SkinRes*  m_pSkinRes;
	II18nRes*  m_pII18nRes;

	map<String, String>  m_dict;
};
}