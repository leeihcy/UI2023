#pragma once

/*
 *  2015.7.19 
 *
 *  1. 废弃了类似TTPlayer形式的多皮肤功能，同步维护所有的皮肤成本太大，
 *     不利于软件的更新，更好的方式是一套皮肤，多个配置文件。
 *
 *  2. 扩展支持插件模式的多皮肤包，每个插件允许的自己的皮肤包，包肤包也
 *     可以有继承关系。
 */
namespace UI
{
	interface ISkinManager;
class UIApplication;
class IUISkinDescParse;
enum  SKIN_PACKET_TYPE;
class SkinRes;

class SkinManager
{
public:
	SkinManager(); 
	~SkinManager(); 
    void  Destroy();

public:
	ISkinManager&  GetISkinManager();
	UIApplication*  GetUIApplication();
	void  SetUIApplication(UIApplication* pUIApp);

	SkinRes*  LoadSkinRes(LPCTSTR szPath);
	SkinRes*  LoadSkinRes(HINSTANCE hInstance, int resId=-1);
	SkinRes*  GetDefaultSkinRes();
	SkinRes*  GetSkinResByName(LPCTSTR szName);
	uint  GetSkinResCount();
	SkinRes*  GetSkinResByIndex(uint i);

	bool  Save(SkinRes* pSkinRes=nullptr);
    void  ChangeSkinHLS( short h, short l, short s, int nFlag );

	LPCTSTR  GetCurrentLanguage();
    void  SetCurrentLanguage(LPCTSTR);

private:
    ISkinManager*  m_pISkinManager;

	// 数据持久层
//  String  m_strSkinDir;
	bool  m_bDirty;

	// 持久性属性对象
	// SkinBuilderRes    m_SkinBuilderRes;

	// 运行时数据
	vector<SkinRes*>  m_vSkinRes;           // 所有的皮肤列表

	// TODO: 运行过程中如何切换语言
	String  m_strLanguage;   // 当前使用的语言

	UIApplication*    m_pUIApplication;
};
}

