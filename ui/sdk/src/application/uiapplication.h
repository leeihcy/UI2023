#ifndef _UI_SDK_SRC_BASE_UIAPPLICATION_H_
#define _UI_SDK_SRC_BASE_UIAPPLICATION_H_

#include "include/interface/ianimate.h"
#include "include/interface/iuiapplication.h"
#include "src/message_loop/message_loop.h"
#include "src/resource/resource_manager.h"
#include "src/animate/animate.h"
#include "src/animate/wrap/animate_wrap.h"
#include "src/helper/topwindow/topwindowmanager.h"
#include "src/helper/timer/timer.h"
#include "src/layout/layout_factory.h"
#include "src/render/renderbase_factory.h"
#include "src/render/textrender/textrender_factory.h"
#include "src/thread/render_thread.h"
#include <map>
#include <vector>
#if defined(OS_WIN)
#include "src/util/windows.h"
#endif

namespace ui {

class GifTimerManager;
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
  void CreateAnimateTimer(int fps);
  void DestroyAnimateTimer();
  void OnAnimateTimer();
  
#if 0    
  GifTimerManager*    GetGifTimerMgr() { return m_pGifTimerMgr; }
	HMODULE  GetUID2DModule();
	HMODULE  GetUID3DModule();
#endif
  RenderBaseFactory &GetRenderBaseFactory() { return m_render_base_factory; }
  TextRenderFactory &GetTextRenderFactroy() { return m_text_render_base_factory; }
  LayoutFactory &GetLayoutFactory() { return m_layout_factory; }
  TimerHelper &GetTimerHelper() { return m_timer_helper; }

  const char *GetRenderBaseName(int nType);
  std::shared_ptr<IRenderBase> CreateRenderBaseByName(IResourceBundle *resource, const char *name);

  ResourceBundle *GetDefaultSkinRes();
#if 0
	bool  ShowToolTip(TOOLTIPITEM* pItem);
	void  HideToolTip();
#endif
  void RestoreRegisterUIObject();
  bool RegisterControlTagParseFunc(const char *szTag,
                                   pfnParseControlNode func);
  bool GetSkinTagParseFunc(const char *szTag, pfnParseResourceNode *pFunc);
  bool GetControlTagParseFunc(const char *szTag, pfnParseControlNode *pFunc);
  bool RegisterUIObject(IMeta *);
  IObject *CreateUIObjectByName(const char *szXmlName, IResourceBundle *);
  IObject *CreateUIObjectByUUID(const Uuid &clsid, IResourceBundle *);

  IRenderTarget *CreateRenderTarget(eGraphicsLibraryType lib_type);
  std::shared_ptr<IRenderBitmap>
  CreateRenderBitmap(eGraphicsLibraryType lib_type, IMAGE_ITEM_TYPE eType);

  void LoadUIObjectListToToolBox();

  // gpu
  bool IsHardwareCompositeEnable();
  bool EnableHardwareComposite();
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

private:
  // 保存UI对象的XML字符串，用于从字符串创建UI对象
  std::vector<IMeta *> m_object_meta_array;
  std::map<std::string, pfnParseResourceNode> m_resource_node_parser_map;
  std::map<std::string, pfnParseControlNode> m_control_node_parser_map;

  RenderBaseFactory m_render_base_factory;
  TextRenderFactory m_text_render_base_factory;
  LayoutFactory m_layout_factory;

  TopWindowManager m_TopWindowMgr;
  TimerHelper m_timer_helper;

#if 0 // defined(OS_WIN)
  GifTimerManager *m_pGifTimerMgr;
  ToolTipManager m_ToolTipMgr;
#endif

  uia::Animate m_animate;

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