#include "stdafx.h"
#include "window.h"
#include "window_desc.h"

Window::Window(IWindow* p) : WindowBase(p)
{
    m_pIWindow = p;
    this->SetDescription(WindowDescription::Get());
}

Window::~Window(void)
{
}

