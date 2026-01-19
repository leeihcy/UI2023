#include "html/base/atomic_string.h"
#include "html/css/css_selector.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"
#include "html/css/property/css_parsing_utils.h"
#include "html/css/parser/css_parser.h"
#include "html/css/parser/css_selector_parser.h"
#include "html/css/property/css_property.h"
#include "html/base/casting.h"
#include "html/base/memory.h"
#include "html/css/property/value_id.h"

#include <cassert>
#include <cstddef>
#include <string>
#include <tuple>

using html::A;

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
   {  
    std::string css = "background: red;";
    html::CSSParser parser;
    std::unique_ptr<html::CSSPropertyValueSet> result = 
      parser.ParseInlineStyleDeclaration(css.data(), css.length());
    assert(result);
    assert(result->Size() == 9);
  }
  {  
    std::string css = "background: url(aaa.png), red;";
    html::CSSParser parser;
    std::unique_ptr<html::CSSPropertyValueSet> result = 
      parser.ParseInlineStyleDeclaration(css.data(), css.length());
    assert(result);
    assert(result->Size() == 9);
  }
  {  
    std::string css = "background: url(\"aaa.png\"), url(\"bbb.png\")";
    html::CSSParser parser;
    std::unique_ptr<html::CSSPropertyValueSet> result = 
      parser.ParseInlineStyleDeclaration(css.data(), css.length());
    assert(result);
    assert(result->Size() == 9);
  }
}

void test5_ConsumeLengthOrPercent() {
  std::tuple<std::string, double> data[] = 
  { 
    {"15px", 15},
    {"0.3%", 0.3},
    {"1000", 1000},
  };

  for (auto& [css, d] : data) {
    html::CSSParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    A<html::CSSPrimitiveValue> value = html::css_parsing_utils::ConsumeLengthOrPercent(
      context, html::CSSPrimitiveValue::ValueRange::kAll);
    assert(value);
    
    html::CSSNumericLiteralValue* numeric = 
      html::DynamicTo<html::CSSNumericLiteralValue>(value.get());
    assert(numeric);
    assert(abs(numeric->GetNum() - d) < 0.000001);
  }
}

void test6_ConsumePosition() {
  {
    std::string css = "left 10px top 15px";
    html::CSSParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    A<html::CSSValue> x = nullptr;
    A<html::CSSValue> y = nullptr;
    html::css_parsing_utils::ConsumePosition(context, x, y);

    assert(x && y);
    html::CSSValuePair* x_pair = html::DynamicTo<html::CSSValuePair>(x.get());
    html::CSSValuePair* y_pair = html::DynamicTo<html::CSSValuePair>(y.get());
    assert(x_pair && y_pair);
    assert(x_pair->First()->IsIdentifierValue());
    assert(x_pair->Second()->IsNumericLiteralValue());
    assert(y_pair->First()->IsIdentifierValue());
    assert(y_pair->Second()->IsNumericLiteralValue());
    assert(html::DynamicTo<html::CSSIdentifierValue>(x_pair->First())->GetValueId() == html::CSSValueId::Left);
    assert(html::DynamicTo<html::CSSIdentifierValue>(y_pair->First())->GetValueId() == html::CSSValueId::Top);
    assert(html::DynamicTo<html::CSSNumericLiteralValue>(x_pair->Second())->GetNum() == 10);
    assert(html::DynamicTo<html::CSSNumericLiteralValue>(y_pair->Second())->GetNum() == 15);
  }

  {
    std::string css = "left top";
    html::CSSParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    A<html::CSSValue> x = nullptr;
    A<html::CSSValue> y = nullptr;
    html::css_parsing_utils::ConsumePosition(context, x, y);

    assert(x && y);
    html::CSSIdentifierValue* x_ident = html::DynamicTo<html::CSSIdentifierValue>(x.get());
    html::CSSIdentifierValue* y_ident = html::DynamicTo<html::CSSIdentifierValue>(y.get());
    assert(x_ident && y_ident);
    assert(x_ident->GetValueId() == html::CSSValueId::Left);
    assert(y_ident->GetValueId() == html::CSSValueId::Top);
  }

  {
    std::tuple<std::string, html::CSSValueId, html::CSSValueId> datas[] = {
      {"top", html::CSSValueId::Center, html::CSSValueId::Top }, 
      {"left", html::CSSValueId::Left, html::CSSValueId::Center },
      {"right", html::CSSValueId::Right, html::CSSValueId::Center }, 
      {"bottom", html::CSSValueId::Center, html::CSSValueId::Bottom}
    };
    for (auto& [css, x_id, y_id] : datas) {
      html::CSSParserContext context;
      context.token_stream.SetInput(css.c_str(), css.length());

      A<html::CSSValue> x = nullptr;
      A<html::CSSValue> y = nullptr;
      html::css_parsing_utils::ConsumePosition(context, x, y);
      assert(x && y);
      html::CSSIdentifierValue* x_ident = html::DynamicTo<html::CSSIdentifierValue>(x.get());
      html::CSSIdentifierValue* y_ident = html::DynamicTo<html::CSSIdentifierValue>(y.get());
      assert(x_ident && y_ident);
      assert(x_ident->GetValueId() == x_id);
      assert(y_ident->GetValueId() == y_id);
    }
  }

  {
    std::string css = "75% 10px";
    html::CSSParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    A<html::CSSValue> x = nullptr;
    A<html::CSSValue> y = nullptr;
    html::css_parsing_utils::ConsumePosition(context, x, y);

    assert(x && y);
    html::CSSNumericLiteralValue* x_num = html::DynamicTo<html::CSSNumericLiteralValue>(x.get());
    html::CSSNumericLiteralValue* y_num = html::DynamicTo<html::CSSNumericLiteralValue>(y.get());
    assert(x_num && y_num);
    assert(x_num->GetNum() == 75);
    assert(y_num->GetNum() == 10);
    assert(x_num->GetUnitType() == html::CSSPrimitiveValue::UnitType::kPercentage);
    assert(y_num->GetUnitType() == html::CSSPrimitiveValue::UnitType::kPixels);
  }
}

void test7_parse_selector() {
  // std::string css = "namespace1|button, #id, .class, tag[foo=\"bar\"]";
  // std::string css = "button, #id, .class, tag[foo=\"bar\"]";
  // std::string css = "tag[foo=\"bar\"]";
  {
    // 当有定义@namespace "http..."时，匹配这个默认命名空间下的 <img> 元素（HTML 命名空间）
    // 当没有定义默认@namespace时，会匹配所有命令空间，即*|img
    std::string css("img");
    html::CSSSelectorParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    std::vector<html::CSSSelector> result;
    html::CSSSelectorParser::ConsumeSelector(context, nullptr, result);
    assert(result.size() == 1);
    assert(result[0].GetQualifiedName().LocalName() == u"img");
    assert(result[0].GetQualifiedName().NamespaceUri() == html::g_star_atom);
    assert(result[0].GetQualifiedName().Prefix() == html::g_null_atom);
  }
  {
    std::string css("#id");
    html::CSSSelectorParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    std::vector<html::CSSSelector> result;
    html::CSSSelectorParser::ConsumeSelector(context, nullptr, result);
    assert(result.size() == 1);
    assert(result[0].GetMatchType() == html::CSSSelector::Id);
    assert(result[0].GetValue() == u"id");
  }
  {
    std::string css(".class");
    html::CSSSelectorParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    std::vector<html::CSSSelector> result;
    html::CSSSelectorParser::ConsumeSelector(context, nullptr, result);
    assert(result.size() == 1);
    assert(result[0].GetMatchType() == html::CSSSelector::Class);
    assert(result[0].GetValue() == u"class");
  }
  {
    // 匹配没有命名空间的 <img> 元素
    std::string css("|img");
    html::CSSSelectorParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    std::vector<html::CSSSelector> result;
    html::CSSSelectorParser::ConsumeSelector(context, nullptr, result);
    assert(result.size() == 1);
    assert(result[0].GetQualifiedName().LocalName()==u"img");
    assert(result[0].GetQualifiedName().NamespaceUri() == html::g_empty_atom);
    assert(result[0].GetQualifiedName().Prefix() == html::g_empty_atom);
  }
  {
    // 匹配任何命名空间下的 <img> 元素
    std::string css("*|img");
    html::CSSSelectorParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    std::vector<html::CSSSelector> result;
    html::CSSSelectorParser::ConsumeSelector(context, nullptr, result);
    assert(result.size() == 1);
    assert(result[0].GetQualifiedName().LocalName()==u"img");
    assert(result[0].GetQualifiedName().NamespaceUri() == html::g_star_atom);
    assert(result[0].GetQualifiedName().Prefix() == html::g_null_atom);
  }
  {
    // selector list
    std::string css("html, head, body");
    html::CSSSelectorParserContext context;
    context.token_stream.SetInput(css.c_str(), css.length());

    std::vector<html::CSSSelector> result;
    html::CSSSelectorParser::ConsumeSelector(context, nullptr, result);
    assert(result.size() == 3);
    assert(result[0].GetQualifiedName().LocalName()==u"html");
    assert(result[1].GetQualifiedName().LocalName()==u"head");
    assert(result[2].GetQualifiedName().LocalName()==u"body");
  }
}

void test8_parse_sheet() {
#if 0
  auto* context = MakeGarbageCollected<CSSParserContext>(GetDocument());
  auto* style_sheet = MakeGarbageCollected<StyleSheetContents>(context);

  String sheet_text = R"CSS(
    #first {
      color: red;
      --x:foo;
      --y:foo;
    }
    #second {
      color: green;
      --x:bar;
      --y:bar;
    }
  )CSS";

  CSSParser::ParseSheet(context, style_sheet, sheet_text,
                        CSSDeferPropertyParsing::kNo);
  StyleRule* rule0 = RuleAt(style_sheet, 0);
  StyleRule* rule1 = RuleAt(style_sheet, 1);
  MutableCSSPropertyValueSet& set0 = rule0->MutableProperties();
  MutableCSSPropertyValueSet& set1 = rule1->MutableProperties();

  EXPECT_EQ(3u, set0.PropertyCount());
  EXPECT_EQ("red", set0.GetPropertyValue(CSSPropertyID::kColor));
  EXPECT_EQ("foo", set0.GetPropertyValue(AtomicString("--x")));
  EXPECT_EQ("foo", set0.GetPropertyValue(AtomicString("--y")));
  EXPECT_EQ(3u, set1.PropertyCount());
  EXPECT_EQ("green", set1.GetPropertyValue(CSSPropertyID::kColor));
  EXPECT_EQ("bar", set1.GetPropertyValue(AtomicString("--x")));
  EXPECT_EQ("bar", set1.GetPropertyValue(AtomicString("--y")));

  set0.MergeAndOverrideOnConflict(&set1);

  EXPECT_EQ(3u, set0.PropertyCount());
  EXPECT_EQ("green", set0.GetPropertyValue(CSSPropertyID::kColor));
  EXPECT_EQ("bar", set0.GetPropertyValue(AtomicString("--x")));
  EXPECT_EQ("bar", set0.GetPropertyValue(AtomicString("--y")));
  EXPECT_EQ(3u, set1.PropertyCount());
  EXPECT_EQ("green", set1.GetPropertyValue(CSSPropertyID::kColor));
  EXPECT_EQ("bar", set1.GetPropertyValue(AtomicString("--x")));
  EXPECT_EQ("bar", set1.GetPropertyValue(AtomicString("--y")));
#endif
}

}

void test_html_css_parser() {
  // test1_perfect_hash_function();
  // test2_parse_property_keyword();
  // test3_ParseInlineStyleDeclaration();
  // test4_parse_shorthand();
  // test5_ConsumeLengthOrPercent();
  // test6_ConsumePosition();
  test7_parse_selector();
  // test8_parse_sheet();
}