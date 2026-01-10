#include "html/css/property/css_parsing_utils.h"


#include "html/base/casting.h"
#include "html/base/memory.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token.h"
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
#include <cstddef>
#include <string>
#include <type_traits>


namespace html {
namespace css_parsing_utils {

bool IsCSSWideKeyword(CSSValueId id) {
  return id == CSSValueId::Inherit || id == CSSValueId::Initial ||
         id == CSSValueId::Unset || id == CSSValueId::Revert ||
         id == CSSValueId::RevertLayer;
}

A<CSSValue> ConsumeCSSWideKeyword(CSSParserTokenStream& stream) {
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

A<CSSValue> ParseLonghand(CSSPropertyId property_id, CSSParserContext& context) {
  // CSSValueId value_id = stream.Peek().ValueId();

  const CSSProperty& property = CSSProperty::Get(property_id);
  return static_cast<const Longhand&>(property).ParseSingleValue(context);
}

A<CSSIdentifierValue> ConsumeIdent(CSSParserTokenStream& stream) {
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

A<CSSValue> ConsumeColor(CSSParserTokenStream &stream) {

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
    A<CSSIdentifierValue> color = ConsumeIdent(stream);
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

bool ConsumeSlashIncludingWhitespace(CSSParserTokenStream& stream) {
  CSSParserToken value = stream.Peek();
  if (value.Type() != CSSParserTokenType::Delimiter || value.Delimiter() != '/') {
    return false;
  }
  stream.ConsumeIncludingWhitespace();
  return true;
}

A<CSSValue> GetSingleValueOrMakeList(
    CSSValueListSeparator list_separator,
    std::vector< A<CSSValue> >&& values) {
  if (values.size() == 1u) {
    return A<CSSValue>(std::move(values.front()));
  }
  return A<CSSValueList>::make_new(std::move(values), list_separator);
}

A<CSSPrimitiveValue> ConsumeLength(CSSParserContext &context,
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


A<CSSPrimitiveValue> ConsumePercent(CSSParserContext &context,
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

A<CSSPrimitiveValue> ConsumeLengthOrPercent(
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
static A<CSSValue> ConsumePositionComponent(CSSParserContext& context,
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

static void PositionFromOneValue(A<CSSValue>&& value, A<CSSValue>& result_x,
                     A<CSSValue>& result_y) {
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

static void PositionFromTwoValues(A<CSSValue> &&value1, A<CSSValue> &&value2,
                                 A<CSSValue> &result_x, A<CSSValue> &result_y) {
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

static void PositionFromThreeOrFourValues(std::array<A<CSSValue>, 5> &values,
                                          A<CSSValue> &result_x,
                                          A<CSSValue> &result_y) {
  A<CSSValue> center = nullptr;
  for (int i = 0; values[i]; i++) {
    // 非ident类型的索引在下面将被跳过。
    auto* current_value = static_cast<CSSIdentifierValue*>(values[i].get());

    CSSValueId id = current_value->GetValueId();
    if (id == CSSValueId::Center) {
      center.reset(std::move(values[i]));
      continue;
    }

    A<CSSValue> result = nullptr;
    if (values[i + 1] && !values[i + 1]->IsIdentifierValue()) {
      int first = i;
      int second = ++i;
      A<CSSValuePair> pair = A<CSSValuePair>::make_new(std::move(values[first]), std::move(values[second]),
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
                     A<CSSValue>& result_x,
                     A<CSSValue>& result_y) {
  auto& stream = context.token_stream;
  
  bool horizontal_edge = false;
  bool vertical_edge = false;
  A<CSSValue> value1 = ConsumePositionComponent(context,
                                              horizontal_edge, vertical_edge);
  if (!value1) {
    return false;
  }
  if (!value1->IsIdentifierValue()) {
    horizontal_edge = true;
  }

  CSSParserTokenStream::State savepoint_after_first_consume = stream.Save();
  A<CSSValue> value2 = ConsumePositionComponent(context,
                                              horizontal_edge, vertical_edge);

  // 只有一个值的场景，将另一个值设置为Center
  if (!value2) {
    PositionFromOneValue(std::move(value1), result_x, result_y);
    return true;
  }

  CSSParserTokenStream::State savepoint_after_second_consume = stream.Save();
  A<CSSValue> value3 = nullptr;
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

  A<CSSValue> value4 = nullptr;
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

  std::array<A<CSSValue>, 5> values = {std::move(value1), std::move(value2),
                                       std::move(value3), std::move(value4),
                                       nullptr};
  PositionFromThreeOrFourValues(values, result_x, result_y);

  return true;
}

A<CSSValue> ConsumeBackgroundSize(CSSParserContext &context) {
  auto& stream = context.token_stream;

  // std::optional<WebFeature> negtive_size
  bool negtive_size = false;
  // ParsingStyle parsing_style
  bool parsing_legacy = true;

  // 关键字
  CSSValueId value_id = stream.Peek().ValueId();
  if (value_id == CSSValueId::Contain || value_id == CSSValueId::Cover) {
    return ConsumeIdent(context.token_stream);
  }

  A<CSSValue> horizontal = ConsumeIdent<CSSValueId::Auto>(stream);
  if (!horizontal) {
    horizontal.reset(ConsumeLengthOrPercent(context, CSSPrimitiveValue::ValueRange::kNonNegative));
  }
  if (!horizontal) {
    return nullptr;
  }

  A<CSSValue> vertical = nullptr;
  if (!stream.AtEnd()) {
    if (stream.Peek().ValueId() == CSSValueId::Auto) {
      stream.ConsumeIncludingWhitespace();
    } else {
      vertical.reset(ConsumeLengthOrPercent(context, CSSPrimitiveValue::ValueRange::kNonNegative));
    }
  } else if (parsing_legacy) {
    // "10px" ==>  "10px 10px"
    vertical.share(horizontal);
  }
  if (!vertical) {
    return horizontal;
  }
  return A<CSSValuePair>::make_new(std::move(horizontal), std::move(vertical),
                                   CSSValuePair::IdenticalValuesPolicy::Keep);
}


/*
background-clip: border-box;
background-clip: padding-box;
background-clip: content-box;
background-clip: text;
background-clip: border-area;
*/
A<CSSValue> ConsumeBackgroundBox(CSSParserTokenStream& stream) {
  return ConsumeIdent<CSSValueId::BorderBox, CSSValueId::PaddingBox,
                      CSSValueId::ContentBox>(stream);
}
A<CSSValue> ConsumeBackgroundBoxOrText(CSSParserTokenStream& stream) {
  return ConsumeIdent<CSSValueId::BorderBox, CSSValueId::PaddingBox,
                      CSSValueId::ContentBox, CSSValueId::Text>(stream);
}

/*
background-attachment: scroll;
background-attachment: fixed;
background-attachment: local;
*/
A<CSSValue> ConsumeBackgroundAttachment(CSSParserTokenStream& stream) {
  return ConsumeIdent<CSSValueId::Scroll, CSSValueId::Fixed,
                      CSSValueId::Local>(stream);
}

// Returns a token whose token.Value() will contain the URL,
// or the empty string if there are fetch restrictions,
// or an EOF token if we failed to parse.
//
// NOTE: We are careful not to return a reference, since the token
// will be overwritten once we move to the next one.
CSSParserToken ConsumeUrlAsToken(CSSParserContext& context) {
  auto& stream = context.token_stream;
  // unsigned int value_start_offset = stream.Offset();

  CSSParserToken token = stream.Peek();
  if (token.GetType() == CSSParserTokenType::Url) {
    stream.ConsumeIncludingWhitespace();
  } else if (token.FunctionId() == CSSValueId::Url) {
    {
      CSSParserTokenStream::RestoringBlockGuard guard(stream);
      stream.ConsumeWhitespace();
      
      token = stream.ConsumeIncludingWhitespace();
      if (token.GetType() == CSSParserTokenType::BadString || !stream.AtEnd()) {
        return CSSParserToken(CSSParserTokenType::Eof);
      }
      guard.Release();
    }
    assert(token.GetType() == CSSParserTokenType::String);
    stream.ConsumeWhitespace();
  } else {
    return CSSParserToken(CSSParserTokenType::Eof);
  }

  // unsigned int value_end_offset = stream.Offset();
  // if (stream.IsAttrTainted(value_start_offset, value_end_offset)) {
  //   return CSSParserToken(CSSParserTokenType::Eof);
  // }
  // return IsFetchRestricted(token.Value(), context)
  //            ? CSSParserToken(CSSParserTokenType::Url, u"")
  //            : token;
  return token;
}


static A<CSSImageValue> CreateCSSImageValueWithReferrer(
    const std::u16string& uri,
    const CSSParserContext& context) {
  // auto* image_value =
  //     MakeGarbageCollected<CSSImageValue>(*CollectUrlData(uri, context));
  // if (context.Mode() == kUASheetMode) {
  //   image_value->SetInitiator(fetch_initiator_type_names::kUacss);
  // }
  // return image_value;
  return nullptr;
}

A<CSSValue> ConsumeImage(CSSParserContext& context) {

  // const ConsumeGeneratedImagePolicy = ConsumeGeneratedImagePolicy::kAllow,
  // const ConsumeStringUrlImagePolicy = ConsumeStringUrlImagePolicy::kForbid,
  // const ConsumeImageSetImagePolicy = ConsumeImageSetImagePolicy::kAllow

  CSSParserTokenStream& stream = context.token_stream;

  CSSParserToken uri = ConsumeUrlAsToken(context);

  if (uri.GetType() != CSSParserTokenType::Eof) {
    return CreateCSSImageValueWithReferrer(uri.Name(), context);
  }
#if 0  
  if (string_url_image_policy == ConsumeStringUrlImagePolicy::kAllow) {
    wtf_size_t value_start_offset = stream.LookAheadOffset();
    String uri_string = ConsumeStringAsString(stream);
    if (!uri_string.IsNull()) {
      wtf_size_t value_end_offset = stream.LookAheadOffset();
      if (stream.IsAttrTainted(value_start_offset, value_end_offset)) {
        // https://drafts.csswg.org/css-values-5/#attr-security
        // “Additionally, attr() is not allowed to be used in any <url> value,
        // whether directly or indirectly. Doing so makes the property it’s used
        // in invalid.”
        return nullptr;
      }
      if (IsFetchRestricted(uri_string, context)) {
        uri_string = "";
      }
      return CreateCSSImageValueWithReferrer(uri_string, context);
    }
  }
  if (stream.Peek().GetType() == kFunctionToken) {
    CSSValueID id = stream.Peek().FunctionId();
    if (image_set_image_policy == ConsumeImageSetImagePolicy::kAllow &&
        IsImageSet(id)) {
      return ConsumeImageSet(stream, context, generated_image_policy);
    }
    if (generated_image_policy == ConsumeGeneratedImagePolicy::kAllow &&
        IsGeneratedImage(id)) {
      return ConsumeGeneratedImage(stream, context);
    }
    if (IsUASheetBehavior(context.Mode())) {
      return ConsumeLightDark(
          static_cast<CSSValue* (*)(CSSParserTokenStream&,
                                    const CSSParserContext&,
                                    const ColorParserContext&)>(
              ConsumeImageOrNone),
          stream, context, ColorParserContext());
    }
  }
#endif
  return nullptr;
}

A<CSSValue> ConsumeImageOrNone(CSSParserContext& context) {
  auto& stream = context.token_stream;

  if (stream.Peek().ValueId() == CSSValueId::None) {
    return ConsumeIdent(stream);
  }
  return ConsumeImage(context);
}


A<CSSValue> ConsumeBackgroundComponent(CSSPropertyId resolved_property,
                                     CSSParserContext& context) {

  // local_context.UseAliasParsing()
  bool use_alias_parsing = false;
  auto& stream = context.token_stream;

  switch (resolved_property) {
    case CSSPropertyId::BackgroundClip:
      return ConsumeBackgroundBoxOrText(stream);
    case CSSPropertyId::BackgroundAttachment:
      return ConsumeBackgroundAttachment(stream);
    case CSSPropertyId::BackgroundOrigin:
      return ConsumeBackgroundBox(stream);
    case CSSPropertyId::BackgroundImage:
      return ConsumeImageOrNone(context);
#if 0      
    case CSSPropertyId::BackgroundPositionX:
    case CSSPropertyId::WebkitMaskPositionX:
      return ConsumePositionLonghand<CSSValueID::kLeft, CSSValueID::kRight>(
          stream, context);
    case CSSPropertyId::BackgroundPositionY:
    case CSSPropertyId::WebkitMaskPositionY:
      return ConsumePositionLonghand<CSSValueID::kTop, CSSValueID::kBottom>(
          stream, context);
    case CSSPropertyId::BackgroundSize:
      return ConsumeBackgroundSize(stream, context,
                                   WebFeature::kNegativeBackgroundSize,
                                   ParsingStyle::kNotLegacy);
    case CSSPropertyId::MaskSize:
      return ConsumeBackgroundSize(stream, context,
                                   WebFeature::kNegativeMaskSize,
                                   ParsingStyle::kNotLegacy);
    case CSSPropertyId::BackgroundColor:
      return ConsumeColor(stream, context);
    case CSSPropertyId::MaskClip:
      return use_alias_parsing
                 ? ConsumePrefixedBackgroundBox(stream, AllowTextValue::kAllow)
                 : ConsumeCoordBoxOrNoClip(stream);
    case CSSPropertyId::MaskOrigin:
      return use_alias_parsing
                 ? ConsumePrefixedBackgroundBox(stream, AllowTextValue::kForbid)
                 : ConsumeCoordBox(stream);
    case CSSPropertyId::BackgroundRepeat:
    case CSSPropertyId::MaskRepeat:
      return ConsumeRepeatStyleValue(stream);
    case CSSPropertyId::MaskComposite:
      return ConsumeMaskComposite(stream);
    case CSSPropertyId::MaskMode:
      return ConsumeMaskMode(stream);
#endif
    default:
      return nullptr;
  };
}


}
}