#include <iostream>
using namespace std;

#include "ui/sdk/src/object/uiapplication.h"
#include "ui/sdk/src/object/window/window.h"

ui::UIApplication uiapp;

void on_window_destroy() {
    printf("on_window_destroy\n");
    uiapp.Quit();
}

int main() {
    ui::Window window;

    ui::Rect rc = {100, 100, 500, 400};
    window.Create(rc);
    window.Show();
    window.DestroySignal().connect(on_window_destroy);

    uiapp.Run();
    return 0;
}