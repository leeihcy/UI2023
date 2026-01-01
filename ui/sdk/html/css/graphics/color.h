#ifndef _HTML_CSS_GRAPHICS_COLOR_H_
#define _HTML_CSS_GRAPHICS_COLOR_H_
#include <string>

namespace html {

typedef unsigned RGBA32; 
class Color {
public:
  static bool ParseHexColor(const std::u16string& text, Color& out);
  static constexpr Color FromRGBA32(RGBA32 color) { return Color(color); }

  constexpr explicit Color(RGBA32 color)
      : param0_is_none_(0),
        param1_is_none_(0),
        param2_is_none_(0),
        alpha_is_none_(0),
        param0_(((color >> 16) & 0xFF)),
        param1_(((color >> 8) & 0xFF)),
        param2_(((color >> 0) & 0xFF)),
        alpha_(((color >> 24) & 0xFF) / 255.f) {}

  inline bool operator==(const Color& other) const {
    return color_space_ == other.color_space_ &&
           param0_is_none_ == other.param0_is_none_ &&
           param1_is_none_ == other.param1_is_none_ &&
           param2_is_none_ == other.param2_is_none_ &&
           alpha_is_none_ == other.alpha_is_none_ && param0_ == other.param0_ &&
           param1_ == other.param1_ && param2_ == other.param2_ &&
           alpha_ == other.alpha_;
  }
  unsigned GetHash() const;

  // std::map<Color, xxx>
  inline int operator < (const Color& other) const {
    float fother = other.param0_ + other.param1_ + other.param2_ + other.alpha_;
    float fthis = param0_ + param1_ + param2_ + alpha_;
    return fother - fthis;
  }

public:
  static const Color kBlack;
  static const Color kWhite;
  static const Color kDarkGray;
  static const Color kGray;
  static const Color kLightGray;
  static const Color kTransparent;

  enum class ColorSpace : uint8_t {
    kSRGB,
    kSRGBLegacy,
    kNone,
  };

private:
  ColorSpace color_space_ = ColorSpace::kSRGBLegacy;

  // Whether or not color parameters were specified as none (this only affects
  // interpolation behavior, the parameter values area always valid).
  uint8_t param0_is_none_ : 1;
  uint8_t param1_is_none_ : 1;
  uint8_t param2_is_none_ : 1;
  uint8_t alpha_is_none_ : 1;

  // The color parameters.
  float param0_ = 0.f;
  float param1_ = 0.f;
  float param2_ = 0.f;

  // The alpha value for the color is guaranteed to be in the [0, 1] interval.
  float alpha_ = 0.f;
};


}

#endif