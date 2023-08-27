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
    
    test1();

    m_window->Show();
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

  void test2() {
    static const char* buffer = R"(
      <?xml version="1.0" standalone="no"?>
      <svg width="200" height="250" version="1.1" xmlns="http://www.w3.org/2000/svg">

        <rect x="10" y="10" width="30" height="30" stroke="black" fill="transparent" stroke-width="5"/>
        <rect x="60" y="10" rx="10" ry="10" width="30" height="30" stroke="black" fill="transparent" stroke-width="5"/>

        <circle cx="25" cy="75" r="20" stroke="red" fill="transparent" stroke-width="5"/>
        <ellipse cx="75" cy="75" rx="20" ry="5" stroke="red" fill="transparent" stroke-width="5"/>

        <line x1="10" x2="50" y1="110" y2="150" stroke="orange" stroke-width="5"/>
        <polyline points="60 110 65 120 70 115 75 130 80 125 85 140 90 135 95 150 100 145"
            stroke="orange" fill="transparent" stroke-width="5"/>

        <polygon points="50 160 55 180 70 180 60 190 65 205 50 195 35 205 40 190 30 180 45 180"
            stroke="green" fill="transparent" stroke-width="5"/>

        <path d="M20,230 Q40,205 50,230 T90,230" fill="none" stroke="blue" stroke-width="5"/>
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