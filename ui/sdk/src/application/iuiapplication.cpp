#include "interface/iuiapplication.h"
#include "src/application/uiapplication.h"
#include "src/resource/res_bundle.h"
#include "include/interface/iobject.h"
// #include "src/Atl\image.h"
// #include "src/Helper\timer\timermanager.h"
// #include "src/Util\dwm\dwmhelper.h"
// #include "src/UIEffect\CacheBitmap\cachebitmap.h"
// #include "src/UIEffect\blur\webkit\shadowblur.h"
// #include "src/layout/layout_factory.h"
// #include "src/Renderbase\renderbase\renderbase_factory.h"


namespace ui 
{
    
IUIApplication::IUIApplication()
{
    m_pImpl = new UIApplication(this);
}
IUIApplication::~IUIApplication()
{
    SAFE_DELETE(m_pImpl);
}

void IUIApplication::Run() {
    m_pImpl->Run();
}
void  IUIApplication::Quit()
{
	m_pImpl->Quit();
}


UIApplication*  IUIApplication::GetImpl() 
{ 
	return m_pImpl;
}
void  IUIApplication::Release()
{
	delete this;
}

IResBundle* IUIApplication::RootBundle() {
    return m_pImpl->GetResourceManager().RootBundle().GetIResBundle();
}

IResBundle*  IUIApplication::LoadResBundle(const wchar_t* szSkinResPath)
{
    ResBundle* p = m_pImpl->GetResourceManager().LoadResBundle(szSkinResPath); 
	if (p)
		return p->GetIResBundle();
	return nullptr;
}

IResBundle*  IUIApplication::LoadResBundle(long hInstance, int resId)
{
	ResBundle* p = m_pImpl->GetResourceManager().LoadResBundle(hInstance, resId); 
	if (p)
		return p->GetIResBundle();
	return nullptr;
}

void  IUIApplication::SetEditorMode(bool b)
{
	m_pImpl->SetEditorMode(b); 
}
void  IUIApplication::SetUIEditorPtr(IUIEditor* p)
{
	m_pImpl->SetUIEditorPtr(p); 
}

bool  IUIApplication::IsEditorMode()
{ 
	return m_pImpl->IsEditorMode();
}
IUIEditor*  IUIApplication::GetUIEditorPtr()
{
    return m_pImpl->GetUIEditorPtr(); 
}

IResourceManager&  IUIApplication::GetResourceManager()    
{ 
    return m_pImpl->GetResourceManager().GetIResourceManager();
}

ITopWindowManager*  IUIApplication::GetTopWindowMgr()
{
    return m_pImpl->GetITopWindowMgr(); 
}
uia::IAnimateManager*  IUIApplication::GetAnimateManager()  
{ 
    return m_pImpl->GetAnimateManager();
}
// IMessageFilterMgr*  IUIApplication::GetMessageFilterMgr()
// {
// 	return m_pImpl->GetMessageFilterMgr();
// }
IResBundle*  IUIApplication::GetDefaultSkinRes()
{
    ResBundle* p = m_pImpl->GetDefaultSkinRes(); 
	if (p)
		return p->GetIResBundle();
	return nullptr;
}
void  IUIApplication::RestoreRegisterUIObject()      
{
    m_pImpl->RestoreRegisterUIObject(); 
}
bool  IUIApplication::RegisterControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag func)
{ 
    return m_pImpl->RegisterControlTagParseFunc(szTag, func); 
}
bool  IUIApplication::GetSkinTagParseFunc(const wchar_t* szTag, pfnParseSkinTag* pFunc)
{ 
    return m_pImpl->GetSkinTagParseFunc(szTag, pFunc);
}
bool  IUIApplication::GetControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag* pFunc) 
{ 
    return m_pImpl->GetControlTagParseFunc(szTag, pFunc);
}

IObject*  IUIApplication::CreateUIObjectByName(const wchar_t* szName, IResBundle* pISkinRes)
{ 
    return m_pImpl->CreateUIObjectByName(szName, pISkinRes); 
}
IObject*  IUIApplication::CreateUIObjectByClsid(const Guid& clsid, IResBundle* pISkinRes) 
{ 
    return m_pImpl->CreateUIObjectByClsid(clsid, pISkinRes);
}
bool  IUIApplication::RegisterUIObject(IObjectDescription* p)
{
    return m_pImpl->RegisterUIObject(p); 
}
void     IUIApplication::LoadUIObjectListToToolBox()         
{
    m_pImpl->LoadUIObjectListToToolBox(); 
}

bool  IUIApplication::RegisterUIRenderBaseCreateData(
	const wchar_t* szName, int nType, pfnUICreateRenderBasePtr pfunc) 
{
    return m_pImpl->GetRenderBaseFactory().RegisterUIRenderBaseCreateData(szName, nType, pfunc); 
}

// bool  IUIApplication::CreateRenderBaseByName(
// 		const wchar_t* szName, IObject* pObject, IRenderBase** ppOut)
// {
//     IResBundle* pSkinRes = nullptr;
//     if (pObject)
//         pSkinRes = pObject->GetSkinRes();
//     else
//         pSkinRes = GetDefaultSkinRes();
// 
//     return m_pImpl->GetRenderBaseFactory().CreateRenderBaseByName(
// 		pSkinRes, szName, pObject, ppOut); 
// }
// 
// const wchar_t*  IUIApplication::GetRenderBaseName(int nType)
// {
// 	return m_pImpl->GetRenderBaseFactory().GetRenderBaseName(nType);
// }

bool  IUIApplication::CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut) 
{ 
    IResBundle* pSkinRes = nullptr;
    if (pObject)
        pSkinRes = pObject->GetSkinRes();
    else
        pSkinRes = GetDefaultSkinRes();
    return m_pImpl->GetRenderBaseFactory().CreateRenderBase(pSkinRes, nType, pObject, ppOut);
}

void  IUIApplication::EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, long wParam, long lParam)
{
    m_pImpl->GetRenderBaseFactory().EnumRenderBaseName(callback, wParam, lParam);
}

bool  IUIApplication::RegisterUITextRenderBaseCreateData(
	const wchar_t* szName, int nType,
    pfnUICreateTextRenderBasePtr pfunc) 
{ 
    return m_pImpl->GetTextRenderFactroy().RegisterUITextRenderBaseCreateData(szName, nType, pfunc); 
}
bool  IUIApplication::CreateTextRenderBaseByName(const wchar_t* szName, IObject* pObject, ITextRenderBase** ppOut) 
{
    IResBundle* pSkinRes = nullptr;
    if (pObject)
        pSkinRes = pObject->GetSkinRes();
    else
        pSkinRes = GetDefaultSkinRes();
    return m_pImpl->GetTextRenderFactroy().CreateTextRenderBaseByName(pSkinRes, szName, pObject, ppOut);
}
bool  IUIApplication::CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut) 
{ 
    IResBundle* pSkinRes = nullptr;
    if (pObject)
        pSkinRes = pObject->GetSkinRes();
    else
        pSkinRes = GetDefaultSkinRes();
    return m_pImpl->GetTextRenderFactroy().CreateTextRender(pSkinRes, nType, pObject, ppOut); 
}
void  IUIApplication::EnumTextRenderBaseName(pfnEnumTextRenderBaseNameCallback callback, long wParam, long lParam)
{
    m_pImpl->GetTextRenderFactroy().EnumTextRenderBaseName(callback, wParam, lParam);
}

bool  IUIApplication::CreateLayoutByName(const wchar_t* szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut) 
{ 
    return m_pImpl->GetLayoutFactory().CreateByName(szName, pObject, bCreateDefault, ppOut); 
}
bool  IUIApplication::CreateLayout(int nType, IObject* pObject, ILayout** ppOut)
{
    return m_pImpl->GetLayoutFactory().Create(nType, pObject, ppOut);
}
void  IUIApplication::EnumLayoutType(pfnEnumLayoutTypeCallback callback, long wParam, long lParam)
{
    m_pImpl->GetLayoutFactory().EnumLayoutType(callback, wParam, lParam);
}
// bool  IUIApplication::IsDialogMessage(MSG* pMsg) 
// {
//     return m_pImpl->IsDialogMessage(pMsg); 
// }
// void  IUIApplication::MsgHandleLoop(bool* pbQuitLoopRef)
// { 
//     m_pImpl->MsgHandleLoop(pbQuitLoopRef);
// }
// void  IUIApplication::RedrawTopWindows() 
// { 
//     m_pImpl->RedrawTopWindows(); 
// }
// bool  IUIApplication::ShowToolTip(TOOLTIPITEM* pItem)
// {
//     return m_pImpl->ShowToolTip(pItem); 
// }
// void  IUIApplication::HideToolTip() 
// { 
//     m_pImpl->HideToolTip(); 
// }

// unsigned int*  IUIApplication::SetTimer(int nElapse, IMessage* pNotify)
// {
//     return TimerHelper::GetInstance()->SetTimer(nElapse, pNotify);
// }
// unsigned int*  IUIApplication::SetTimer(
// 	int nElapse, std::function<bool(unsigned int*, TimerItem*)> func)
// {
// 	return TimerHelper::GetInstance()->SetTimer(nElapse, func);
// }
// unsigned int*  IUIApplication::SetTimer(int nElapse, TimerItem* pTimerItem)
// {
//     return TimerHelper::GetInstance()->SetTimer(nElapse, pTimerItem);
// }
// unsigned int*  IUIApplication::SetTimerById(
// 	int nElapse, unsigned int* nId, IMessage* pNotify)
// {
//     return  TimerHelper::GetInstance()->SetTimerById(nElapse, nId, pNotify);
// }
// void  IUIApplication::KillTimer(unsigned int*& nTimerID)
// {
//     TimerHelper::GetInstance()->KillTimer(nTimerID);
// }
// void  IUIApplication::KillTimerById(int nId, IMessage* pNotify)
// {
//     TimerHelper::GetInstance()->KillTimerById(nId, pNotify);
// }
// void  IUIApplication::KillTimerByNotify(IMessage* pNotify)
// {
//     TimerHelper::GetInstance()->KillTimerByNotify(pNotify);
// }
// 
// HDC  IUIApplication::GetCacheDC()
// {
//     return Image::GetCacheDC();
// }
// void  IUIApplication::ReleaseCacheDC(HDC hDC)
// {
//     return Image::ReleaseCacheDC(hDC);
// }  
// HBITMAP  IUIApplication::GetCacheBitmap(int nWidth, int nHeight)
// {
//     return CacheBitmap::GetInstance()->Create(nWidth, nHeight);
// }
// void   IUIApplication::ShadowBlur(HBITMAP hBitmap, COLORREF colorShadow, RECT* prcBlur, int nRadius)
// {
//     ::ShadowBlur(hBitmap, colorShadow, prcBlur, nRadius);
// }

// bool  IUIApplication::IsUnderXpOS() 
// { 
//     return m_pImpl->IsUnderXpOS();
// }
// bool  IUIApplication::IsVistaOrWin7etc() 
// { 
//     return m_pImpl->IsVistaOrWin7etc();
// }
// bool  IUIApplication::IsAeroEnable()
// {
//     return DwmHelper::GetInstance()->IsEnable();
// }

// HWND  IUIApplication::GetForwardPostMessageWnd() 
// { 
//     return m_pImpl->GetForwardPostMessageWnd();
// }

// bool  IUIApplication::IsGpuCompositeEnable()
// {
//     return m_pImpl->IsGpuCompositeEnable();
// }
// bool  IUIApplication::EnableGpuComposite()
// {
//     return m_pImpl->EnableGpuComposite();
// }

// IWindowBase*  IUIApplication::GetWindowBaseFromHWND(HWND hWnd)
// {
//     return m_pImpl->GetTopWindowMgr()->GetWindowBase(hWnd);
// }
}

