#include "html/css/property/property_id.h"
#include <cassert>
#include <string>
#include <tuple>

void test1_perfect_hash_function() {
  // 完美Hash验证
  {
    std::tuple<std::string, html::CSSPropertyId> data[] = {
      {"background", html::CSSPropertyId::Background},
      {"background-image", html::CSSPropertyId::BackgroundImage},
      {"background-position-x", html::CSSPropertyId::BackgroundPositionX},
      {"not_exist", html::CSSPropertyId::Invalid},
    };
    for (auto& [name, id] : data) {
      auto proerty_id = html::CSSPropertyNameToIdByHash(name.c_str(), name.length());
      assert(proerty_id == id);
    }
  }
}

void test_html_css_parser() {
  test1_perfect_hash_function();
}