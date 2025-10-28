#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include "sdk/include/interface/icontrol.h"

void on_window_destroy(ui::IApplication *uiapp, ui::Event *) {
  uiapp->Quit();
}

bool on_test_timer(ui::IWindow *window,  ui::TimerID timer_id) {
  // invalidate test
  return true;
}

void on_button_click(ui::Event* e) {
  ui::IObject* sender = e->Target();
  printf("button clicked, id=%s\n", sender->GetId());
}

void bind_controls(ui::IWindow *window) {
  // 通过冒泡
  ui::IRootObject* root = window->GetRootObject();
  root->connect(BUTTON_CLICK_EVENT,ui::Slot(on_button_click));

  ui::IObject *btn = nullptr;
  btn = window->TryFindObject("btn_8");
  if (btn) {
    btn->SetEnable(false);
  }
}

int main() {
  char version[32] = {0};
  ui::SDKVersion::GetVersionText(version, 32);

  ui::ApplicationPtr app;
  ui::IResourceBundle *resource = app->LoadResource("bundle/control");

  ui::WindowPtr window(resource);

  window->Create("control", nullptr);
  window->SetTitle("Control Demo");
  window->Show();
  window->connect(WINDOW_DESTROY_EVENT, ui::Slot(on_window_destroy, app.get()));
  bind_controls(window.get());

  ui::TimerID timer_id =
      app->SetTimer(2000, ui::Slot(&on_test_timer, window.get()));
  app->Run();
  app->KillTimer(timer_id);

  return 0;
}