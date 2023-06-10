#include "stdafx.h"
#include "Inc\Interface\ihwndhost.h"
#include "Src\UIObject\HwndHost\HwndHost.h"

namespace UI
{

UI_IMPLEMENT_INTERFACE(HwndHost, Control)

void  IHwndHost::Attach(HWND hWnd)
{
	__pImpl->Attach(hWnd);
}
HWND  IHwndHost::Detach()
{
	return __pImpl->Detach();
}
}
