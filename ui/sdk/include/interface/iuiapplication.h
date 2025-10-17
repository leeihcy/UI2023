#ifndef _IUIAPPLICATION_H_
#define _IUIAPPLICATION_H_
#include "sdk/include/interface.h"
#include "sdk/include/interface/ilayout.h"
#include "sdk/include/macro/uidefine.h"
#include "sdk/include/uiapi.h"
#include "sdk/include/common/signalslot/slot.h"
// #include <functional>

namespace uia {
struct IAnimate;
}
namespace ui {
class Application;
struct ILayout;
struct IUIEditor;
struct IResource;
struct IResourceManager;
struct IImageManager;
struct IImageRes;
struct ICursorRes;
struct IGifRes;
struct IFontManager;
struct IFontRes;
struct IColorManager;
struct IColorRes;
struct IStyleManager;
struct IStyleRes;
struct ILayoutManager;
struct ILayoutRes;
struct IMeta;
struct IRenderBaseMeta;
struct ITextRenderBaseMeta;
struct IWindowBase;
struct ITopWindowManager;
struct IRenderBase;
struct ITextRenderBase;
struct IMessageFilterMgr;
struct TOOLTIPITEM;
class TimerItem;
typedef void* TimerID;

using IApplicationPtr =
    std::unique_ptr<ui::IApplication, void (*)(IApplication *)>;

struct UIAPI IApplication {
  static IApplicationPtr create();
  friend void destroy(IApplication *p);

private:
  // 禁止外部继承该类，只能通过create创建。
  // ApplicationPtr已封装了销毁逻辑。
  IApplication();

protected:
  // 只能通过内部的destroy函数销毁。
  ~IApplication();

public:
  Application *GetImpl();

  void Run();
  void Quit();

  IResource *RootBundle();
  IResource *LoadResource(const char *szSkinPath);
  IResource *LoadResource(llong hInstance, int resId = -1);

  void SetEditorMode(bool b);
  bool IsEditorMode();
  void SetUIEditorPtr(IUIEditor *);
  IUIEditor *GetUIEditorPtr();

  IResourceManager &GetResourceManager();
  ITopWindowManager *GetTopWindowMgr();
  uia::IAnimate *GetAnimate();
  // IMessageFilterMgr*  GetMessageFilterMgr();

  IResource *GetDefaultSkinRes();

  void RestoreRegisterUIObject();
  bool RegisterControlTagParseFunc(const char *szTag, pfnParseControlTag func);
  bool GetSkinTagParseFunc(const char *szTag, pfnParseSkinTag *pFunc);
  bool GetControlTagParseFunc(const char *szTag, pfnParseControlTag *pFunc);

  IObject *CreateUIObjectByName(const char *szName, IResource *pISkinRes);
  IObject *CreateUIObjectByClsid(const Uuid &clsid, IResource *pISkinRes);
  bool RegisterUIObject(IMeta *p);
  void LoadUIObjectListToToolBox();

  bool RegisterUIRenderBase(IRenderBaseMeta& meta);
  std::shared_ptr<IRenderBase> CreateRenderBase(int nType, IObject *pObject);
  void EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, llong wParam,
                          llong lParam);

  bool RegisterUITextRender(ITextRenderBaseMeta &meta);
  std::shared_ptr<ITextRenderBase> CreateTextRenderBaseByName(const char *szName, IObject *pObject);
  std::shared_ptr<ITextRenderBase> CreateTextRenderBase(int nType, IObject *pObject);
  void EnumTextRenderBaseName(pfnEnumTextRenderNameCallback callback,
                              llong wParam, llong lParam);

  bool CreateLayoutByName(const char *szName, IObject *pObject,
                          bool bCreateDefault, ILayout **ppOut);
  void EnumLayoutType(pfnEnumLayoutTypeCallback callback, llong wParam,
                      llong lParam);
  bool RegisterLayout(const char *name, pfnUICreateLayoutPtr pfn);

#if 0
    bool  IsDialogMessage(MSG* pMsg);
#endif
  void MsgHandleLoop(bool *pbQuitLoopRef = nullptr);
  void RedrawTopWindows();

  bool ShowToolTip(TOOLTIPITEM *pItem);
  void HideToolTip();

  TimerID SetTimer(int elapse, slot<bool(TimerID)>&& timer_callback);
  void KillTimer(TimerID timer_id);
  // unsigned int SetTimerById(int nElapse, unsigned int nId, IMessage *pNotify);
  // unsigned int SetTimer(int nElapse, TimerItem *pTimerItem);
  // void KillTimerById(int nId, IMessage *pNotify);
  // void KillTimerByNotify(IMessage *pNotify);

#if 0
    HDC   GetCacheDC();
    void  ReleaseCacheDC(HDC hDC);
    HBITMAP  GetCacheBitmap(int nWidth, int nHeight);  // 注：不要释放该HBITMAP，由内部维护
    void  ShadowBlur(HBITMAP hBitmap, Color colorShadow, Rect* prcBlur, int nRadius);
#endif
  bool IsUnderXpOS();
  bool IsVistaOrWin7etc();
  bool IsAeroEnable();
  bool IsHardwareCompositeEnable();
  bool EnableHardwareComposite();
#if 0
  HWND GetForwardPostMessageWnd();
  IWindowBase *GetWindowBaseFromHWND(HWND hWnd);
#endif
private:
  Application *m_pImpl;
};

struct ApplicationPtr : public IApplicationPtr {
  ApplicationPtr() : IApplicationPtr(IApplication::create()) {}
};

} // namespace ui
#endif // _IUIAPPLICATION_H_