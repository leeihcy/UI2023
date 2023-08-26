#include <iostream>
using namespace std;

#include "sdk/include/inc.h"
#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include "svg/include/inc.h"
// #include "svg/include/interface/irect.h"
#include "svg/include/interface/isvg.h"

class MainWindow {
public:
  MainWindow(ui::IResource *res) : m_window(res) {
    create_ui();
    
    ui::Rect rc = {100, 100, 500, 400};
    m_window->Create(rc);
    

    m_window->Show();
    // m_window->DestroySignal().connect(on_window_destroy, app.get());
    m_window->connect("destroy", 
      ui::Slot(&MainWindow::on_window_destroy, this, res->GetUIApplication()));
  }

  void create_ui() {
    ui::svg::SvgPtr panel(m_window->GetResource());
    panel->AddAttribute(XML_LAYOUT_ITEM_LEFT, "0");
    panel->AddAttribute(XML_LAYOUT_ITEM_TOP, "0");
    panel->AddAttribute(XML_LAYOUT_ITEM_RIGHT, "0");
    panel->AddAttribute(XML_LAYOUT_ITEM_BOTTOM, "0");
    panel->InitDefaultAttrib();

    // ui::svg::RectPtr rect(m_window->GetResource());
    // rect->AddAttribute(L"x", L"100");
    // rect->AddAttribute(L"y", L"100");
    // rect->AddAttribute(L"width", L"100");
    // rect->AddAttribute(L"height", L"100");
    // rect->InitDefaultAttrib();

    // panel->AddChild(rect.release());
    m_window->AddChild(panel.release());
    // m_window->AddChild(rect.release());
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
  ui::svg::RegisterObjects(app.get());

  {
    MainWindow main_window(app->RootBundle());
    app->Run();
  }
  return 0;
}