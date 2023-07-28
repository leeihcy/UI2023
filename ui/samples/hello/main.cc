#include <iostream>
using namespace std;

#include "ui/sdk/include/interface/iuiapplication.h"
#include "ui/sdk/include/interface/iwindow.h"

void on_window_destroy(ui::IApplication *uiapp) {
  printf("on_window_destroy\n");
  uiapp->Quit();
}
// void on_window_paint(SkCanvas& canvas) {
//     canvas.clear(SK_ColorRED);

//     SkPaint paint;
//     canvas.drawRoundRect(SkRect::MakeLTRB(100, 100, 300, 300), 10, 10,
//     paint);
// }

int main() {
  char version[32] = {0};
  ui::SDKVersion::GetVersionText(version, 32);
  printf("ui sdk verson: %s\n", version);

  ui::ApplicationPtr app;
  ui::WindowPtr window(app->RootBundle());

  ui::Rect rc = {100, 100, 500, 400};
  window->Create(rc);
  window->SetTitle("你好Hello!");
  window->Show();
  window->DestroySignal().connect(on_window_destroy, app.get());
  // window->PaintSignal().connect(on_window_paint);

  app->Run();

  return 0;
}