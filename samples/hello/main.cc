#include <iostream>
using namespace std;

#include "ui/sdk/include/interface/iuiapplication.h"
#include "ui/sdk/include/interface/iwindow.h"

void on_window_destroy(ui::IApplication *uiapp) {
  printf("on_window_destroy\n");
  uiapp->Quit();
}
void on_window_paint(ui::IRenderTarget *pRT) {
  ui::Color colors[3] = {ui::Color(255, 0, 0, 255), ui::Color(0, 255, 0, 255),
                         ui::Color(0, 0, 255, 255)};
  static int i = 0;
  i++;
  if (i >= 3) {
    i = 0;
  }
  ui::Rect rc = {100, 100, 200, 200};
  pRT->DrawRect(&rc, &colors[i]);
}

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
  window->PaintSignal().connect(on_window_paint);

  app->Run();

  return 0;
}