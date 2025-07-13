#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"

void on_window_destroy(ui::IApplication *uiapp, ui::Event *) {
  printf("%s", "on_window_destroy\n");
  uiapp->Quit();
}
void on_window_paint(ui::IWindow* window, ui::Event *e) {
  ui::IRenderTarget *pRT = static_cast<ui::WindowPaintEvent *>(e)->rt;

  ui::Rect rcwindow;
  window->GetObjectClientRect(&rcwindow);
  ui::Color color_background = ui::Color::white();
  pRT->DrawRect(&rcwindow, &color_background);

  ui::Color colors[3] = {ui::Color::MakeRGB(255, 0, 0),
                         ui::Color::MakeRGB(0, 255, 0),
                         ui::Color::MakeRGB(0, 0, 255)};
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

  ui::Rect rc = ui::Rect::MakeXYWH(0, 0, 500, 400);
  window->Create(nullptr, &rc);
  window->SetTitle("1.你好Hello! -- 窗口创建");
  window->Show();
  window->connect(WINDOW_DESTROY_EVENT, ui::Slot(on_window_destroy, app.get()));
  window->connect(WINDOW_PAINT_EVENT, ui::Slot(on_window_paint, window.get()));

  app->Run();

  return 0;
}