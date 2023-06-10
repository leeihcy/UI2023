#pragma once
#include "Src\SkinParse\xml\xmlwrap.h"
#include "..\Util\DataStruct\list.h"

namespace UI
{
class Object;
interface UIElement;
interface ILayoutWindowNodeList;


//  延迟加载的布局配置
//  一个xml配置中，允许配置多个相关联的窗口、自定义列表项
class LayoutConfigItem
{
public:
    void  SetPath(LPCTSTR szPath);
    LPCTSTR  GetPath();

    void  AddWindow(LPCTSTR szName);
    bool  FindWindow(LPCTSTR szName);

    void  AddListItem(LPCTSTR szName);
    bool  FindListItem(LPCTSTR szName);

    bool  Find(LPCTSTR szTagName, LPCTSTR szName);

    UINT  GetWindowCount();
    LPCTSTR  GetWindowName(UINT index);

private:
	String  m_strPath;
    vector<String>  m_vecWindow;
    vector<String>  m_vecListItem;
};

// 新加载的子控件的notify设置为谁
#define NOTIFY_TARGET_ROOT  (IMessage*)1  // 窗口（根对象）
#define NOTIFY_TARGET_NULL  (IMessage*)0  // 不指定

class LayoutManager
{
public:
	LayoutManager(SkinRes*);
	~LayoutManager(void);
	
    ILayoutManager&  GetILayoutManager();

    Object*  LoadPluginLayout(
                LPCTSTR szId, 
                Object* pNewParemt, 
                IMessage* pNotifyTarget = NOTIFY_TARGET_ROOT);

	UIElementProxy  FindWindowElement(LPCTSTR szTagName, LPCTSTR szId);
    UIElementProxy  FindListItemElement(LPCTSTR szId);

	enum PARSEFLAG
	{
		PARSEFLAG_LOADCHILD = 1,
		PARSEFLAG_RELOAD = 2,
	};
    Object*  ParseElement(
                UIElement* pUIElement, 
                Object* pParent, 
                IMessage* pNotifyTarget = NOTIFY_TARGET_ROOT,
				int flags = PARSEFLAG_LOADCHILD);
    Object*  ParseChildElement(
                UIElement* pParentElement, 
                Object* pParent,
                IMessage* pNotifyTarget = NOTIFY_TARGET_ROOT,
				int flags = PARSEFLAG_LOADCHILD);

	bool  ReLoadLayout(
                Object* pRootObj, 
				LPCTSTR szNewLayoutId,
                map<String, Object*>& listAllChild);
	void  ReloadChildObjects(
                Object* pObjParent,
                UIElement* pObjElement,
                map<String, Object*>& listAllChild);

    static HRESULT  UIParseLayoutTagCallback(
                IUIElement*, ISkinRes* pSkinRes);
	static HRESULT  UIParseLayoutConfigTagCallback(
                IUIElement*, ISkinRes* pSkinRes);
	
	// 编辑器专用函数
	bool  LoadWindowNodeList(ILayoutWindowNodeList** pp);

private:
    void  ParseNewElement(UIElement* pElem);
	void  ParseLayoutConfigTag(UIElement* pElem);
    bool  ParseLayoutConfigFileTag(UIElement* pElem);

    UIElementProxy  find_element_from_cache(LPCTSTR szTagName, LPCTSTR szId);
	UIElementProxy  load_window_by_id(LPCTSTR szTagName, LPCTSTR szId);
	UIElementProxy  load_element_from_layout_config(LPCTSTR szTagName, LPCTSTR szId);
	bool  testUIElement(UIElement* pParentElem, LPCTSTR szTagName, LPCTSTR szId);

private:
    ILayoutManager*  m_pILayoutManager;
    UIList<UIElement*>  m_listUIElement;  // 已加载的布局项<layout>结点列表
	UIList<LayoutConfigItem*>  m_listLayoutConfig;  // 还未加载的布局项

	// 对象属性
	SkinRes*         m_pSkinRes;
};

}