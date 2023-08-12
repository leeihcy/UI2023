#include <iostream>
using namespace std;

#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"

class MainWindow {
public:
  MainWindow(ui::IResBundle *res) : m_window(res) {
    ui::Rect rc = {100, 100, 500, 400};
    m_window->Create(rc);
    m_window->Show();
    // m_window->DestroySignal().connect(on_window_destroy, app.get());
    m_window->connect("destroy", 
      ui::Slot(&MainWindow::on_window_destroy, this, res->GetUIApplication()));
  }

  void on_window_destroy(ui::IApplication *uiapp, ui::Event* event) {
    printf("on_window_destroy\n");
    uiapp->Quit();
  }

private:
  ui::WindowPtr m_window;
};

int main() {
  ui::ApplicationPtr app;
  {
    MainWindow main_window(app->RootBundle());
    app->Run();
  }
  return 0;
}