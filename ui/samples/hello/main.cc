#include <iostream>
using namespace std;

#include "ui/sdk/include/interface/iuiapplication.h"
#include "ui/sdk/include/interface/iwindow.h"

void on_window_destroy(ui::IUIApplication* uiapp) {
    printf("on_window_destroy\n");
    uiapp->Quit();
}
void on_window_paint(SkCanvas& canvas) {
    canvas.clear(SK_ColorRED);

    SkPaint paint;
    canvas.drawRoundRect(SkRect::MakeLTRB(100, 100, 300, 300), 10, 10, paint);
}

int mac_main();
int main() {
#if 0
    mac_main();
#else
    ui::IUIApplication* uiapp;

    // wchar szSDKVersion[64] = {0};
    // UI::UISDKVersion::GetVersionText(szSDKVersion, 64);

    CreateUIApplication(&uiapp);
    //UICtrl_RegisterUIObject(g_pUIApplication);

    ui::ISkinRes* skinres = uiapp->LoadSkinRes(
        LR"(/Users/libo/2030/github/UI2023/ui/samples/StyleDemo/skin/Default)");

    ui::IWindow* pWindow = ui::IWindow::CreateInstance(skinres);

    ui::Rect rc = {100, 100, 500, 400};
    pWindow->Create(rc);
    pWindow->SetTitle("你好Hello!");
    pWindow->Show();
    pWindow->DestroySignal().connect(on_window_destroy, uiapp);
    pWindow->PaintSignal().connect(on_window_paint);

    uiapp->Run();

    pWindow->Release();
    uiapp->Release();

#endif
    return 0;
}