// StyleDemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "StyleDemo.h"
#include <combaseapi.h>
//#define interface struct
//#define PURE                    = 0

#include "UISDK\Inc\inc.h"
//#include "UISDK\Control\Inc\Base\inc.h"
#include "UISDK\Inc\Interface\iwindow.h"
//#include "UISDK\Control\Inc\Interface\ilabel.h"
//#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "samples\common\demo_common.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uisdk.lib")
//#pragma comment(lib, "uictrls.lib")

UI::IApplication* g_pUIApplication = nullptr;
UI::IResBundle* g_pSkinRes = nullptr;

TCHAR  g_szAppDir[MAX_PATH] = {0};
long   g_framecount = 0;


class DemoWindow : public UI::IWindowDelegate
{
public:
    DemoWindow()
    {
    }
    ~DemoWindow() {
        if (m_pWindow)
        {
            m_pWindow->DestroyWindow();
            m_pWindow->Release();
            m_pWindow = NULL;
        }
    }

    //UI_BEGIN_MSG_MAP_Ixxx(DemoWindow)
    //    UIMSG_WM_DESTROY(OnDestroy)
    //    UIMSG_WM_INITIALIZE(OnInitialize)
    //UI_END_MSG_MAP_CHAIN_PARENT(IWindow);

    void  Create(UI::IResBundle* skinres) {
        m_pWindow = UI::IWindow::CreateInstance(skinres);
        m_pWindow->SetWindowMessageCallback(
            static_cast<UI::IWindowDelegate*>(this));
        m_pWindow->Create(
            L"DemoWnd",
            0, 0, 0,
            0);

        m_pWindow->ShowWindow();
    }
    void  Destroy() {
        
    }  

private:
    virtual void  OnWindowInitialize() override {

    }
    virtual void  OnWindowDestroy() override {
        ::PostQuitMessage(0);
    }
    virtual BOOL  OnWindowMessage(
        UINT, WPARAM, LPARAM, LRESULT& lResult) override {
        return FALSE;
    }

    UI::IWindow* m_pWindow = nullptr;

    void OnInitialize()
    {

    }

    void OnDestroy()
    {
        PostQuitMessage(0); 
    }

private:
};


int APIENTRY _tWinMain(
                       HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR  lpCmdLine,
                       int  nCmdShow)
{
    Demo_GetAppDir(g_szAppDir);
    Demo_SetDllDir(g_szAppDir);

    TCHAR szSDKVersion[64] = {0};
    UI::UISDKVersion::GetVersionText(szSDKVersion, 64);

    CreateUIApplication(&g_pUIApplication);
    //UICtrl_RegisterUIObject(g_pUIApplication);

    g_pSkinRes = g_pUIApplication->LoadResBundle(LR"(C:\Users\libo\Desktop\github\UI2023\ui\samples\StyleDemo\skin\Default)");
    

    {
        DemoWindow wnd;
        wnd.Create(g_pSkinRes);

        g_pUIApplication->MsgHandleLoop();


    }
    g_pUIApplication->Release();
    return 0;
}