#include "stdafx.h"
#include "window.h"
#include "window_meta.h"

Window::Window(IWindow* p) : WindowBase(p)
{
    m_pIWindow = p;
    this->SetMeta(WindowMeta::Get());
}

Window::~Window(void)
{
}

