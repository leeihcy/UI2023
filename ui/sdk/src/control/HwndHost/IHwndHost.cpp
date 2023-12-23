#include "stdafx.h"
#include "include/interface/ihwndhost.h"
#include "src/UIObject\HwndHost\HwndHost.h"

namespace ui
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
