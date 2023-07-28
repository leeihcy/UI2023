#ifndef _UI_SDK_SRC_BASE_UIAPPLICATION_H_
#define _UI_SDK_SRC_BASE_UIAPPLICATION_H_

#include <map>
#include <vector>
#include "interface/iuiapplication.h"
#include "message_loop/message_loop.h"
#include "resource/resource_manager.h"
#include "src/render/renderbase_factory.h"
#include "src/render/textrender/textrender_factory.h"
#include "src/layout/layout_factory.h"
#include "src/helper/topwindow/topwindowmanager.h"
#include "src/animate/wrap/animate.h"
#include "src/helper/msg/msghelper.h"
// #include "src/helper/tooltip/tooltipmanager.h"
// #include "ui/UISDK/Src/Animate/wrap/animate.h

namespace ui
{

class GifTimerManager;
// struct IObjectDescription;
// class ImageManager;
// class ImageRes;
// class CursorRes;
// class GifRes;
// class FontManager;
// class FontRes;
// class ColorManager;
// class ColorRes;
// class StyleManager;
// class StyleRes;
// class LayoutManager;
struct IUIAutoTest;

class Application
{
public:  
    Application(IApplication* p);
    ~Application();
    IApplication*  GetIUIApplication() { return m_pUIApplication; }

    void Run();
    void Quit();

    ResourceManager&  GetResourceManager();

    void  SetEditorMode(bool b) { m_bEditorMode = b; }
	bool  IsEditorMode() { return m_bEditorMode; }
	void  SetUIEditorPtr(IUIEditor* p) { m_pUIEditor = p; }
	IUIEditor*  GetUIEditorPtr() { return m_pUIEditor; }

    void  x_Init();  // 内部初始化，避免在构造函数中调用太多东西

	IUIAutoTest*  GetUIAutoTestPtr() { return m_pUIAutoTest; }
	
	ITopWindowManager*  GetITopWindowMgr();
	TopWindowManager*   GetTopWindowMgr() { return &m_TopWindowMgr; }
	uia::IAnimateManager*   GetAnimateManager();
#if 0    
    GifTimerManager*    GetGifTimerMgr() { return m_pGifTimerMgr; }
    IWaitForHandlesMgr*  GetWaitForHandlesMgr() { return &m_WaitForHandlesMgr; }
	IMessageFilterMgr*  GetMessageFilterMgr() { return &m_MsgFilterMgr; }
	HMODULE  GetUID2DModule();
	HMODULE  GetUID3DModule();
#endif
	RenderBaseFactory&  GetRenderBaseFactory() { return m_renderBaseFactory; }
	TextRenderFactory&  GetTextRenderFactroy() { return m_textRenderFactroy; }
	LayoutFactory&  GetLayoutFactory() { return m_layoutFactory; }

	const wchar_t*  GetRenderBaseName(int nType);
	bool  CreateRenderBaseByName(
				const wchar_t* szName, 
                IObject* pObject, 
                IRenderBase** ppOut);

	ResBundle*  GetDefaultSkinRes();
#if 0
	bool  ShowToolTip(TOOLTIPITEM* pItem);
	void  HideToolTip();
#endif
    void  RestoreRegisterUIObject();
    bool  RegisterControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag func);
    bool  GetSkinTagParseFunc(const wchar_t* szTag, pfnParseSkinTag* pFunc);
    bool  GetControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag* pFunc);
    bool  RegisterUIObject(IObjectDescription*);
	IObject*  CreateUIObjectByName(const wchar_t* szXmlName, IResBundle*);
    IObject*  CreateUIObjectByClsid(const Guid& clsid, IResBundle*);

    void  LoadUIObjectListToToolBox();
#if 0
	bool  IsDialogMessage(MSG* pMsg);
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

private:
    void  RegisterWndClass();
#endif
    void  RegisterDefaultUIObject();
    void  ClearRegisterUIObject();
#if 0
public:

private:
    // gpu合成
    bool  m_bGpuEnable;
#endif
    typedef std::vector<IObjectDescription*>       UIOBJ_CREATE_DATA;
    typedef std::map<String, pfnParseSkinTag>      UISKINTAGPARSE_DATA;
    typedef std::map<String, pfnParseControlTag>   UICONTROLTAGPARSE_DATA;

	UIOBJ_CREATE_DATA         m_vecUIObjectDesc;       // 保存UI对象的XML字符串，用于从字符串创建UI对象
    UISKINTAGPARSE_DATA       m_mapSkinTagParseData;
    UICONTROLTAGPARSE_DATA    m_mapControlTagParseData;

	RenderBaseFactory  m_renderBaseFactory;
	TextRenderFactory  m_textRenderFactroy;
	LayoutFactory  m_layoutFactory;

    TopWindowManager    m_TopWindowMgr; 

#if defined(OS_WIN)
	GifTimerManager*    m_pGifTimerMgr;
    ToolTipManager      m_ToolTipMgr;
    WaitForHandlesMgr   m_WaitForHandlesMgr;
    MessageFilterMgr    m_MsgFilterMgr;
    ForwardPostMessageWindow  m_WndForwardPostMsg;     
#endif
    AnimateHelper       m_animate; 

private:

    IApplication*     m_pUIApplication = nullptr;              // 对外提供的接口
    // OSVERSIONINFOEX     m_osvi;                        // 操作系统版本
	IUIEditor*          m_pUIEditor;                   // 外部的编辑器指针，用于消息通知和数据获取
	IUIAutoTest*        m_pUIAutoTest;                 // 外部的自动化测试指针。

    // 是否是编辑器模式
    bool  m_bEditorMode = false;

private:
    MessageLoop m_message_loop;
    ResourceManager m_resource_manager;
    
};
} // namespace ui


#endif