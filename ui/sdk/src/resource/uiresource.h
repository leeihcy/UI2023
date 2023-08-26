#pragma once
#include "include/inc.h"

namespace ui
{
class Resource;
class Object;

class OutCtrlDllItem
{
public:
	OutCtrlDllItem();
	~OutCtrlDllItem();

public:
	HMODULE hModule;
	std::wstring  strDllPath;
};
class SkinBuilderRes
{
public:
	SkinBuilderRes(){m_pUIApp = NULL;};
	~SkinBuilderRes() {Clear();}

	void  SetUIApplication(IApplication* pUIApp){ m_pUIApp = pUIApp; }
	void  AddOuterCtrlsDllPath(const std::string&  strDll);
	OutCtrlDllItem*  FindOuterCtrlsDll(const std::string& strDll);
	OutCtrlDllItem*  FindOuterCtrlsDll(HMODULE hModule);
	void  Clear();

	bool  Register(OutCtrlDllItem*);

private:
	IApplication*  m_pUIApp;
	std::vector<OutCtrlDllItem*>   m_vecOuterCtrlsDll;   // 外部控件所在的DLL，用于注册外部部件创建
};

// 当前皮肤的色调数据
struct SKIN_HLS_INFO
{
	short  h;       // -120 ~ 120
	short  l;       // -100 ~ 100
	short  s;       // -100 ~ 100
	int   nFlag;
};


struct ISkinConfigRes : public IRootInterface
{
	virtual SKIN_HLS_INFO* GetSkinHlsInfo()= 0;
	virtual void SetHLS(char h, char l, char s, int nFlag) = 0;
};
//
//	皮肤信息描述，各元素xml位置 
//
class SkinConfigRes : public ISkinConfigRes
{
public:
	SkinConfigRes();

private:
	std::wstring   m_strImageXmlPath;       // 全路径，保存到文件中的才是相对路径 
	std::wstring   m_strFontXmlPath;
	std::wstring   m_strColorXmlPath;
	std::wstring   m_strStyleXmlPath;
	std::wstring   m_strLayoutXmlPath;
	std::wstring   m_strI18nXmlPath;

	SKIN_HLS_INFO  m_sHlsInfo;
public:

	void    SetHLS(char h, char l, char s, int nFlag);
	virtual SKIN_HLS_INFO* GetSkinHlsInfo() { return &m_sHlsInfo; }
};


}

