#include "include/interface/iuiapplication.h"
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
    
IApplication::IApplication()
{
    m_pImpl = new Application(this);
}
IApplication::~IApplication()
{
    SAFE_DELETE(m_pImpl);
}

void IApplication::Run() {
    m_pImpl->Run();
}
void  IApplication::Quit()
{
	m_pImpl->Quit();
}


Application*  IApplication::GetImpl() 
{ 
	return m_pImpl;
}
void  IApplication::Release()
{
	delete this;
}

IResBundle* IApplication::RootBundle() {
    return m_pImpl->GetResourceManager().RootBundle().GetIResBundle();
}

IResBundle*  IApplication::LoadResBundle(const wchar_t* szSkinResPath)
{
    ResBundle* p = m_pImpl->GetResourceManager().LoadResBundle(szSkinResPath); 
	if (p)
		return p->GetIResBundle();
	return nullptr;
}

IResBundle*  IApplication::LoadResBundle(long hInstance, int resId)
{
	ResBundle* p = m_pImpl->GetResourceManager().LoadResBundle(hInstance, resId); 
	if (p)
		return p->GetIResBundle();
	return nullptr;
}

void  IApplication::SetEditorMode(bool b)
{
	m_pImpl->SetEditorMode(b); 
}
void  IApplication::SetUIEditorPtr(IUIEditor* p)
{
	m_pImpl->SetUIEditorPtr(p); 
}

bool  IApplication::IsEditorMode()
{ 
	return m_pImpl->IsEditorMode();
}
IUIEditor*  IApplication::GetUIEditorPtr()
{
    return m_pImpl->GetUIEditorPtr(); 
}

IResourceManager&  IApplication::GetResourceManager()    
{ 
    return m_pImpl->GetResourceManager().GetIResourceManager();
}

ITopWindowManager*  IApplication::GetTopWindowMgr()
{
    return m_pImpl->GetITopWindowMgr(); 
}
uia::IAnimateManager*  IApplication::GetAnimateManager()  
{ 
    return m_pImpl->GetAnimateManager();
}
// IMessageFilterMgr*  IApplication::GetMessageFilterMgr()
// {
// 	return m_pImpl->GetMessageFilterMgr();
// }
IResBundle*  IApplication::GetDefaultSkinRes()
{
    ResBundle* p = m_pImpl->GetDefaultSkinRes(); 
	if (p)
		return p->GetIResBundle();
	return nullptr;
}
void  IApplication::RestoreRegisterUIObject()      
{
    m_pImpl->RestoreRegisterUIObject(); 
}
bool  IApplication::RegisterControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag func)
{ 
    return m_pImpl->RegisterControlTagParseFunc(szTag, func); 
}
bool  IApplication::GetSkinTagParseFunc(const wchar_t* szTag, pfnParseSkinTag* pFunc)
{ 
    return m_pImpl->GetSkinTagParseFunc(szTag, pFunc);
}
bool  IApplication::GetControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag* pFunc) 
{ 
    return m_pImpl->GetControlTagParseFunc(szTag, pFunc);
}

IObject*  IApplication::CreateUIObjectByName(const wchar_t* szName, IResBundle* pISkinRes)
{ 
    return m_pImpl->CreateUIObjectByName(szName, pISkinRes); 
}
IObject*  IApplication::CreateUIObjectByClsid(const Uuid& clsid, IResBundle* pISkinRes) 
{ 
    return m_pImpl->CreateUIObjectByClsid(clsid, pISkinRes);
}
bool  IApplication::RegisterUIObject(IObjectDescription* p)
{
    return m_pImpl->RegisterUIObject(p); 
}
void     IApplication::LoadUIObjectListToToolBox()         
{
    m_pImpl->LoadUIObjectListToToolBox(); 
}

bool  IApplication::RegisterUIRenderBaseCreateData(
	const wchar_t* szName, int nType, pfnUICreateRenderBasePtr pfunc) 
{
    return m_pImpl->GetRenderBaseFactory().RegisterUIRenderBaseCreateData(szName, nType, pfunc); 
}

// bool  IApplication::CreateRenderBaseByName(
// 		const wchar_t* szName, IObject* pObject, IRenderBase** ppOut)
// {
//     IResBundle* pSkinRes = nullptr;
//     if (pObject)
//         pSkinRes = pObject->GetResBundle();
//     else
//         pSkinRes = GetDefaultSkinRes();
// 
//     return m_pImpl->GetRenderBaseFactory().CreateRenderBaseByName(
// 		pSkinRes, szName, pObject, ppOut); 
// }
// 
// const wchar_t*  IApplication::GetRenderBaseName(int nType)
// {
// 	return m_pImpl->GetRenderBaseFactory().GetRenderBaseName(nType);
// }

bool  IApplication::CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut) 
{ 
    IResBundle* pSkinRes = nullptr;
    if (pObject)
        pSkinRes = pObject->GetResBundle();
    else
        pSkinRes = GetDefaultSkinRes();
    return m_pImpl->GetRenderBaseFactory().CreateRenderBase(pSkinRes, nType, pObject, ppOut);
}

void  IApplication::EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, long wParam, long lParam)
{
    m_pImpl->GetRenderBaseFactory().EnumRenderBaseName(callback, wParam, lParam);
}

bool  IApplication::RegisterUITextRenderBaseCreateData(
	const wchar_t* szName, int nType,
    pfnUICreateTextRenderBasePtr pfunc) 
{ 
    return m_pImpl->GetTextRenderFactroy().RegisterUITextRenderBaseCreateData(szName, nType, pfunc); 
}
bool  IApplication::CreateTextRenderBaseByName(const wchar_t* szName, IObject* pObject, ITextRenderBase** ppOut) 
{
    IResBundle* pSkinRes = nullptr;
    if (pObject)
        pSkinRes = pObject->GetResBundle();
    else
        pSkinRes = GetDefaultSkinRes();
    return m_pImpl->GetTextRenderFactroy().CreateTextRenderBaseByName(pSkinRes, szName, pObject, ppOut);
}
bool  IApplication::CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut) 
{ 
    IResBundle* pSkinRes = nullptr;
    if (pObject)
        pSkinRes = pObject->GetResBundle();
    else
        pSkinRes = GetDefaultSkinRes();
    return m_pImpl->GetTextRenderFactroy().CreateTextRender(pSkinRes, nType, pObject, ppOut); 
}
void  IApplication::EnumTextRenderBaseName(pfnEnumTextRenderBaseNameCallback callback, long wParam, long lParam)
{
    m_pImpl->GetTextRenderFactroy().EnumTextRenderBaseName(callback, wParam, lParam);
}

bool  IApplication::CreateLayoutByName(const wchar_t* szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut) 
{ 
    return m_pImpl->GetLayoutFactory().CreateByName(szName, pObject, bCreateDefault, ppOut); 
}
void  IApplication::EnumLayoutType(pfnEnumLayoutTypeCallback callback, long wParam, long lParam)
{
    m_pImpl->GetLayoutFactory().EnumLayoutType(callback, wParam, lParam);
}
// bool  IApplication::IsDialogMessage(MSG* pMsg) 
// {
//     return m_pImpl->IsDialogMessage(pMsg); 
// }
// void  IApplication::MsgHandleLoop(bool* pbQuitLoopRef)
// { 
//     m_pImpl->MsgHandleLoop(pbQuitLoopRef);
// }
// void  IApplication::RedrawTopWindows() 
// { 
//     m_pImpl->RedrawTopWindows(); 
// }
// bool  IApplication::ShowToolTip(TOOLTIPITEM* pItem)
// {
//     return m_pImpl->ShowToolTip(pItem); 
// }
// void  IApplication::HideToolTip() 
// { 
//     m_pImpl->HideToolTip(); 
// }

// unsigned int*  IApplication::SetTimer(int nElapse, IMessage* pNotify)
// {
//     return TimerHelper::GetInstance()->SetTimer(nElapse, pNotify);
// }
// unsigned int*  IApplication::SetTimer(
// 	int nElapse, std::function<bool(unsigned int*, TimerItem*)> func)
// {
// 	return TimerHelper::GetInstance()->SetTimer(nElapse, func);
// }
// unsigned int*  IApplication::SetTimer(int nElapse, TimerItem* pTimerItem)
// {
//     return TimerHelper::GetInstance()->SetTimer(nElapse, pTimerItem);
// }
// unsigned int*  IApplication::SetTimerById(
// 	int nElapse, unsigned int* nId, IMessage* pNotify)
// {
//     return  TimerHelper::GetInstance()->SetTimerById(nElapse, nId, pNotify);
// }
// void  IApplication::KillTimer(unsigned int*& nTimerID)
// {
//     TimerHelper::GetInstance()->KillTimer(nTimerID);
// }
// void  IApplication::KillTimerById(int nId, IMessage* pNotify)
// {
//     TimerHelper::GetInstance()->KillTimerById(nId, pNotify);
// }
// void  IApplication::KillTimerByNotify(IMessage* pNotify)
// {
//     TimerHelper::GetInstance()->KillTimerByNotify(pNotify);
// }
// 
// HDC  IApplication::GetCacheDC()
// {
//     return Image::GetCacheDC();
// }
// void  IApplication::ReleaseCacheDC(HDC hDC)
// {
//     return Image::ReleaseCacheDC(hDC);
// }  
// HBITMAP  IApplication::GetCacheBitmap(int nWidth, int nHeight)
// {
//     return CacheBitmap::GetInstance()->Create(nWidth, nHeight);
// }
// void   IApplication::ShadowBlur(HBITMAP hBitmap, COLORREF colorShadow, Rect* prcBlur, int nRadius)
// {
//     ::ShadowBlur(hBitmap, colorShadow, prcBlur, nRadius);
// }

// bool  IApplication::IsUnderXpOS() 
// { 
//     return m_pImpl->IsUnderXpOS();
// }
// bool  IApplication::IsVistaOrWin7etc() 
// { 
//     return m_pImpl->IsVistaOrWin7etc();
// }
// bool  IApplication::IsAeroEnable()
// {
//     return DwmHelper::GetInstance()->IsEnable();
// }

// HWND  IApplication::GetForwardPostMessageWnd() 
// { 
//     return m_pImpl->GetForwardPostMessageWnd();
// }

// bool  IApplication::IsGpuCompositeEnable()
// {
//     return m_pImpl->IsGpuCompositeEnable();
// }
// bool  IApplication::EnableGpuComposite()
// {
//     return m_pImpl->EnableGpuComposite();
// }

// IWindowBase*  IApplication::GetWindowBaseFromHWND(HWND hWnd)
// {
//     return m_pImpl->GetTopWindowMgr()->GetWindowBase(hWnd);
// }
}

