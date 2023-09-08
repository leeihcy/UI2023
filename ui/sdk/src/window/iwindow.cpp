#include "include/interface/iwindow.h"
#include "include/uiapi.h"
#include "include/macro/uidefine.h"
#include "src/window/window.h"
#include "src/window/window_meta.h"

#if 0
#include "src/resource/res_bundle.h"
#endif

namespace ui {
UI_IMPLEMENT_INTERFACE(Window, Panel)

void IWindow::Create(const Rect &rect) { __pImpl->Create(rect); }
void IWindow::SetTitle(const char * title_utf8) { __pImpl->SetTitle(title_utf8); }
void IWindow::Show() { __pImpl->Show(); }


#if 0
UI_IMPLEMENT_INTERFACE(WindowBase, Panel)
UI_IMPLEMENT_INTERFACE(Window, WindowBase)

HWND  IWindowBase::GetHWND()                                              
{ 
	return __pImpl->GetHWND(); 
}
bool  IWindowBase::IsChildWindow()                                        
{
	return __pImpl->IsChildWindow(); 
}
bool  IWindowBase::IsWindowVisible()
{
	return __pImpl->IsWindowVisible(); 
}
void  IWindowBase::ShowWindow()                                           
{
	__pImpl->ShowWindow();
}
void  IWindowBase::HideWindow()                                            
{ 
	__pImpl->HideWindow(); 
}
void  IWindowBase::CenterWindow(HWND hWndCenter)                           
{
	__pImpl->CenterWindow(hWndCenter);
}
void  IWindowBase::CenterWindow(HMONITOR hMonitor)                         
{
	__pImpl->CenterWindow(hMonitor);
}
bool  IWindowBase::IsDoModal()                                             
{ 
	return __pImpl->IsDoModal(); 
}
IWindowRender*  IWindowBase::GetWindowRender()                            
{ 
	return __pImpl->GetWindowRender()->GetIWindowRender();
}
IResource*  IWindowBase::GetResource()                                      
{ 
	return __pImpl->GetResource()->GetIResource(); 
}
void   IWindowBase::SetFocusObject(IObject* pObj)
{
    Object*  p = nullptr;
    if (pObj)
        p = pObj->GetImpl();

    __pImpl->SetFocusObject(p);
}

void  IWindowBase::SetPressObject(IObject* pObj)
{
	Object*  p = nullptr;
	if (pObj)
		p = pObj->GetImpl();

	__pImpl->SetPressObject(p);
}

IObject*  IWindowBase::GetHoverObject()
{
    Object* p = __pImpl->GetHoverObject();
    if (p)
        return p->GetIObject();
    return nullptr;
}
IObject*  IWindowBase::GetPressObject()
{
    Object* p = __pImpl->GetPressObject();
    if (p)
        return p->GetIObject();
    return nullptr;
}
IObject*  IWindowBase::GetObjectByPos(IObject* pObjParent, Point* pt, bool bSkinBuilderInvoke)
{ 
    if (nullptr == pObjParent)
        return nullptr;

    Object* pRet = __pImpl->GetObjectByPos(pObjParent->GetImpl(), pt, bSkinBuilderInvoke); 
    if (pRet)
        return pRet->GetIObject();

    return nullptr;
}
bool  IWindowBase::Create(const wchar_t* szID, HWND hWndParent, Rect* prc, long lStyle, long lExStyle) 
{
	return __pImpl->Create(szID, hWndParent, prc, lStyle, lExStyle);
}
void  IWindowBase::DestroyWindow()
{
	return __pImpl->DestroyWindow();
}
bool  IWindowBase::Attach(HWND hWnd, const wchar_t* szID) 
{ 
	return __pImpl->Attach(hWnd, szID);
}
void  IWindowBase::Detach()
{
	__pImpl->Detach(); 
}
INT_PTR  IWindowBase::DoModal(const wchar_t* szID, HWND hWndParent, bool canResize ) 
{
	return __pImpl->DoModal(szID, hWndParent, canResize);
}
INT_PTR  IWindowBase::DoModal(HINSTANCE hResInst, unsigned int nResID, const wchar_t* szID, HWND hWndParent)
{
	return __pImpl->DoModal(hResInst, nResID, szID, hWndParent); 
}
HWND  IWindowBase::DoModeless(const wchar_t* szID, HWND hWndParent, bool canResize )
{
	return __pImpl->DoModeless(szID, hWndParent, canResize);
}
HWND  IWindowBase::DoModeless(HINSTANCE hResInst, unsigned int nResID, const wchar_t* szID, HWND hWndParent) 
{ 
	return __pImpl->DoModeless(hResInst, nResID, szID, hWndParent);
}
void  IWindowBase::EndDialog(INT_PTR n)
{ 
	return __pImpl->EndDialog(n);
}

void  IWindowBase::CalcWindowSizeByClientSize( Size sizeClient, Size* pSizeWindow )
{
	__pImpl->CalcWindowSizeByClientSize(sizeClient, pSizeWindow); 
}
void  IWindowBase::CalcClientRectByWindowRect( Rect* rcWindow, Rect* rcClient )
{
	__pImpl->CalcClientRectByWindowRect(rcWindow, rcClient);
}
void  IWindowBase::SaveMemBitmap(wchar_t* szFile) 
{
	__pImpl->SaveMemBitmap(szFile);
}
void  IWindowBase::AlphaBlendMemBitmap(HDC hDC, Rect* prc, int alpha)
{
	__pImpl->AlphaBlendMemBitmap(hDC, prc, alpha);
}
void  IWindowBase::BitBltMemBitmap(HDC hDC, Rect* prc)
{
	__pImpl->BitBltMemBitmap(hDC, prc);
}

void  IWindowBase::EnableDwmTransition(bool b) 
{ 
	__pImpl->EnableDwmTransition(b); 
}

// bool  IWindowBase::AddAnchorItem(const SyncWindowData& data)
// {
// 	return __pImpl->AddAnchorItem(data);
// }
// void  IWindowBase::HideAllAnchorItem() 
// {
// 	__pImpl->HideAllAnchorItem(); 
// }

long  IWindowBase::SetDroppable(bool b) 
{
	return __pImpl->SetCanDrop(b);
}
void  IWindowBase::EnableGpuComposite(bool b)
{
	return __pImpl->SetGpuComposite(b);
}
bool  IWindowBase::IsGpuComposite()
{
	return __pImpl->IsGpuComposite();
}
void  IWindowBase::DirectComposite()
{
	__pImpl->DirectComposite();
}

void  IWindowBase::SetWindowMessageCallback(IWindowDelegate* p)
{
	__pImpl->SetWindowMessageCallback(p);
}

signal_mc<long>&  IWindowBase::SizeChangedEvent()
{
	return __pImpl->size_changed;
}

signal_mc<bool&>&  IWindowBase::OnCloseEvent()
{
    return __pImpl->on_close;
}

void ui::IWindowBase::ChangeSkinLayout(const wchar_t* szLayoutId)
{
	__pImpl->ChangeSkinLayout(szLayoutId);
}

bool ui::IWindowBase::IsSizeMoveIng()
{
	return __pImpl->IsSizeMoveIng();
}


void  IWindowBase::GetWindowNormalRect(Rect* prc)
{
	__pImpl->GetWindowNormalRect(prc);
}
void  IWindowBase::SetWindowNormalRect(const Rect* prc)
{
	__pImpl->SetWindowNormalRect(prc);
}

void  IWindowBase::UpdateDesktopLayout()
{
	__pImpl->UpdateDesktopLayout();
}
#endif
#if 0
	extern "C"
		void GetWindowNormalRect(HWND hWnd, Rect* prc)
	{
		UIASSERT(hWnd);
		if (!hWnd)
			return;

		UIASSERT(prc);
		if (IsIconic(hWnd) || IsZoomed(hWnd))
		{
			WINDOWPLACEMENT wndplacement = { 0 };
			wndplacement.length = sizeof(wndplacement);
			GetWindowPlacement(hWnd, &wndplacement);

			CopyRect(prc, &wndplacement.rcNormalPosition);
		}
		else
		{
			::GetWindowRect(hWnd, prc);
		}
	}
#endif

}