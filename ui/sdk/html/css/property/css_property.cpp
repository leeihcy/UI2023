#include "html/css/property/css_property.h"
#include "html/css/parser/css_parser.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/property/property_id.h"
#include "html/css/property/css_parsing_utils.h"
#include "html/css/property/css_property_instances.h"
#include "html/css/style_property_shorthand.h"
#include <assert.h>
#include <array>


namespace html {

// static 
const CSSProperty& CSSProperty::Get(CSSPropertyId id) {
  return GetCSSPropertyInternal(id);
}

// https://www.w3.org/TR/css-backgrounds-4/
//
// Background 允许你在一个元素上叠加多个背景图像，每个层都有自己的属性设置。
// .element {
//   background: 
//     /* 第一层 */
//     url(image1.jpg) no-repeat 10px 20px,
//     /* 第二层 */
//     url(image2.png) repeat-x center / contain,
//     /* 第三层 */
//     linear-gradient(to right, red, blue) fixed,
//     /* 最后一层：背景色（应用于所有层） */
//     #f0f0f0;
// }
// 规则：
//   先列出的层显示在最上面（类似z-index）
//   每个背景层可以有自己的属性值：
//   background-color 只能有一个值，且总是：显示在所有层的最下面,不受逗号分隔的层影响
//
bool Background::ParseShorthand(CSSParserContext &context) const {
  const StylePropertyShorthand& shorthand = backgroundShorthand();
  assert(false);
#if 0
  const unsigned longhand_count = shorthand.length();
  assert(longhand_count <= 10);

  std::array<std::vector<const CSSValue*>, 10> longhands;
  

  bool implicit = false;
  // background_color只能放在最后一个layer。
  bool previous_layer_had_background_color = false;

  // 循环每个background layer，以逗号分隔。
  do {
    std::array<bool, 10> parsed_longhand = {false};
    CSSValue* origin_value = nullptr;
    bool found_property;
    bool found_any = false;

    // 依次解析每个字段
    do {
      found_property = false;
      bool bg_position_parsed_in_current_layer = false;

      // 匹配每个longhand property
      for (unsigned i = 0; i < longhand_count; ++i) {
        if (parsed_longhand[i]) {
          continue;
        }

        CSSValue* value = nullptr;
        CSSValue* value_y = nullptr;
        const CSSProperty& property = shorthand.properties()[i];
        if (property.IdEquals(CSSPropertyId::BackgroundPositionX)) {
          if (!ConsumePosition(context.token_stream, context, UnitlessQuirk::kForbid,
                               WebFeature::kThreeValuedPositionBackground,
                               value, value_y)) {
            continue;
          }
          if (value) {
            bg_position_parsed_in_current_layer = true;
          }
        } else if (property.IdEquals(CSSPropertyId::BackgroundSize)) {
          if (!ConsumeSlashIncludingWhitespace(context.token_stream)) {
            continue;
          }
          value = ConsumeBackgroundSize(
              context.token_stream, context, WebFeature::kNegativeBackgroundSize,
              ParsingStyle::kNotLegacy);
          if (!value || !bg_position_parsed_in_current_layer) {
            return false;
          }
        } else if (property.IdEquals(CSSPropertyId::BackgroundPositionY)) {
          continue;
        } else {
          value =
              ConsumeBackgroundComponent(property.PropertyId(), context,
                                         /*local_context.UseAliasParsing()*/);
        }
        if (value) {
          if (property.IdEquals(CSSPropertyId::BackgroundOrigin)) {
            origin_value = value;
          }
          parsed_longhand[i] = true;
          found_property = true;
          found_any = true;
          longhands[i].push_back(value);
          if (value_y) {
            parsed_longhand[i + 1] = true;
            longhands[i + 1].push_back(value_y);
          }
        }
      }
    } while (found_property && !context.token_stream.AtEnd() &&
             context.token_stream.Peek().Type() != CSSParserTokenType::Comma);
    if (!found_any) {
      return false;
    }
    if (previous_layer_had_background_color) {
      // Colors are only allowed in the last layer; previous layer had
      // a background color and we now know for sure it was not the last one,
      // so return parse failure.
      return false;
    }
#endif
#if 0
    // TODO(timloh): This will make invalid longhands, see crbug.com/386459
    for (unsigned i = 0; i < longhand_count; ++i) {
      const CSSProperty& property = *shorthand.properties()[i];

      if (property.IDEquals(CSSPropertyID::kBackgroundColor)) {
        if (parsed_longhand[i]) {
          previous_layer_had_background_color = true;
        }
      }
      if (!parsed_longhand[i]) {
        if ((property.IDEquals(CSSPropertyID::kBackgroundClip) ||
             property.IDEquals(CSSPropertyID::kMaskClip)) &&
            origin_value) {
          longhands[i].push_back(origin_value);
          continue;
        }

        if (shorthand_id == CSSPropertyID::kMask) {
          longhands[i].push_back(To<Longhand>(property).InitialValue());
        } else {
          longhands[i].push_back(CSSInitialValue::Create());
        }
      }
    }

  } while (css_parsing_utils::ConsumeCommaIncludingWhitespace(context.token_stream));
#endif
#if 0
  for (unsigned i = 0; i < longhand_count; ++i) {
    const CSSProperty& property = *shorthand.properties()[i];

    const CSSValue* longhand;
    if (property.IDEquals(CSSPropertyID::kBackgroundColor)) {
      // There can only be one background-color (we've verified this earlier,
      // by means of previous_layer_had_background_color), so pick out only
      // the last one (any others will just be “initial” over and over again).
      longhand = longhands[i].back().Get();
    } else {
      // To conserve memory we don't wrap a single value in a list.
      longhand = GetSingleValueOrMakeList(CSSValue::kCommaSeparator,
                                          std::move(longhands[i]));
    }

    AddProperty(property.PropertyID(), shorthand.id(), *longhand, important,
                implicit ? IsImplicitProperty::kImplicit
                         : IsImplicitProperty::kNotImplicit,
                properties);
  }
#endif
  return true;
}

U<CSSValue> BackgroundColor::ParseSingleValue(CSSParserContext &context) const {
  return css_parsing_utils::ConsumeColor(context.token_stream);
}


}