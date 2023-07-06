#include <iostream>
using namespace std;

#include "ui/sdk/src/object/uiapplication.h"
#include "ui/sdk/src/object/window/window.h"

void on_window_destroy(ui::UIApplication* uiapp) {
    printf("on_window_destroy\n");
    uiapp->Quit();
}

int mac_main();
int main() {
    // mac_main();
    ui::UIApplication uiapp;
    ui::Window window;

    ui::Rect rc = {100, 100, 500, 400};
    window.Create(rc);
    window.Show();
    window.DestroySignal().connect(on_window_destroy, &uiapp);

    uiapp.Run();
    return 0;
}