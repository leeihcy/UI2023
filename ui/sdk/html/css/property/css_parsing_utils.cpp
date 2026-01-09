#include "html/css/property/css_parsing_utils.h"


#include "html/base/casting.h"
#include "html/base/memory.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/parser/css_parser.h"
#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"
#include "html/css/property/css_property.h"
#include "html/css/property/value_id.h"
#include "html/css/graphics/color.h"
#include "html/util/util.h"

#include <algorithm>
#include <cassert>
#include <array>
#include <type_traits>


namespace html {
namespace css_parsing_utils {

bool IsCSSWideKeyword(CSSValueId id) {
  return id == CSSValueId::Inherit || id == CSSValueId::Initial ||
         id == CSSValueId::Unset || id == CSSValueId::Revert ||
         id == CSSValueId::RevertLayer;
}

U<CSSValue> ConsumeCSSWideKeyword(CSSParserTokenStream& stream) {
  if (!IsCSSWideKeyword(stream.Peek().ValueId())) {
    return nullptr;
  }
  switch (stream.ConsumeIncludingWhitespace().ValueId()) {
    case CSSValueId::Initial:
      return CSSInitialValue::Create();
    case CSSValueId::Inherit:
      return CSSInheritedValue::Create();
    case CSSValueId::Unset:
      return CSSUnsetValue::Create();
    case CSSValueId::Revert:
      return CSSRevertValue::Create();
    case CSSValueId::RevertLayer:
      return CSSRevertLayerValue::Create();
    // case CSSValueId::RevertRule: {
    //   if (!RuntimeEnabledFeatures::CSSRevertRuleEnabled()) {
    //     return nullptr;
    //   }
    //   return cssvalue::CSSRevertRuleValue::Create();
    // }
    default:
      assert(false);
      abort();
  }
}

bool MaybeConsumeImportant(CSSParserTokenStream& stream,
                           bool allow_important_annotation) {
  stream.ConsumeWhitespace();
  if (stream.AtEnd() || !allow_important_annotation) {
    return false;
  }

  CSSParserSavePoint savepoint(stream);

  // !
  if (stream.Peek().Type() != CSSParserTokenType::Delimiter ||
      stream.Peek().Delimiter() != '!') {
    return false;
  }
  stream.ConsumeIncludingWhitespace();

  // important
  if (stream.Peek().Type() != CSSParserTokenType::Ident ||
      !EqualIgnoringASCIICase(stream.Peek().Name().c_str(), u"important")) {
    return false;
  }
  stream.ConsumeIncludingWhitespace();

  savepoint.Release();
  return true;
}

U<CSSValue> ParseLonghand(CSSPropertyId property_id, CSSParserContext& context) {
  // CSSValueId value_id = stream.Peek().ValueId();

  const CSSProperty& property = CSSProperty::Get(property_id);
  return static_cast<const Longhand&>(property).ParseSingleValue(context);
}

U<CSSIdentifierValue> ConsumeIdent(CSSParserTokenStream& stream) {
  if (stream.Peek().Type() != CSSParserTokenType::Ident) {
    return nullptr;
  }
  return CSSIdentifierValue::Create(stream.ConsumeIncludingWhitespace().ValueId());
}

static bool ParseHexColor(CSSParserTokenStream& stream,
                          Color& result) {
  const CSSParserToken& token = stream.Peek();
  if (token.Type() == CSSParserTokenType::Hash) {
    if (!Color::ParseHexColor(token.Name(), result)) {
      return false;
    }
  } else {
    // quirky mode color 不处理，例如#f0f0, #red
    return false;
  }
  stream.ConsumeIncludingWhitespace();
  return true;
}

U<CSSValue> ConsumeColor(CSSParserTokenStream &stream) {

  if (stream.Peek().FunctionId() == CSSValueId::ColorMix) {
    // CSSValue* color =
    //     ConsumeColorMixFunction(stream);
    // return color;
    assert(false); 
    return nullptr;
  }

  CSSValueId id = stream.Peek().ValueId();
  if (id >= CSSValueId::ColorBasicStart && id <= CSSValueId::ColorExtendedEnd) {
    // if (!IsValueAllowedInMode(id, context.Mode())) {
    //   return nullptr;
    // }
    // if (color_parser_context.OnlyAbsoluteColorsAllowed() &&
    //     (id == CSSValueID::kCurrentcolor ||
    //      StyleColor::IsSystemColorIncludingDeprecated(id) ||
    //      StyleColor::IsSystemColor(id))) {
    //   return nullptr;
    // }
    U<CSSIdentifierValue> color = ConsumeIdent(stream);
    return color;
  }

  Color color = Color::kTransparent;
  if (ParseHexColor(stream, color)) {
    return CSSColorValue::Create(color);
  }

  // Parses the color inputs rgb(), rgba(), hsl(), hsla(), hwb(), lab(),
  // oklab(), lch(), oklch() and color(). https://www.w3.org/TR/css-color-4/
  // ColorFunctionParser parser;
  // if (CSSValue* functional_syntax_color = parser.ConsumeFunctionalSyntaxColor(
  //         stream, context, color_parser_context)) {
  //   return functional_syntax_color;
  // }

  // if (!color_parser_context.OnlyAbsoluteColorsAllowed()) {
  //   return ConsumeLightDark(ConsumeColor, stream, context,
  //                           color_parser_context);
  // }
  return nullptr;
}

bool ConsumeCommaIncludingWhitespace(CSSParserTokenStream& stream) {
  CSSParserToken value = stream.Peek();
  if (value.Type() != CSSParserTokenType::Comma) {
    return false;
  }
  stream.ConsumeIncludingWhitespace();
  return true;
}

U<CSSPrimitiveValue> ConsumeLength(CSSParserContext &context,
                                 CSSPrimitiveValue::ValueRange value_range) {
  const CSSParserToken token = context.token_stream.Peek();
  if (token.GetType() == CSSParserTokenType::Dimension) {
    switch (token.GetUnitType()) {
    // case CSSPrimitiveValue::UnitType::kQuirkyEms:
    //   if (context.Mode() != kUASheetMode) {
    //     return nullptr;
    //   }
    case CSSPrimitiveValue::UnitType::kPixels:
      break;
    default:
      return nullptr;
    }

    if (value_range == CSSPrimitiveValue::ValueRange::kNonNegative &&
        token.NumericValue() < 0) {
      return nullptr;
    }
    return CSSNumericLiteralValue::Create(
        context.token_stream.ConsumeIncludingWhitespace().NumericValue(),
        token.GetUnitType());
  }
  if (token.GetType() == CSSParserTokenType::Number) {
     if (value_range == CSSPrimitiveValue::ValueRange::kNonNegative &&
        token.NumericValue() < 0) {
      return nullptr;
    }
    return CSSNumericLiteralValue::Create(
      context.token_stream.ConsumeIncludingWhitespace().NumericValue(),
      CSSPrimitiveValue::UnitType::kPixels);
  }

  // TODO: 
  assert(false);
  return nullptr;
}


U<CSSPrimitiveValue> ConsumePercent(CSSParserContext &context,
                                 CSSPrimitiveValue::ValueRange value_range) {
  const CSSParserToken token = context.token_stream.Peek();
  if (token.GetType() == CSSParserTokenType::Percentage) {
    if (value_range == CSSPrimitiveValue::ValueRange::kNonNegative &&
        token.NumericValue() < 0) {
      return nullptr;
    }
    return CSSNumericLiteralValue::Create(
        context.token_stream.ConsumeIncludingWhitespace().NumericValue(),
        CSSPrimitiveValue::UnitType::kPercentage);
  }
  
  // CSSMathFunctionValue
  assert(false);
  return nullptr;
}

U<CSSPrimitiveValue> ConsumeLengthOrPercent(
    CSSParserContext& context,
    CSSPrimitiveValue::ValueRange value_range
    // CSSAnchorQueryTypes allowed_anchor_queries,
    // AllowCalcSize allow_calc_size
  ) {
  // using enum CSSMathExpressionNode::Flag;
  // using Flags = CSSMathExpressionNode::Flags;

  const CSSParserToken& token = context.token_stream.Peek();
  if (token.GetType() == CSSParserTokenType::Dimension || token.GetType() == CSSParserTokenType::Number) {
    return ConsumeLength(context, value_range);
  }

  if (token.GetType() == CSSParserTokenType::Percentage) {
    return ConsumePercent(context, value_range);
  }
#if 0
  Flags parsing_flags({AllowPercent});
  switch (allow_calc_size) {
    case AllowCalcSize::kAllowWithAutoAndContent:
      parsing_flags.Put(AllowContentInCalcSize);
      [[fallthrough]];
    case AllowCalcSize::kAllowWithAuto:
      parsing_flags.Put(AllowAutoInCalcSize);
      [[fallthrough]];
    case AllowCalcSize::kAllowWithoutAuto:
      parsing_flags.Put(AllowCalcSize);
      [[fallthrough]];
    case AllowCalcSize::kForbid:
      break;
  }
  MathFunctionParser math_parser(stream, context, value_range, parsing_flags,
                                 allowed_anchor_queries);
  if (const CSSMathFunctionValue* calculation = math_parser.Value()) {
    if (CanConsumeCalcValue(calculation->Category(), context.Mode())) {
      return math_parser.ConsumeValue();
    }
  }
#endif
  return nullptr;
}


//
// background-position: top center 
// background-position: left 10px top 15px;
// background-position: right 3em bottom 10px
// background-position: 75% 50%
//
static U<CSSValue> ConsumePositionComponent(CSSParserContext& context,
                                          bool& horizontal_edge,
                                          bool& vertical_edge) {
  if (context.token_stream.Peek().Type() != CSSParserTokenType::Ident) {
    return ConsumeLengthOrPercent(
        context, CSSPrimitiveValue::ValueRange::kAll);
  }

  CSSValueId id = context.token_stream.Peek().ValueId();
  if (id == CSSValueId::Left || id == CSSValueId::Right) {
    if (horizontal_edge) {
      return nullptr;
    }
    horizontal_edge = true;
  } else if (id == CSSValueId::Top || id == CSSValueId::Bottom) {
    if (vertical_edge) {
      return nullptr;
    }
    vertical_edge = true;
  } else if (id != CSSValueId::Center) {
    return nullptr;
  }
  return ConsumeIdent(context.token_stream);
}

static bool IsHorizontalPositionKeywordOnly(const CSSValue* value) {
  if (!value) {
    return false;
  }
  const CSSIdentifierValue* identifier_value = DynamicTo<CSSIdentifierValue>(value);
  if (!identifier_value) {
    return false;
  }
  CSSValueId value_id = identifier_value->GetValueId();
  return value_id == CSSValueId::Left || value_id == CSSValueId::Right;
}
static bool IsVerticalPositionKeywordOnly(const CSSValue* value) {
  if (!value) {
    return false;
  }
  const CSSIdentifierValue* identifier_value = DynamicTo<CSSIdentifierValue>(value);
  if (!identifier_value) {
    return false;
  }
  CSSValueId value_id = identifier_value->GetValueId();
  return value_id == CSSValueId::Top || value_id == CSSValueId::Bottom;
}

static void PositionFromOneValue(U<CSSValue>&& value, U<CSSValue>& result_x,
                     U<CSSValue>& result_y) {
  bool is_vertical = IsVerticalPositionKeywordOnly(value.get());
  if (is_vertical) {
    result_y.reset(std::move(value));
    result_x.reset(CSSIdentifierValue::Create(CSSValueId::Center));
  }
  else {
    result_x.reset(std::move(value));
    result_y.reset(CSSIdentifierValue::Create(CSSValueId::Center));
  }
}

static void PositionFromTwoValues(U<CSSValue> &&value1, U<CSSValue> &&value2,
                                 U<CSSValue> &result_x, U<CSSValue> &result_y) {
  bool must_order_as_xy = IsHorizontalPositionKeywordOnly(value1.get()) ||
                          IsVerticalPositionKeywordOnly(value2.get()) ||
                          !value1->IsIdentifierValue() ||
                          !value2->IsIdentifierValue();
  bool must_order_as_yx = IsVerticalPositionKeywordOnly(value1.get()) ||
                          IsHorizontalPositionKeywordOnly(value2.get());
  assert(!must_order_as_xy || !must_order_as_yx);

  if (must_order_as_yx) {
    result_y.reset(std::move(value1));
    result_x.reset(std::move(value2));
  } else {
    result_x.reset(std::move(value1));
    result_y.reset(std::move(value2));
  }
}

static void PositionFromThreeOrFourValues(std::array<U<CSSValue>, 5> &values,
                                          U<CSSValue> &result_x,
                                          U<CSSValue> &result_y) {
  U<CSSValue> center = nullptr;
  for (int i = 0; values[i]; i++) {
    // 非ident类型的索引在下面将被跳过。
    auto* current_value = static_cast<CSSIdentifierValue*>(values[i].get());

    CSSValueId id = current_value->GetValueId();
    if (id == CSSValueId::Center) {
      center.reset(std::move(values[i]));
      continue;
    }

    U<CSSValue> result = nullptr;
    if (values[i + 1] && !values[i + 1]->IsIdentifierValue()) {
      int first = i;
      int second = ++i;
      U<CSSValuePair> pair = U<CSSValuePair>::make_new(std::move(values[first]), std::move(values[second]),
          CSSValuePair::IdenticalValuesPolicy::Keep);

      result.reset(std::move(pair));
    } else {
      result.reset(std::move(values[i]));
    }

    if (id == CSSValueId::Left || id == CSSValueId::Right) {
      result_x.reset(std::move(result));
    } else {
      result_y.reset(std::move(result));
    }
  }
  if (center) {
    if (!result_x) {
      result_x.reset(std::move(center));
    } else {
      result_y.reset(std::move(center));
    }
  }
}

//
// https://www.w3.org/TR/css-values-5/#typedef-position
//
// <position> = <position-one> | <position-tow> | <position-four>
// 默认 left top 对齐。如果只指定了一个值，则另一个值将认为是 center
// 示例：
// left 10px top 15 px;  // 10px, 15px
// left top;    // 0px, 0px
// left 15px;   // 0px, 15px
// 10px top;    // 10px, 0px
//
bool ConsumePosition(CSSParserContext& context,
                     U<CSSValue>& result_x,
                     U<CSSValue>& result_y) {
  auto& stream = context.token_stream;
  
  bool horizontal_edge = false;
  bool vertical_edge = false;
  U<CSSValue> value1 = ConsumePositionComponent(context,
                                              horizontal_edge, vertical_edge);
  if (!value1) {
    return false;
  }
  if (!value1->IsIdentifierValue()) {
    horizontal_edge = true;
  }

  CSSParserTokenStream::State savepoint_after_first_consume = stream.Save();
  U<CSSValue> value2 = ConsumePositionComponent(context,
                                              horizontal_edge, vertical_edge);

  // 只有一个值的场景，将另一个值设置为Center
  if (!value2) {
    PositionFromOneValue(std::move(value1), result_x, result_y);
    return true;
  }

  CSSParserTokenStream::State savepoint_after_second_consume = stream.Save();
  U<CSSValue> value3 = nullptr;
  auto* identifier_value1 = DynamicTo<CSSIdentifierValue>(value1.get());
  auto* identifier_value2 = DynamicTo<CSSIdentifierValue>(value2.get());

  // 第一个是关键字，第二个是数字，下一个如果是关键字，则尝试读取第三个字段
  if (identifier_value1 &&
      !!identifier_value2 != (stream.Peek().GetType() ==
                              CSSParserTokenType::Ident) &&
      (identifier_value2
           ? identifier_value2->GetValueId()
           : identifier_value1->GetValueId()) != CSSValueId::Center) {
    value3.reset(
        ConsumePositionComponent(context, horizontal_edge, vertical_edge));
  }

  if (!value3) {
    if (vertical_edge && !value2->IsIdentifierValue()) {
      stream.Restore(savepoint_after_first_consume);
      PositionFromOneValue(std::move(value1), result_x, result_y);
      return true;
    }
    PositionFromTwoValues(std::move(value1), std::move(value2), result_x, result_y);
    return true;
  }

  U<CSSValue> value4 = nullptr;
  auto* identifier_value3 = DynamicTo<CSSIdentifierValue>(value3.get());
  if (identifier_value3 &&
      identifier_value3->GetValueId() != CSSValueId::Center &&
      stream.Peek().GetType() != CSSParserTokenType::Ident) {
    value4.reset(ConsumePositionComponent(context,
                                      horizontal_edge, vertical_edge));
  }

  // std::optional<WebFeature> three_value_position
  bool three_value_position = true;

  if (!value4) {
    if (!three_value_position) {
      // [top | bottom] <length-percentage> is not permitted
      if (vertical_edge && !value2->IsIdentifierValue()) {
        stream.Restore(savepoint_after_first_consume);
        PositionFromOneValue(std::move(value1), result_x, result_y);
        return true;
      }
      stream.Restore(savepoint_after_second_consume);
      PositionFromTwoValues(std::move(value1), std::move(value2), result_x, result_y);
      return true;
    }
    // DCHECK_EQ(*three_value_position,
    //           WebFeature::kThreeValuedPositionBackground);
    // context.Count(*three_value_position);
  }

  std::array<U<CSSValue>, 5> values = {std::move(value1), std::move(value2),
                                       std::move(value3), std::move(value4),
                                       nullptr};
  PositionFromThreeOrFourValues(values, result_x, result_y);

  return true;
}

}
}