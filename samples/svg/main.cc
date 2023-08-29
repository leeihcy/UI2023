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
    
    // test_hello_svg();
    test_shapes();
    // test_viewbox();
    // test_iconfont();
    

    m_window->Show();
    m_window->connect("destroy", 
      ui::Slot(&MainWindow::on_window_destroy, this, res->GetUIApplication()));
  }

  void create_ui() {
    ui::SvgPtr svg(m_window->GetResource());
    svg->AddAttribute(XML_LAYOUT_ITEM_LEFT, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_TOP, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_RIGHT, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_BOTTOM, "0");
    svg->InitDefaultAttrib();
    m_svg = static_cast<ui::ISvg*>(m_window->AddChild(svg.release()));
  }

  void test_hello_svg() {
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
  void test_viewbox() {
    static const char* buffer1 = R"(
      <svg width="150" height="150" viewBox="0 0 3000 3000" xmlns="http://www.w3.org/2000/svg">
        <rect x="0" y="0" width="100%" height="100%"/>
        <circle cx="50%" cy="50%" r="40" fill="white"/>
      </svg>
    )";
    static const char* buffer2 = R"(
      <svg width="150" height="150" viewBox="0 0 300 300" xmlns="http://www.w3.org/2000/svg">
        <rect x="0" y="0" width="100%" height="100%"/>
        <circle cx="50%" cy="50%" r="40" fill="white"/>
      </svg>	
    )";
    static const char* buffer3 = R"(
      <svg width="150" height="150" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
        <rect x="0" y="0" width="100%" height="100%"/>
        <circle cx="50%" cy="50%" r="40" fill="white"/>
      </svg>
    )";
    m_svg->Load(buffer2);
  }
  void test_shapes() {
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

  void test_iconfont() {
    static const char* buffer = R"(<?xml version="1.0" standalone="no"?><!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd"><svg t="1693018373729" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="1350" xmlns:xlink="http://www.w3.org/1999/xlink" width="200" height="200"><path d="M512.1 64.3c-247.3 0-447.9 200.6-447.9 447.9S264.8 960 512.1 960 960 759.4 960 512.1 759.4 64.3 512.1 64.3zM281.3 588.5c-42.2 0-76.4-34.2-76.4-76.4s34.2-76.4 76.4-76.4 76.4 34.2 76.4 76.4-34.2 76.4-76.4 76.4z m229.7 0c-42.2 0-76.4-34.2-76.4-76.4s34.2-76.4 76.4-76.4 76.4 34.2 76.4 76.4-34.2 76.4-76.4 76.4z m229.7 0c-42.2 0-76.4-34.2-76.4-76.4s34.2-76.4 76.4-76.4 76.4 34.2 76.4 76.4-34.2 76.4-76.4 76.4z" p-id="1351"></path></svg>)";
    m_svg->Load(buffer);
  }

  void on_window_destroy(ui::IApplication *uiapp, ui::Event* event) {
    printf("on_window_destroy\n");
    uiapp->Quit();
  }

private:
  ui::WindowPtr m_window;
  ui::ISvg* m_svg;
};

int main() {
  ui::ApplicationPtr app;
  ui::SvgRegisterObjects(app.get());

  {
    MainWindow main_window(app->RootBundle());
    app->Run();
  }
  return 0;
}