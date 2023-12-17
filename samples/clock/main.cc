#include <iostream>
using namespace std;

#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include <string_view>

void on_window_destroy(ui::IApplication *uiapp, ui::Event *) {
  printf("on_window_destroy\n");
  uiapp->Quit();
}
void on_window_paint(ui::Event *e) {
  ui::IRenderTarget *pRT = static_cast<ui::WindowPaintEvent *>(e)->rt;

  // ui::Color colors[3] = {ui::Color::MakeRGB(255, 0, 0),
  //                        ui::Color::MakeRGB(0, 255, 0),
  //                        ui::Color::MakeRGB(0, 0, 255)};
  // static int i = 0;
  // i++;
  // if (i >= 3) {
  //   i = 0;
  // }
  // ui::Rect rc = {100, 100, 200, 200};
  // pRT->DrawRect(&rc, &colors[i]);
}

int main() {
  ui::ApplicationPtr app;
  ui::IResource* resource = app->LoadResource("sample/clock");

  ui::WindowPtr window(resource);

  ui::Rect rc = {100, 100, 500, 400};
  window->Create("clock", rc);
  window->SetTitle("你好Hello!");
  window->Show();
  window->connect(WINDOW_DESTROY_EVENT, ui::Slot(on_window_destroy, app.get()));
  window->connect(WINDOW_PAINT_EVENT, ui::Slot(on_window_paint));

  app->Run();

  return 0;
}