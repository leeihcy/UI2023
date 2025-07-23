#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"

void on_window_destroy(ui::IApplication *uiapp, ui::Event *) {
  uiapp->Quit();
}
void on_window_paint(ui::IWindow* window, ui::Event *e) {
  ui::IRenderTarget *pRT = static_cast<ui::WindowPaintEvent *>(e)->rt;
#if 1
  // 测量一个200x200 point 区域的真实显示大小。
  // 在macos高清屏上，用截屏测量应该是200x200，像素大小为400x400
  ui::Rect rc_fill = ui::Rect::MakeXYWH(5, 5, 200, 200);
  window->DpiScaleRect(&rc_fill);

  ui::Color color_fill = ui::Color::red();
  pRT->DrawRect(&rc_fill, &color_fill);


  ui::DrawTextParam param;
  param.text = "200x200 Region";
  param.bound = rc_fill; 
  pRT->DrawString(nullptr, &param);

  // for (int i = 0; i < 5; i++) {
  //   pRT->DrawString(nullptr, &param);
  //   param.bound.Offset(0, 150); 
  // }
  // // param.text = "好好学习，天天向上";
  // // pRT->DrawString(nullptr, &param);
#endif
}

int main() {
  char version[32] = {0};
  ui::SDKVersion::GetVersionText(version, 32);

  ui::ApplicationPtr app;
  ui::IResource *resource = app->LoadResource("sample/dpi_layout");

  ui::WindowPtr window(resource);

  ui::Rect rc = ui::Rect::MakeXYWH(0, 0, 500, 400);
  window->Create("dpi_layout", &rc);
  window->SetTitle("Dpi & Layout Demo");
  window->Show();
  window->connect(WINDOW_DESTROY_EVENT, ui::Slot(on_window_destroy, app.get()));
  window->connect(WINDOW_PAINT_EVENT, ui::Slot(on_window_paint, window.get()));

  app->Run();

  return 0;
}