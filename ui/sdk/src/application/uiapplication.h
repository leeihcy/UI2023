#ifndef _UI_SDK_SRC_BASE_UIAPPLICATION_H_
#define _UI_SDK_SRC_BASE_UIAPPLICATION_H_

#include "include/interface/ianimate.h"
#include "include/interface/iuiapplication.h"
#include "src/message_loop/message_loop.h"
#include "src/resource/resource_manager.h"
#include "src/animate/wrap/animate_wrap.h"
#include "src/helper/topwindow/topwindowmanager.h"
#include "src/layout/layout_factory.h"
#include "src/render/renderbase_factory.h"
#include "src/render/textrender/textrender_factory.h"
#include <map>
#include <vector>
// #include "src/helper/tooltip/tooltipmanager.h"
// #include "ui/UISDK/Src/Animate/wrap/animate.h

namespace ui {

class GifTimerManager;
// struct IMeta;
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

class Application {
public:
  Application(IApplication *p);
  ~Application();
  IApplication *GetIUIApplication() { return m_pUIApplication; }

  void Run();
  void Quit();

  MessageLoop& GetMessageLoop();
  ResourceManager &GetResourceManager();

  void SetEditorMode(bool b) { m_bEditorMode = b; }
  bool IsEditorMode() { return m_bEditorMode; }
  void SetUIEditorPtr(IUIEditor *p) { m_pUIEditor = p; }
  IUIEditor *GetUIEditorPtr() { return m_pUIEditor; }

  void x_Init(); // 内部初始化，避免在构造函数中调用太多东西

  IUIAutoTest *GetUIAutoTestPtr() { return m_pUIAutoTest; }

  ITopWindowManager *GetITopWindowMgr();
  TopWindowManager *GetTopWindowMgr() { return &m_TopWindowMgr; }
  
  uia::IAnimate *GetAnimate();
  void CreateAnimateTimer();
  void DestroyAnimateTimer();
  void OnAnimateTimer();
  
#if 0    
  GifTimerManager*    GetGifTimerMgr() { return m_pGifTimerMgr; }
	HMODULE  GetUID2DModule();
	HMODULE  GetUID3DModule();
#endif
  RenderBaseFactory &GetRenderBaseFactory() { return m_renderBaseFactory; }
  TextRenderFactory &GetTextRenderFactroy() { return m_textRenderFactroy; }
  LayoutFactory &GetLayoutFactory() { return m_layoutFactory; }

  const char *GetRenderBaseName(int nType);
  bool CreateRenderBaseByName(const char *szName, IObject *pObject,
                              IRenderBase **ppOut);

  Resource *GetDefaultSkinRes();
#if 0
	bool  ShowToolTip(TOOLTIPITEM* pItem);
	void  HideToolTip();
#endif
  void RestoreRegisterUIObject();
  bool RegisterControlTagParseFunc(const char *szTag,
                                   pfnParseControlTag func);
  bool GetSkinTagParseFunc(const char *szTag, pfnParseSkinTag *pFunc);
  bool GetControlTagParseFunc(const char *szTag, pfnParseControlTag *pFunc);
  bool RegisterUIObject(IMeta *);
  IObject *CreateUIObjectByName(const char *szXmlName, IResource *);
  IObject *CreateUIObjectByClsid(const Uuid &clsid, IResource *);

  void LoadUIObjectListToToolBox();

  // gpu
  bool IsGpuCompositeEnable();
  bool EnableGpuComposite();
  void ShutdownGpuCompositor();

#if 0
	void  MsgHandleLoop(bool* pbQuitLoopRef);
	void  MsgHandleOnce();
	void  RedrawTopWindows() { m_TopWindowMgr.UpdateAllWindow(); }
	HWND  GetForwardPostMessageWnd();
#endif
#if defined(OS_WIN)
	bool  IsUnderXpOS();
	bool  IsVistaOrWin7etc();
#endif
  void RegisterDefaultUIObject();
  void ClearRegisterUIObject();

public:
private:
  // gpu合成总开关
  bool m_bGpuEnable = false;

  typedef std::map<std::string, pfnParseSkinTag> UISKINTAGPARSE_DATA;
  typedef std::map<std::string, pfnParseControlTag> UICONTROLTAGPARSE_DATA;

  // 保存UI对象的XML字符串，用于从字符串创建UI对象
  std::vector<IMeta *> m_vecUIObjectDesc;
  UISKINTAGPARSE_DATA m_mapSkinTagParseData;
  UICONTROLTAGPARSE_DATA m_mapControlTagParseData;

  RenderBaseFactory m_renderBaseFactory;
  TextRenderFactory m_textRenderFactroy;
  LayoutFactory m_layoutFactory;

  TopWindowManager m_TopWindowMgr;

#if 0 // defined(OS_WIN)
  GifTimerManager *m_pGifTimerMgr;
  ToolTipManager m_ToolTipMgr;
#endif

  std::unique_ptr<uia::Animate> m_animate;

private:
  IApplication *m_pUIApplication = nullptr; // 对外提供的接口
  
  IUIEditor *m_pUIEditor; // 外部的编辑器指针，用于消息通知和数据获取
  IUIAutoTest *m_pUIAutoTest; // 外部的自动化测试指针。

#if defined(OS_WIN)
  // 操作系统版本
  OSVERSIONINFOEX     m_osvi;    
#endif

  // 是否是编辑器模式
  bool m_bEditorMode = false;

private:
  MessageLoop m_message_loop;
  ResourceManager m_resource_manager;
};
} // namespace ui

#endif