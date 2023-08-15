#include <iostream>
using namespace std;

#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include "svg/include/interface/irect.h"

class MainWindow {
public:
  MainWindow(ui::IResBundle *res) : m_window(res) {
    create_ui();
    
    ui::Rect rc = {100, 100, 500, 400};
    m_window->Create(rc);
    

    m_window->Show();
    // m_window->DestroySignal().connect(on_window_destroy, app.get());
    m_window->connect("destroy", 
      ui::Slot(&MainWindow::on_window_destroy, this, res->GetUIApplication()));
  }

  void create_ui() {
    auto* panel = ui::IPanel::CreateInstance(m_window->GetResBundle());
    panel->AddAttribute(XML_LAYOUT_ITEM_LEFT, L"0");
    panel->AddAttribute(XML_LAYOUT_ITEM_TOP, L"0");
    panel->AddAttribute(XML_LAYOUT_ITEM_RIGHT, L"0");
    panel->AddAttribute(XML_LAYOUT_ITEM_BOTTOM, L"0");
    m_window->AddChild(panel);
    panel->InitDefaultAttrib();

    auto* rect = ui::svg::IRect::CreateInstance(m_window->GetResBundle());
    rect->AddAttribute(XML_LAYOUT_ITEM_LEFT, L"100");
    rect->AddAttribute(XML_LAYOUT_ITEM_TOP, L"100");
    rect->AddAttribute(XML_LAYOUT_ITEM_RIGHT, L"100");
    rect->AddAttribute(XML_LAYOUT_ITEM_BOTTOM, L"100");
    panel->AddChild(rect);
    rect->InitDefaultAttrib();
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