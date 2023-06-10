#ifndef _UI_IHWNDHOST_H_
#define _UI_IHWNDHOST_H_
#include "icontrol.h"

namespace UI
{

class HwndHost;
struct UIAPI_UUID(54929797-CB09-45A2-BA90-9101739A399E) 
    IHwndHost : public IControl
{
	void  Attach(HWND hWnd);
	HWND  Detach();

    UI_DECLARE_INTERFACE(HwndHost);
};
}

#endif // _UI_IHWNDHOST_H_