#include "html/core_initializer.h"
#include "html/css/css_default_style_sheets.h"
#include "html/css/css_default_style_sheets.h"

namespace html {

// TODO: blink中，派生了ModulesInitializer。不是直接创建CoreInitializer
// static 
CoreInitializer& CoreInitializer::GetInstance() {
  static CoreInitializer s;
  return s;
}

void CoreInitializer::Initialize() {
  
  // ...

  // html_names::Init();
  // mathml_names::Init();
  // svg_names::Init();
  // xlink_names::Init();
  // xml_names::Init();
  // xmlns_names::Init();

  CSSDefaultStyleSheets::Init();
}

}