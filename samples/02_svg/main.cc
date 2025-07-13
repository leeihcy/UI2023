#include <fstream>
#include <iostream>
using namespace std;

#include "sdk/include/inc.h"
#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include "sdk/include/util/path.h"
#include "svg/include/inc.h"
#include "svg/include/interface/isvg.h"

const char *samples[] = {
    "hello_svg.svg", "use.svg",       "transform_1.svg", "group.svg",
    "tiger.svg",     "shapes.svg",    "viewbox_1.svg",   "viewbox_2.svg",
    "viewbox_3.svg", "icon_font.svg",
};
const constexpr int samples_count = std::size(samples);
static int samples_index = 0;

class MainWindow {
public:
  MainWindow(ui::IResource *res) : m_window(res) {
    create_ui();

    ui::Rect rc = {0, 0, 500, 400};
    m_window->Create(nullptr, &rc);

    load_next();

    m_window->Show();
    m_window->connect("destroy", ui::Slot(&MainWindow::on_window_destroy, this,
                                          res->GetUIApplication()));
    m_window->connect("lbutton_down",
                      ui::Slot(&MainWindow::on_lbutton_down, this));
  }

  void create_ui() {
    ui::SvgPtr svg(m_window->GetResource());
    svg->AddAttribute(XML_LAYOUT_ITEM_LEFT, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_TOP, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_RIGHT, "0");
    svg->AddAttribute(XML_LAYOUT_ITEM_BOTTOM, "0");
    svg->InitDefaultAttrib();
    m_svg = static_cast<ui::ISvg *>(m_window->AddChild(svg.release()));
  }

  void load_svg_from_file(const char *path) {
    printf("load: %s\n", path);

    std::fstream file;
    file.open(path, ios_base::in);
    if (!file.good()) {
      return;
    }
    file.seekg(0, ios_base::end);
    size_t length = file.tellg();
    if (length == 0) {
      return;
    }
    char *data = new char[length + 1];
    file.seekg(ios_base::beg);
    file.read(data, length);

    m_svg->Load(data);
    m_window->SetTitle(path);

    delete[] data;
    data = nullptr;
  }
  void load_next() {
    ui::Path svg_path = ui::Path::ExecutePath()
                            .Dir()
                            .Join("sample/svg")
                            .Join(samples[samples_index % samples_count]);
    samples_index++;
    load_svg_from_file(svg_path.ToString());
  }

  void on_window_destroy(ui::IApplication *uiapp, ui::Event *event) {
    printf("on_window_destroy\n");
    uiapp->Quit();
  }
  void on_lbutton_down(ui::Event *event) { load_next(); }

private:
  ui::WindowPtr m_window;
  ui::ISvg *m_svg;
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