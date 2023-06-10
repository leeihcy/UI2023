#pragma once

#include "Src\Helper\topwindow\topwindowmanager.h"
#include "Src\Helper\tooltip\tooltipmanager.h"
#include "Src\Helper\msg\msghelper.h"
#include "Src\Resource\skinmanager.h"
#include "Src\Helper\MouseKeyboard\mousemanager.h"
#include "Src\Helper\layout\layout_factory.h"
#include "Src\Renderbase\renderbase\renderbase_factory.h"
#include "Src\Renderbase\textrenderbase\textrender_factory.h"
#include "Src\Animate\wrap\animate.h"


namespace UI
{
class GifTimerManager;
interface IObjectDescription;
class ImageManager;
class ImageRes;
class CursorRes;
class GifRes;
class FontManager;
class FontRes;
class ColorManager;
class ColorRes;
class StyleManager;
class StyleRes;
class LayoutManager;
struct IUIAutoTest;
//
//  PS: 20130110 考虑到在UIEditor中会存在两份UIApplication实例，因此不能再将UIApplication作为全局对象来实现了。
//  PS: 20130317 考虑到以后的扩展和升级，不将接口做成纯虚函数，而是采用Pimpl实现
//
class UIApplication
{
public:
	UIApplication(IUIApplication* p);
	~UIApplication(void);
    void  x_Init();  // 内部初始化，避免在构造函数中调用太多东西

	void  SetEditorMode(bool b) { m_bEditorMode = b; }
	bool  IsEditorMode() { return m_bEditorMode; }
	void  SetUIEditorPtr(IUIEditor* p) { m_pUIEditor = p; }
	IUIEditor*  GetUIEditorPtr() { return m_pUIEditor; }
	IUIAutoTest*  GetUIAutoTestPtr() { return m_pUIAutoTest; }
	
	SkinManager&  GetSkinMgr();
	ITopWindowManager*  GetITopWindowMgr();
	TopWindowManager*   GetTopWindowMgr() { return &m_TopWindowMgr; }
	UIA::IAnimateManager*   GetAnimateMgr();
	GifTimerManager*    GetGifTimerMgr() { return m_pGifTimerMgr; }
	IWaitForHandlesMgr*  GetWaitForHandlesMgr() { return &m_WaitForHandlesMgr; }
	IMessageFilterMgr*  GetMessageFilterMgr() { return &m_MsgFilterMgr; }
	HMODULE  GetUID2DModule();
	HMODULE  GetUID3DModule();

	RenderBaseFactory&  GetRenderBaseFactory() { return m_renderBaseFactory; }
	TextRenderFactory&  GetTextRenderFactroy() { return m_textRenderFactroy; }
	LayoutFactory&  GetLayoutFactory() { return m_layoutFactory; }

	LPCTSTR  GetRenderBaseName(int nType);
	bool  CreateRenderBaseByName(
				LPCTSTR szName, 
                IObject* pObject, 
                IRenderBase** ppOut);

	SkinRes*  GetDefaultSkinRes();
	
	bool  ShowToolTip(TOOLTIPITEM* pItem);
	void  HideToolTip();

    void  RestoreRegisterUIObject();
    bool  RegisterControlTagParseFunc(LPCTSTR szTag, pfnParseControlTag func);
    bool  GetSkinTagParseFunc(LPCTSTR szTag, pfnParseSkinTag* pFunc);
    bool  GetControlTagParseFunc(LPCTSTR szTag, pfnParseControlTag* pFunc);

    bool  RegisterUIObject(IObjectDescription*);
	IObject*  CreateUIObjectByName(LPCTSTR szXmlName, ISkinRes*);
    IObject*  CreateUIObjectByClsid(REFCLSID clsid, ISkinRes*);
    void  LoadUIObjectListToToolBox();

	BOOL  IsDialogMessage(MSG* pMsg);
	void  MsgHandleLoop(bool* pbQuitLoopRef);
	void  MsgHandleOnce();
	void  RedrawTopWindows() { m_TopWindowMgr.UpdateAllWindow(); }
	bool  IsUnderXpOS();
	bool  IsVistaOrWin7etc();
	HWND  GetForwardPostMessageWnd();

    // gpu 
    bool  IsGpuCompositeEnable();
    bool  EnableGpuComposite();
	void  ShutdownGpuCompositor();

public:
    IUIApplication*  GetIUIApplication() { return m_pUIApplication; }

private:
    void  RegisterWndClass();
    void  RegisterDefaultUIObject();
    void  ClearRegisterUIObject();

public:

private:
    typedef vector<IObjectDescription*>       UIOBJ_CREATE_DATA;
    typedef map<String, pfnParseSkinTag>      UISKINTAGPARSE_DATA;
    typedef map<String, pfnParseControlTag>   UICONTROLTAGPARSE_DATA;

    IUIApplication*     m_pUIApplication;              // 对外提供的接口
    OSVERSIONINFOEX     m_osvi;                        // 操作系统版本
	IUIEditor*          m_pUIEditor;                   // 外部的编辑器指针，用于消息通知和数据获取
	IUIAutoTest*        m_pUIAutoTest;                 // 外部的自动化测试指针。

    // gpu合成
    bool  m_bGpuEnable;
    // 是否是设计模式
    bool  m_bEditorMode;

	UIOBJ_CREATE_DATA         m_vecUIObjectDesc;       // 保存UI对象的XML字符串，用于从字符串创建UI对象
    UISKINTAGPARSE_DATA       m_mapSkinTagParseData;
    UICONTROLTAGPARSE_DATA    m_mapControlTagParseData;

	RenderBaseFactory  m_renderBaseFactory;
	TextRenderFactory  m_textRenderFactroy;
	LayoutFactory  m_layoutFactory;

    TopWindowManager    m_TopWindowMgr; 
    SkinManager         m_skinMgr;       // 需要提前释放，做成指针模式
	GifTimerManager*    m_pGifTimerMgr;

    WaitForHandlesMgr   m_WaitForHandlesMgr;
    ForwardPostMessageWindow  m_WndForwardPostMsg; 
    MessageFilterMgr    m_MsgFilterMgr;
    ToolTipManager      m_ToolTipMgr;
    AnimateHelper       m_animate; 
};

}

