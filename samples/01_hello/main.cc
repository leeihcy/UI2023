#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include <cstdio>

void on_window_destroy(ui::IApplication *uiapp, ui::Event *) {
  printf("%s", "on_window_destroy\n");
  uiapp->Quit();
}

void on_lbutton_down(ui::IWindow* window, ui::Event* e) {
  ui::Rect rc;
  window->GetRootObject()->GetClientRectWithZeroOffset(&rc);
  window->GetRootObject()->Invalidate(&rc);
}

void on_window_paint(ui::IWindow* window, ui::Event *e) {
  ui::IRenderTarget *pRT = static_cast<ui::WindowPaintEvent *>(e)->rt;

#if 1
  ui::Rect rc_client;
  window->GetRootObject()->GetClientRectInObject(&rc_client);
  ui::Color color_background = ui::Color::white();
  pRT->FillRect(rc_client, color_background);
#endif

  // 渐变颜色
  ui::Color colors[] = {ui::Color::MakeRGB(255, 0, 0),
                         ui::Color::MakeRGB(0, 255, 0),
                         ui::Color::MakeRGB(0, 0, 255),
                         ui::Color::MakeRGB(255, 0, 255),
                        ui::Color::MakeRGB(255, 0, 0)};
  static int counter = 0;
  counter+=16;
  ui::Color color;
  int index = (counter & 0x300) >> 8;
  ui::Color from = colors[index];
  ui::Color to = colors[index+1];

  color.a = 255;
  float progress = (counter & 0xff) / 256.f;
  color.r = (ui::byte)((int)from.r + ((int)to.r-(int)from.r) * progress);
  color.g = (ui::byte)((int)from.g + ((int)to.g-(int)from.g) * progress);
  color.b = (ui::byte)((int)from.b + ((int)to.b-(int)from.b) * progress);


  // 弹跳区域
  static int x_direction = 1;
  static int y_direction = 1;
  static ui::Rect region = {100, 100, 200, 200};
  constexpr int step = 5;
  region.Offset(step * x_direction, step * y_direction);

  if (rc_client.left >= region.left) {
    x_direction = 1;
  } 
  else if (rc_client.right <= region.right) {
    x_direction = -1;
  }
  if (rc_client.top >= region.top) {
    y_direction = 1;
  } else if (rc_client.bottom <= region.bottom) {
    y_direction = -1;
  }

  pRT->FillRect(region, color);
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
  window->Connect(WINDOW_DESTROY_EVENT, ui::Slot(on_window_destroy, app.get()));
  window->Connect(WINDOW_PAINT_EVENT, ui::Slot(on_window_paint, window.get()));
  window->Connect("lbutton_down", ui::Slot(&on_lbutton_down, window.get()));
  window->Show();
  
  app->Run();

  return 0;
}