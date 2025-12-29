#include "html/css/property/property_id.h"
#include "html/css/parser/css_parser.h"
#include <cassert>
#include <string>
#include <tuple>

namespace {

void test1_perfect_hash_function() {
  // 完美Hash验证
  {
    std::tuple<std::string, html::CSSPropertyId> data[] = {
      {"background", html::CSSPropertyId::Background},
      // {"background-image", html::CSSPropertyId::BackgroundImage},
      // {"background-position-x", html::CSSPropertyId::BackgroundPositionX},
      {"not_exist", html::CSSPropertyId::Invalid},
    };
    for (auto& [name, id] : data) {
      auto proerty_id = html::CSSPropertyNameToIdByHash(name.c_str(), name.length());
      assert(proerty_id == id);
    }
  }
}


void test2_parse_property_keyword() {
  // std::string css = "background-color:blue";
  std::string css = "background-color:inherit";
  // std::string css = "background-color:unset";
  // std::string css = "background-color:initial";
  html::CSSParser parser;
  parser.ParseInlineStyleDeclaration(css.c_str(), css.length());
}

}

void test_html_css_parser() {
  // test1_perfect_hash_function();
  test2_parse_property_keyword();
}