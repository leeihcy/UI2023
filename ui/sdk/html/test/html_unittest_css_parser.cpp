#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"
#include "html/css/property/css_parsing_utils.h"
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
  {
    std::tuple<std::string, html::CSSValueClassType> datas[] = {
      { "background-color:inherit", html::CSSValueClassType::Inherited },
      { "background-color : unset", html::CSSValueClassType::Unset },
      { "background-color:initial ",html::CSSValueClassType::Initial },
      { " background-color: revert ",html::CSSValueClassType::Revert },
    };
    for (auto& [css, class_type] : datas) {
      html::CSSParser parser;
      auto result = parser.ParseInlineStyleDeclaration(css.c_str(), css.length());
      assert(result->Size() == 1);
      assert(result->Item(0).Id() == html::CSSPropertyId::BackgroundColor);
      assert(result->Item(0).Value()->GetClassType() == class_type);
    }
  }
}
void test3_ParseInlineStyleDeclaration() {
  std::string css = " background-color : red ; background-color : #FFF ! important ;";
  // std::string css = " background-color : #FFF ! important ;";
  html::CSSParser parser;
  std::unique_ptr<html::CSSPropertyValueSet> result = 
    parser.ParseInlineStyleDeclaration(css.data(), css.length());
  assert(result);
  assert(result->Size() == 2);

  assert(result->Item(0).Id() == html::CSSPropertyId::BackgroundColor);
  assert(result->Item(0).Value()->GetClassType() == html::CSSValueClassType::Identifier);
  assert(result->Item(0).IsImportant() == false);

  assert(result->Item(1).Id() == html::CSSPropertyId::BackgroundColor);
  assert(result->Item(1).Value()->GetClassType() == html::CSSValueClassType::Color);
  const html::CSSColorValue* color_value = static_cast<const html::CSSColorValue*>(result->Item(1).Value());
  assert(color_value->GetColor() == html::Color::FromRGBA32(0xFFFFFFFF));
  assert(result->Item(1).IsImportant() == true);
}

void test4_parse_shorthand() {
  std::string css = "background: url(aaa.png), red;";
  html::CSSParser parser;
  std::unique_ptr<html::CSSPropertyValueSet> result = 
    parser.ParseInlineStyleDeclaration(css.data(), css.length());
  assert(result);
  assert(result->Size() == 1);
}

void test5_ConsumeLengthOrPercent() {
  std::tuple<std::string> data[] = 
  { 
    "15px" 
  };

  for (auto& [css] : data) {
    html::CSSParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    html::U<html::CSSPrimitiveValue> value = html::css_parsing_utils::ConsumeLengthOrPercent(
      context, html::CSSPrimitiveValue::ValueRange::kAll);
    assert(value);
  }
}

}

void test_html_css_parser() {
  // test1_perfect_hash_function();
  // test2_parse_property_keyword();
  // test3_ParseInlineStyleDeclaration();
  // test4_parse_shorthand();
  // test5_ConsumeLengthOrPercent();
}