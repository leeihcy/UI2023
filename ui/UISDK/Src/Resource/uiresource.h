#pragma once

namespace UI
{
class SkinRes;
class Object;

class OutCtrlDllItem
{
public:
	OutCtrlDllItem();
	~OutCtrlDllItem();

public:
	HMODULE hModule;
	String  strDllPath;
};
class SkinBuilderRes
{
public:
	SkinBuilderRes(){m_pUIApp = NULL;};
	~SkinBuilderRes() {Clear();}

	void  SetUIApplication(IUIApplication* pUIApp){ m_pUIApp = pUIApp; }
	void  AddOuterCtrlsDllPath(const String&  strDll);
	OutCtrlDllItem*  FindOuterCtrlsDll(const String& strDll);
	OutCtrlDllItem*  FindOuterCtrlsDll(HMODULE hModule);
	void  Clear();

	bool  Register(OutCtrlDllItem*);

private:
	IUIApplication*  m_pUIApp;
	vector<OutCtrlDllItem*>   m_vecOuterCtrlsDll;   // 外部控件所在的DLL，用于注册外部部件创建
};

// 当前皮肤的色调数据
struct SKIN_HLS_INFO
{
	short  h;       // -120 ~ 120
	short  l;       // -100 ~ 100
	short  s;       // -100 ~ 100
	int   nFlag;
};


interface ISkinConfigRes : public IRootInterface
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
	String   m_strImageXmlPath;       // 全路径，保存到文件中的才是相对路径 
	String   m_strFontXmlPath;
	String   m_strColorXmlPath;
	String   m_strStyleXmlPath;
	String   m_strLayoutXmlPath;
	String   m_strI18nXmlPath;

	SKIN_HLS_INFO  m_sHlsInfo;
public:

	void    SetHLS(char h, char l, char s, int nFlag);
	virtual SKIN_HLS_INFO* GetSkinHlsInfo() { return &m_sHlsInfo; }
};


}

