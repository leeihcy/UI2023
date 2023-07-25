#include "stdafx.h"
#include "dwmhelper.h"

DwmHelper::DwmHelper()
{
    pDwmExtendFrameIntoClientArea = nullptr;
    pDwmEnableBlurBehindWindow = nullptr;
    pDwmIsCompositionEnabled = nullptr;
    pDwmGetWindowAttribute = nullptr;
    pDwmSetWindowAttribute = nullptr;
    pDwmDefWindowProc = nullptr;

    m_hModule = LoadLibrary(_T("dwmapi.dll"));
    if (nullptr == m_hModule)
        return;

    pDwmExtendFrameIntoClientArea = (pfnDwmExtendFrameIntoClientArea)GetProcAddress(m_hModule, "DwmExtendFrameIntoClientArea");
    pDwmEnableBlurBehindWindow = (pfnDwmEnableBlurBehindWindow)GetProcAddress(m_hModule, "DwmEnableBlurBehindWindow");
    pDwmIsCompositionEnabled = (pfnDwmIsCompositionEnabled)GetProcAddress(m_hModule, "DwmIsCompositionEnabled");
    pDwmGetWindowAttribute = (pfnDwmGetWindowAttribute)GetProcAddress(m_hModule, "DwmGetWindowAttribute");
    pDwmSetWindowAttribute = (pfnDwmSetWindowAttribute)GetProcAddress(m_hModule, "DwmSetWindowAttribute");
	pDwmDefWindowProc = (pfnDwmDefWindowProc)GetProcAddress(m_hModule, "DwmDefWindowProc");

	if (UI::util::IsWin8OrLater())
	{
		pSetWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(
			GetModuleHandle(L"user32.dll"), "SetWindowCompositionAttribute");
	}
}
DwmHelper::~DwmHelper()
{
    if (m_hModule)
    {
        FreeLibrary(m_hModule);
        m_hModule = nullptr;
    }
}

DwmHelper*  DwmHelper::GetInstance()
{
    static DwmHelper  dwm;
    return &dwm;
}

bool  DwmHelper::IsEnable()
{ 
    if (nullptr == m_hModule)
        return false;

    if (nullptr == pDwmIsCompositionEnabled)
        return false;

    BOOL b = FALSE;
    pDwmIsCompositionEnabled(&b);

    return b ? true:false;
};

// 开启/禁用窗口显示、隐藏、最化小、还原时的动画效果
void UI::DwmHelper::EnableTransition(HWND hWnd, bool b)
{
    if (!hWnd)
        return;
    if (!pDwmSetWindowAttribute)
        return;

    BOOL bEnable = b?FALSE:TRUE;
    pDwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED,
        &bEnable, sizeof(bEnable));
}

// if (!pDwmSetWindowAttribute)
// return false;
// 
// BOOL enable = TRUE;
// pDwmSetWindowAttribute(hWnd, 0x010001, &enable, sizeof(enable));
// return true;
// 开启win10上的磨砂效果，调用user32里的一个未公开api
bool  DwmHelper::EnableWindows10Blur(HWND hWnd, bool b)
{
	if (!hWnd)
		return false;

	if (!pSetWindowCompositionAttribute)
		return false;

	ACCENT_POLICY accent;
	memset(&accent, 0, sizeof(accent));

	WINDOWCOMPOSITIONATTRIBDATA data;
	data.Attrib = WCA_ACCENT_POLICY;
	data.pvData = &accent;
	data.cbData = sizeof(accent);

	if (!b)
		accent.AccentState = ACCENT_DISABLED;
	else
		accent.AccentState = ACCENT_ENABLE_BLURBEHIND;
	pSetWindowCompositionAttribute(hWnd, &data);

// 	if (pDwmSetWindowAttribute)
// 	{
// 		DWMNCRENDERINGPOLICY ncrp = DWMNCRP_DISABLED;
// 		pDwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
// 	}
	return true;
}

// 开启win10的窗口透明效果
bool  DwmHelper::EnableWindows10Trans(HWND hWnd, bool b)
{
	if (!hWnd)
		return false;

	if (!pSetWindowCompositionAttribute)
		return false;

	ACCENT_POLICY accent;
	memset(&accent, 0, sizeof(accent));
	accent.AccentState = ACCENT_ENABLE_TRANSPARENTGRADIENT;
	accent.GradientColor = 0x01000000;  // 用0x00反而不透明了

	WINDOWCOMPOSITIONATTRIBDATA data;
	data.Attrib = WCA_ACCENT_POLICY;
	data.pvData = &accent;
	data.cbData = sizeof(accent);

	pSetWindowCompositionAttribute(hWnd, &data);

	return true;
}