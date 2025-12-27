#ifndef _UI_SDK_HTML_CSS_PROPERTY_PROPERTYID_H_
#define _UI_SDK_HTML_CSS_PROPERTY_PROPERTYID_H_

#include <string>
namespace html {

enum class CSSPropertyId : int {
  Invalid = -1,
  Variable = 0,

  // 固定格式， id, // "name"
  // gen_property_hash.py 脚本将读取这些字段生成Perfect Hash。
  
  Background,      // "background"
  BackgroundClip,      // "background-clip"
  BackgroundColor,     // "background-color"
  BackgroundRepeat,    // "background-repeat"
  BackgroundOrigin,    // "background-origin"
  BackgroundSize,      // "background-size"
  BackgroundPosition,  // "background-position"
  BackgroundPositionX, // "background-position-x"
  BackgroundPositionY, // "background-position-y"
  BackgroundBlendMode, // "background-blend-mode"
  BackgroundImage,     // "background-image"
  BackgroundAttachment,// "background-attachment"



  CSSPropertyCount,
};

constexpr size_t MaxCSSPropertyNameLength = 48;

CSSPropertyId CSSPropertyNameToId(const std::u16string& name);

// perfect hash function.
CSSPropertyId CSSPropertyNameToIdByHash(const char* name, unsigned int len);
}

#endif