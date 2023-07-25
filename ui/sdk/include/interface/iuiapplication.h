#ifndef _IUIAPPLICATION_H_
#define _IUIAPPLICATION_H_
#include <functional>
#include "include/common/guid/guid.h"
#include "include/base/uidefine.h"
#include "include/base/uiapi.h"

namespace uia
{
	struct IAnimateManager;
}
namespace ui
{
class UIApplication;
struct ILayout;
struct IUIEditor;
struct ISkinRes;      
struct ISkinManager;
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


struct UIAPI IUIApplication
{
    IUIApplication();
    ~IUIApplication();
    UIApplication*  GetImpl();
    void  Release();

    void Run();
    void Quit();

    ISkinRes*  LoadSkinRes(const wchar_t* szSkinPath);
	ISkinRes*  LoadSkinRes(long hInstance, int resId=-1);

    void  SetEditorMode(bool b);
    bool  IsEditorMode();
    void  SetUIEditorPtr(IUIEditor*);
    IUIEditor*  GetUIEditorPtr();

    ISkinManager&       GetSkinManager();
    ITopWindowManager*  GetTopWindowMgr();
    uia::IAnimateManager*   GetAnimateMgr();
	IMessageFilterMgr*  GetMessageFilterMgr();

    ISkinRes*       GetDefaultSkinRes();

    void  RestoreRegisterUIObject();
    bool  RegisterControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag func);
    bool  GetSkinTagParseFunc(const wchar_t* szTag, pfnParseSkinTag* pFunc);
    bool  GetControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag* pFunc);
    
    IObject*  CreateUIObjectByName(const wchar_t* szName, ISkinRes* pISkinRes);
    IObject*  CreateUIObjectByClsid(const Guid& clsid, ISkinRes* pISkinRes);
    bool  RegisterUIObject(IObjectDescription* p);
    void  LoadUIObjectListToToolBox();

    bool  RegisterUIRenderBaseCreateData(const wchar_t* szName, int nType, pfnUICreateRenderBasePtr pfunc);
    bool  CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut);
    void  EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, long wParam, long lParam);

    bool  RegisterUITextRenderBaseCreateData(const wchar_t* szName, int nType, pfnUICreateTextRenderBasePtr pfunc);
    bool  CreateTextRenderBaseByName(const wchar_t* szName, IObject* pObject, ITextRenderBase** ppOut);
    bool  CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut);
    void  EnumTextRenderBaseName(pfnEnumTextRenderBaseNameCallback callback, long wParam, long lParam);

    bool  CreateLayoutByName(const wchar_t* szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut);
    bool  CreateLayout(int nType, IObject* pObject, ILayout** ppOut);
    void  EnumLayoutType(pfnEnumLayoutTypeCallback callback, long wParam, long lParam);

#if 0
    bool  IsDialogMessage(MSG* pMsg);
#endif
    void  MsgHandleLoop(bool* pbQuitLoopRef=nullptr);
    void  RedrawTopWindows();

    bool  ShowToolTip(TOOLTIPITEM* pItem);
    void  HideToolTip();
	unsigned int  SetTimer(int nElapse, IMessage* pNotify);
	unsigned int  SetTimer(int nElapse, std::function<bool(unsigned int, TimerItem*)> func);
    unsigned int  SetTimerById(int nElapse, unsigned int nId, IMessage* pNotify);
    unsigned int  SetTimer(int nElapse, TimerItem* pTimerItem);
    void  KillTimer(unsigned int& nTimerID);
    void  KillTimerById(int nId, IMessage* pNotify);
    void  KillTimerByNotify(IMessage* pNotify);

#if 0
    HDC   GetCacheDC();
    void  ReleaseCacheDC(HDC hDC);
    HBITMAP  GetCacheBitmap(int nWidth, int nHeight);  // 注：不要释放该HBITMAP，由内部维护
    void  ShadowBlur(HBITMAP hBitmap, COLORREF colorShadow, RECT* prcBlur, int nRadius);
#endif
    bool  IsUnderXpOS();
    bool  IsVistaOrWin7etc();
    bool  IsAeroEnable();
	bool  IsGpuCompositeEnable();
    bool  EnableGpuComposite();

    HWND  GetForwardPostMessageWnd();
    IWindowBase*  GetWindowBaseFromHWND(HWND hWnd);

private:
    UIApplication*  m_pImpl;
};

struct UIAPI UISDKVersion
{
	static int  GetMajor();
	static int  GetMinor();
	static int  GetPatch();
	static int  GetVersionText(wchar_t* szText, int nTextSize);
};

}
#endif // _IUIAPPLICATION_H_