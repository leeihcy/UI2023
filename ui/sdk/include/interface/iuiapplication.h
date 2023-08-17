#ifndef _IUIAPPLICATION_H_
#define _IUIAPPLICATION_H_
#include "sdk/include/common/ptr/unique_ptr.h"
#include "sdk/include/common/uuid/uuid.h"
#include "sdk/include/macro/uidefine.h"
#include "sdk/include/uiapi.h"
#include <functional>

namespace uia {
struct IAnimateManager;
}
namespace ui {
class Application;
struct ILayout;
struct IUIEditor;
struct IResBundle;
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
struct IObjectDescription;
struct IWindowBase;
struct ITopWindowManager;
struct IRenderBase;
struct ITextRenderBase;
struct IMessageFilterMgr;
struct TOOLTIPITEM;
class TimerItem;

struct UIAPI IApplication {
  // 返回的对象，需要调用destroy进行释放！
  static IApplication* create();
  static void destroy(IApplication*);

  IApplication();
  ~IApplication();
  Application *GetImpl();
  

  void Run();
  void Quit();

  IResBundle *RootBundle();
  IResBundle *LoadResBundle(const wchar_t *szSkinPath);
  IResBundle *LoadResBundle(long hInstance, int resId = -1);

  void SetEditorMode(bool b);
  bool IsEditorMode();
  void SetUIEditorPtr(IUIEditor *);
  IUIEditor *GetUIEditorPtr();

  IResourceManager &GetResourceManager();
  ITopWindowManager *GetTopWindowMgr();
  uia::IAnimateManager *GetAnimateManager();
  // IMessageFilterMgr*  GetMessageFilterMgr();

  IResBundle *GetDefaultSkinRes();

  void RestoreRegisterUIObject();
  bool RegisterControlTagParseFunc(const wchar_t *szTag,
                                   pfnParseControlTag func);
  bool GetSkinTagParseFunc(const wchar_t *szTag, pfnParseSkinTag *pFunc);
  bool GetControlTagParseFunc(const wchar_t *szTag, pfnParseControlTag *pFunc);

  IObject *CreateUIObjectByName(const wchar_t *szName, IResBundle *pISkinRes);
  IObject *CreateUIObjectByClsid(const Uuid &clsid, IResBundle *pISkinRes);
  bool RegisterUIObject(IObjectDescription *p);
  void LoadUIObjectListToToolBox();

  bool RegisterUIRenderBaseCreateData(const wchar_t *szName, int nType,
                                      pfnUICreateRenderBasePtr pfunc);
  bool CreateRenderBase(int nType, IObject *pObject, IRenderBase **ppOut);
  void EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, long wParam,
                          long lParam);

  bool RegisterUITextRenderBaseCreateData(const wchar_t *szName, int nType,
                                          pfnUICreateTextRenderBasePtr pfunc);
  bool CreateTextRenderBaseByName(const wchar_t *szName, IObject *pObject,
                                  ITextRenderBase **ppOut);
  bool CreateTextRenderBase(int nType, IObject *pObject,
                            ITextRenderBase **ppOut);
  void EnumTextRenderBaseName(pfnEnumTextRenderBaseNameCallback callback,
                              long wParam, long lParam);

  bool CreateLayoutByName(const wchar_t *szName, IObject *pObject,
                          bool bCreateDefault, ILayout **ppOut);
  void EnumLayoutType(pfnEnumLayoutTypeCallback callback, long wParam,
                      long lParam);

#if 0
    bool  IsDialogMessage(MSG* pMsg);
#endif
  void MsgHandleLoop(bool *pbQuitLoopRef = nullptr);
  void RedrawTopWindows();

  bool ShowToolTip(TOOLTIPITEM *pItem);
  void HideToolTip();
  unsigned int SetTimer(int nElapse, IMessage *pNotify);
  unsigned int SetTimer(int nElapse,
                        std::function<bool(unsigned int, TimerItem *)> func);
  unsigned int SetTimerById(int nElapse, unsigned int nId, IMessage *pNotify);
  unsigned int SetTimer(int nElapse, TimerItem *pTimerItem);
  void KillTimer(unsigned int &nTimerID);
  void KillTimerById(int nId, IMessage *pNotify);
  void KillTimerByNotify(IMessage *pNotify);

#if 0
    HDC   GetCacheDC();
    void  ReleaseCacheDC(HDC hDC);
    HBITMAP  GetCacheBitmap(int nWidth, int nHeight);  // 注：不要释放该HBITMAP，由内部维护
    void  ShadowBlur(HBITMAP hBitmap, COLORREF colorShadow, Rect* prcBlur, int nRadius);
#endif
  bool IsUnderXpOS();
  bool IsVistaOrWin7etc();
  bool IsAeroEnable();
  bool IsGpuCompositeEnable();
  bool EnableGpuComposite();
#if 0
  HWND GetForwardPostMessageWnd();
  IWindowBase *GetWindowBaseFromHWND(HWND hWnd);
#endif
private:
  Application *m_pImpl;
};

using ApplicationPtr = ui::unique_ptr<ui::IApplication>;

} // namespace ui
#endif // _IUIAPPLICATION_H_