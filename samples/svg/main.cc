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
    ui::svg::SvgPtr svg(m_window->GetResource());
    svg->AddAttribute(XML_LAYOUT_ITEM_LEFT, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_TOP, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_RIGHT, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_BOTTOM, "0");
    svg->InitDefaultAttrib();
    m_svg = static_cast<ui::svg::ISvg*>(m_window->AddChild(svg.release()));
  }

  void test1() {
    static const char* buffer = R"(
      <svg version="1.1" 
        baseProfile="full" 
        width="300" height="200" 
        xmlns="http://www.w3.org/2000/svg"> 
      <rect width="100%" height="100%" fill="red" /> 
      <circle cx="150" cy="100" r="80" fill="green" /> 
      <text x="150" y="125" font-size="60" text-anchor="middle" fill="white">SVG</text> 
    </svg>
    )";

    m_svg->Load(buffer);
  }

  void on_window_destroy(ui::IApplication *uiapp, ui::Event* event) {
    printf("on_window_destroy\n");
    uiapp->Quit();
  }

private:
  ui::WindowPtr m_window;
  ui::svg::ISvg* m_svg;
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