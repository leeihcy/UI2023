#ifndef _UI_ICUSTOMWINDOW_H_
#define _UI_ICUSTOMWINDOW_H_

#include "iwindow.h"

namespace UI
{
class CustomWindow;
interface IWindowTransparent;

interface UIAPI_UUID(AB69FBED-6311-4980-8444-B5D880568B10) ICustomWindow
 : public IWindow
{
	//ICustomWindow();  // 内部直接调用ICustomWindow(CREATE_IMPL_TRUE)

    void  SetWindowResizeType(UINT nType);
    void  SetWindowTransparentType(WINDOW_TRANSPARENT_TYPE eMode);
    WINDOW_TRANSPARENT_TYPE  GetWindowTransparentType();
	IWindowTransparent*  GetWindowTransparent();

    UI_DECLARE_INTERFACE(CustomWindow);
};
typedef ObjectCreator<ICustomWindow>  CCustomWindow;

}

#endif // _UI_ICUSTOMWINDOW_H_