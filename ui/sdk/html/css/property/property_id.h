#ifndef _UI_SDK_HTML_CSS_PROPERTY_PROPERTYID_H_
#define _UI_SDK_HTML_CSS_PROPERTY_PROPERTYID_H_

#include <string>
namespace html {

enum class CSSPropertyID : int {
  Invalid = 0,
  Variable = 1,

  // 固定格式， id, // "name"
  // gen_property_hash.py 脚本将读取这些字段生成Perfect Hash。
  
  // https://www.w3.org/TR/css-backgrounds-4/
  Background,          // "background"
  BackgroundAttachment,// "background-attachment"
  BackgroundClip,      // "background-clip"
  BackgroundColor,     // "background-color"
  BackgroundImage,     // "background-image"
  BackgroundOrigin,    // "background-origin"
  BackgroundPosition,  // "background-position"
  BackgroundPositionX, // "background-position-x"
  BackgroundPositionY, // "background-position-y"
  BackgroundRepeat,    // "background-repeat"
  BackgroundSize,      // "background-size"
  BackgroundBlendMode, // "background-blend-mode"
  
  CSSPropertyCount,
};

constexpr size_t MaxCSSPropertyNameLength = 48;

CSSPropertyID CSSPropertyNameToId(const std::u16string& name);

// perfect hash function.
CSSPropertyID CSSPropertyIDNameToIdByHash(const char* name, unsigned int len);
}

#endif