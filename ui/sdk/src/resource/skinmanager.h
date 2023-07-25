#pragma once
#include <vector>
#include <string>

/*
 *  2015.7.19 
 *
 *  1. 废弃了类似TTPlayer形式的多皮肤功能，同步维护所有的皮肤成本太大，
 *     不利于软件的更新，更好的方式是一套皮肤，多个配置文件。
 *
 *  2. 扩展支持插件模式的多皮肤包，每个插件允许的自己的皮肤包，包肤包也
 *     可以有继承关系。
 */
namespace ui
{
struct ISkinManager;
class UIApplication;
class IUISkinDescParse;
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

	SkinRes*  LoadSkinRes(const wchar_t* szPath);
	SkinRes*  LoadSkinRes(long hInstance, int resId=-1);
	SkinRes*  GetDefaultSkinRes();
	SkinRes*  GetSkinResByName(const wchar_t* szName);
	unsigned int  GetSkinResCount();
	SkinRes*  GetSkinResByIndex(unsigned int i);

	bool  Save(SkinRes* pSkinRes=nullptr);
    void  ChangeSkinHLS( short h, short l, short s, int nFlag );

	const wchar_t*  GetCurrentLanguage();
    void  SetCurrentLanguage(const wchar_t*);

private:
    ISkinManager*  m_pISkinManager;

	// 数据持久层
//  String  m_strSkinDir;
	bool  m_bDirty;

	// 持久性属性对象
	// SkinBuilderRes    m_SkinBuilderRes;

	// 运行时数据
	std::vector<SkinRes*>  m_vSkinRes;           // 所有的皮肤列表

	// TODO: 运行过程中如何切换语言
	std::wstring  m_strLanguage;   // 当前使用的语言

	UIApplication*    m_pUIApplication;
};
}

