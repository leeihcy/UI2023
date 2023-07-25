#include "stdafx.h"
#include "gesturehelper.h"

namespace ui
{
GestureHelper*  GetGestureInstance()
{
    static GestureHelper dwm;
    return &dwm;
}

GestureHelper::GestureHelper()
{
    pSetGestureConfig = nullptr;
    pGetGestureInfo = nullptr;
    pCloseGestureInfoHandle = nullptr;
	pBeginPanningFeedback = nullptr;
	pEndPanningFeedback = nullptr;
	pUpdatePanningFeedback = nullptr;

    m_hModule = LoadLibrary(_T("User32.dll"));
    if (nullptr == m_hModule)
        return;

	m_hModuleUxTheme = LoadLibrary(_T("UxTheme.dll"));
	if (nullptr == m_hModuleUxTheme)
	{
		FreeLibrary(m_hModule);
		return;
	}

    pSetGestureConfig = (pfnSetGestureConfig)
			GetProcAddress(m_hModule, "SetGestureConfig");
    pGetGestureInfo = (pfnGetGestureInfo)
			GetProcAddress(m_hModule, "GetGestureInfo");
    pCloseGestureInfoHandle = (pfnCloseGestureInfoHandle)
			GetProcAddress(m_hModule, "CloseGestureInfoHandle");
	pBeginPanningFeedback = (pfnBeginPanningFeedback)
		GetProcAddress(m_hModuleUxTheme, "BeginPanningFeedback");
	pEndPanningFeedback = (pfnEndPanningFeedback)
		GetProcAddress(m_hModuleUxTheme, "EndPanningFeedback");
	pUpdatePanningFeedback = (pfnUpdatePanningFeedback)
		GetProcAddress(m_hModuleUxTheme, "UpdatePanningFeedback");
}

GestureHelper::~GestureHelper()
{
    if (m_hModule)
    {
        FreeLibrary(m_hModule);
        m_hModule = nullptr;
    }
	if (m_hModuleUxTheme)
	{
		FreeLibrary(m_hModuleUxTheme);
		m_hModuleUxTheme = nullptr;
	}
}

bool GestureHelper::IsValid()
{
    return pSetGestureConfig ? true:false;
}



}