#include <iostream>
using namespace std;

#include "ui/sdk/src/object/uiapplication.h"
#include "ui/sdk/src/object/window/window.h"

int main() {
    ui::UIApplication uiapp;
    ui::Window window;

    ui::Rect rc = {100, 100, 500, 400};
    window.Create(rc);
    window.Show();
    uiapp.Run();
    return 0;
}