#pragma once
#include "Src\UIObject\Window\windowbase.h"

namespace UI
{

    // TODO: 整理WindowBase类，将一些逻辑剥离出来放到Window类中

class Window : public WindowBase
{
public:
	Window(IWindow* p);
	~Window(void);

	UI_BEGIN_MSG_MAP()
        UIMSG_QUERYINTERFACE(Window)
	UI_END_MSG_MAP_CHAIN_PARENT(WindowBase)

    IWindow* GetIWindow() { return m_pIWindow; }

protected:
    IWindow*   m_pIWindow;
};



}