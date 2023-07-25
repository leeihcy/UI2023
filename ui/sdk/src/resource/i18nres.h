#pragma once
#include <map>

#include "include/interface/iuires.h"

namespace ui
{
class SkinRes;
class I18nRes : public II18nCallback
{
public:
	I18nRes(SkinRes*);
	~I18nRes();

public:
	void  Add(const wchar_t* szId, const wchar_t* szValue);

    const wchar_t*  MapConfigValue(const wchar_t* configText);
    void  Clear();

	II18nRes&  GetII18nRes();

private:
    virtual const wchar_t* Map(const wchar_t* key) override;

private:
	SkinRes*  m_pSkinRes;
	II18nRes*  m_pII18nRes;

	std::map<std::wstring, std::wstring>  m_dict;
};
}