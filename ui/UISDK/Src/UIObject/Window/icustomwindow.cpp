#include "stdafx.h"
#include "Inc\Interface\icustomwindow.h"
#include "Src\UIObject\Window\customwindow.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE(CustomWindow, Window)

// ICustomWindow::ICustomWindow() : ICustomWindow(CREATE_IMPL_TRUE) {
// };

void  ICustomWindow::SetWindowResizeType(UINT nType)         
{ 
    __pImpl->SetResizeCapability(nType); 
}
void  ICustomWindow::SetWindowTransparentType(WINDOW_TRANSPARENT_TYPE eMode) 
{
    __pImpl->SetWindowTransparentType(eMode); 
}
WINDOW_TRANSPARENT_TYPE  ICustomWindow::GetWindowTransparentType()    
{
    return __pImpl->GetWindowTransparentType(); 
}
IWindowTransparent*  ICustomWindow::GetWindowTransparent()   
{ 
    return __pImpl->GetWindowTransparentPtr();
}
}