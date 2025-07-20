#ifndef _IUIINTERFACE_H_
#define _IUIINTERFACE_H_
#include "sdk/include/interface/imessage.h"

// IApplication接口
namespace ui {
struct IToolTipUI;
struct IWindow;
struct IResourceManager;
struct IResource;
struct TOOLTIPITEM;
struct IRenderBitmap;
struct IRenderFont;
struct ISkinConfigRes;
struct ISkinConfigManager;
struct ITopWindowManager;
struct IWaitForHandlesMgr;
struct IRenderTarget;
struct IMessageFilterMgr;
struct IMapAttribute;
struct IObject;

// 句柄定义
struct IApplication;

class TopWindowManager;
struct UIAPI ITopWindowManager : public IRootInterface {
  ITopWindowManager(TopWindowManager *p);
  TopWindowManager *GetImpl();

  long AddTopWindowObject(IWindow *);
  long RemoveTopWindowObject(IWindow *);
  // void     ChangeSkin(IResource* pNewSkinRes);

  bool UpdateAllWindow();
  // void SendMessage2AllWnd(UIMSG *pMsg);
  // void PostMessage2AllWnd(UIMSG* pMsg);
  // void ForwardMessage2AllObj(UIMSG *pMsg);

private:
  TopWindowManager *m_pImpl;
};

//////////////////////////////////////////////////////////////////////////

//
// 各种提示条绘制的抽象类
//
struct IToolTipUI {
public:
  virtual ~IToolTipUI() {};
  virtual bool Create() = 0;
  virtual bool Destroy() = 0;
  virtual bool SetText(const wchar_t *szText) = 0;
  virtual bool SetTitle(const wchar_t *szText) = 0;
  virtual bool Show(/*HWND*/ int hWndParent) = 0;
  virtual bool Hide() = 0;
  virtual bool SetUIApplication(IApplication *p) = 0;
  virtual void OnSerialize(SerializeParam *pData) = 0;
};
struct IFlashInvalidateListener {
  virtual void OnInvalidateRect(const Rect *, bool) = 0;
};
struct IPreTranslateMessage // IMessageFilter 被系统其它地方已经定义过了
{
public:
  virtual bool PreTranslateMessage(/*MSG*/ void *pMsg) = 0;
};

struct IWaitForHandleCallback {
  virtual void OnWaitForHandleObjectCallback(long *handle, long) = 0;
};

struct IMessageFilterMgr : public IRootInterface {
  virtual void AddMessageFilter(IPreTranslateMessage *p) = 0;
  virtual void RemoveMessageFilter(IPreTranslateMessage *p) = 0;
};

} // namespace ui

#endif // _IUIINTERFACE_H_