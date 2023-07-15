#include "interface/iwindow.h"
#include "base/uiapi.h"
#include "base/uicreator.h"
#include "base/uidefine.h"
#include "src/window/window.h"

#if 0
#include "Src\Resource\skinres.h"
#endif

using namespace ui;
UI_IMPLEMENT_INTERFACE(Window, Message)

void IWindow::Create(const Rect &rect) { __pImpl->Create(rect); }
void IWindow::SetTitle(const char *title) { __pImpl->SetTitle(title); }
void IWindow::Show() { __pImpl->Show(); }

ui::signal<void()> &IWindow::DestroySignal() { return __pImpl->DestroySignal(); }
ui::signal<void(SkCanvas &)> &IWindow::PaintSignal() {
  return __pImpl->PaintSignal();
}
#if 0
UI_IMPLEMENT_INTERFACE(WindowBase, Panel)
UI_IMPLEMENT_INTERFACE(Window, WindowBase)

HWND  IWindowBase::GetHWND()                                              
{ 
	return __pImpl->GetHWND(); 
}
BOOL  IWindowBase::IsChildWindow()                                        
{
	return __pImpl->IsChildWindow(); 
}
BOOL  IWindowBase::IsWindowVisible()
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
ISkinRes*  IWindowBase::GetSkinRes()                                      
{ 
	return __pImpl->GetSkinRes()->GetISkinRes(); 
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
IObject*  IWindowBase::GetObjectByPos(IObject* pObjParent, POINT* pt, bool bSkinBuilderInvoke)
{ 
    if (nullptr == pObjParent)
        return nullptr;

    Object* pRet = __pImpl->GetObjectByPos(pObjParent->GetImpl(), pt, bSkinBuilderInvoke); 
    if (pRet)
        return pRet->GetIObject();

    return nullptr;
}
bool  IWindowBase::Create(LPCTSTR szID, HWND hWndParent, RECT* prc, long lStyle, long lExStyle) 
{
	return __pImpl->Create(szID, hWndParent, prc, lStyle, lExStyle);
}
void  IWindowBase::DestroyWindow()
{
	return __pImpl->DestroyWindow();
}
bool  IWindowBase::Attach(HWND hWnd, LPCTSTR szID) 
{ 
	return __pImpl->Attach(hWnd, szID);
}
void  IWindowBase::Detach()
{
	__pImpl->Detach(); 
}
INT_PTR  IWindowBase::DoModal(LPCTSTR szID, HWND hWndParent, bool canResize ) 
{
	return __pImpl->DoModal(szID, hWndParent, canResize);
}
INT_PTR  IWindowBase::DoModal(HINSTANCE hResInst, UINT nResID, LPCTSTR szID, HWND hWndParent)
{
	return __pImpl->DoModal(hResInst, nResID, szID, hWndParent); 
}
HWND  IWindowBase::DoModeless(LPCTSTR szID, HWND hWndParent, bool canResize )
{
	return __pImpl->DoModeless(szID, hWndParent, canResize);
}
HWND  IWindowBase::DoModeless(HINSTANCE hResInst, UINT nResID, LPCTSTR szID, HWND hWndParent) 
{ 
	return __pImpl->DoModeless(hResInst, nResID, szID, hWndParent);
}
void  IWindowBase::EndDialog(INT_PTR n)
{ 
	return __pImpl->EndDialog(n);
}

void  IWindowBase::CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow )
{
	__pImpl->CalcWindowSizeByClientSize(sizeClient, pSizeWindow); 
}
void  IWindowBase::CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient )
{
	__pImpl->CalcClientRectByWindowRect(rcWindow, rcClient);
}
void  IWindowBase::SaveMemBitmap(TCHAR* szFile) 
{
	__pImpl->SaveMemBitmap(szFile);
}
void  IWindowBase::AlphaBlendMemBitmap(HDC hDC, RECT* prc, int alpha)
{
	__pImpl->AlphaBlendMemBitmap(hDC, prc, alpha);
}
void  IWindowBase::BitBltMemBitmap(HDC hDC, RECT* prc)
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

HRESULT  IWindowBase::SetDroppable(bool b) 
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

void UI::IWindowBase::ChangeSkinLayout(LPCTSTR szLayoutId)
{
	__pImpl->ChangeSkinLayout(szLayoutId);
}

bool UI::IWindowBase::IsSizeMoveIng()
{
	return __pImpl->IsSizeMoveIng();
}


void  IWindowBase::GetWindowNormalRect(LPRECT prc)
{
	__pImpl->GetWindowNormalRect(prc);
}
void  IWindowBase::SetWindowNormalRect(LPCRECT prc)
{
	__pImpl->SetWindowNormalRect(prc);
}

void  IWindowBase::UpdateDesktopLayout()
{
	__pImpl->UpdateDesktopLayout();
}
#endif
#if 0
namespace UI
{
	extern "C"
		void GetWindowNormalRect(HWND hWnd, LPRECT prc)
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
}
#endif