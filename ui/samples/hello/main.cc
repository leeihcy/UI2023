#include <iostream>
using namespace std;

#include "ui/sdk/src/object/application/uiapplication.h"
#include "ui/sdk/src/object/window/window.h"

void on_window_destroy(ui::UIApplication* uiapp) {
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
    ui::UIApplication uiapp;
    ui::Window window;

    ui::Rect rc = {100, 100, 500, 400};
    window.Create(rc);
    window.SetTitle("你好Hello!");
    window.Show();
    window.DestroySignal().connect(on_window_destroy, &uiapp);
    window.PaintSignal().connect(on_window_paint);

    uiapp.Run();
#endif
    return 0;
}