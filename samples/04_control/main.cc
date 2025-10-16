#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include "sdk/include/interface/icontrol.h"

void on_window_destroy(ui::IApplication *uiapp, ui::Event *) {
  uiapp->Quit();
}

bool on_test_timer(ui::IWindow *window,  ui::TimerID timer_id) {
  // invalidate test
  auto* label = window->TryFindObject("title");
  if (label){ 
    label->Invalidate(); 
  };
  
  return true;
}

void on_button_click(ui::Event* e) {
  ui::IButton* sender = static_cast<ui::ButtonClickedEvent*>(e)->button;
  printf("button clicked, id=%s\n", sender->GetId());
}

int main() {
  char version[32] = {0};
  ui::SDKVersion::GetVersionText(version, 32);

  ui::ApplicationPtr app;
  ui::IResource *resource = app->LoadResource("sample/control");

  ui::WindowPtr window(resource);

  auto rc = ui::Rect::MakeXYWH(100, 100, 600, 600);
  window->Create("control", &rc);
  window->SetTitle("Control Demo");
  window->Show();
  window->connect(WINDOW_DESTROY_EVENT, ui::Slot(on_window_destroy, app.get()));

  auto* btn_1 = window->TryFindObject("btn_1");
  if (btn_1) {
    btn_1->connect(BUTTON_CLICK_EVENT, ui::Slot(on_button_click));
  }

  ui::TimerID timer_id =
      app->SetTimer(2000, ui::Slot(&on_test_timer, window.get()));
  app->Run();
  app->KillTimer(timer_id);

  return 0;
}