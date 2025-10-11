#ifndef _UI_COLOR_H_
#define _UI_COLOR_H_
#include "sdk/include/interface.h"

namespace ui {
// 换色
#if 0
#define MIN_HUE_VALUE 0 // 色调  HLS范围定义
#define MAX_HUE_VALUE 240
#define MIN_LUMINANCE_VALUE 0 // 亮度
#define MAX_LUMINANCE_VALUE 240
#define MIN_SATURATION_VALUE 0 // 饱和度
#define MAX_SATURATION_VALUE 240
#else
#define MIN_HUE_VALUE 0 // 色调  HLS范围定义
#define MAX_HUE_VALUE 360
#define MIN_LUMINANCE_VALUE 0 // 亮度
#define MAX_LUMINANCE_VALUE 1
#define MIN_SATURATION_VALUE 0 // 饱和度
#define MAX_SATURATION_VALUE 1
#endif

#define CHANGE_SKIN_HLS_FLAG_H 1
#define CHANGE_SKIN_HLS_FLAG_L 2
#define CHANGE_SKIN_HLS_FLAG_S 4
#define CHANGE_SKIN_HLS_FLAG_HLS                                               \
  (CHANGE_SKIN_HLS_FLAG_H | CHANGE_SKIN_HLS_FLAG_L | CHANGE_SKIN_HLS_FLAG_S)
#define CHANGE_SKIN_HLS_FALG_REPLACE_MODE 8 // 着色模式??

typedef struct {
  double hue;        // [0 ~ 360^o)
  double saturation; // [0, 1]
  double lightness;  // [0, 1]
} HSL;

typedef struct {
  double hue;        // [0 ~ 360^o)
  double saturation; // [0, 1]
  double value;      // [0, 1]
} HSV, HSB;

struct UIAPI Color {
  Color();
  
  static Color Make(uint);
  static Color MakeRGBA(byte r1, byte g1, byte b1, byte a1);
  static Color MakeARGB(byte a1, byte r1, byte g1, byte b1);
  static Color MakeRGB(byte r1, byte g1, byte b1);

  static Color transparent();

  static Color black();
  static Color silver();
  static Color gray();
  static Color white();
  static Color maroon();
  static Color red();
  static Color purple();
  static Color fuchsia();
  static Color green();
  static Color lime();
  static Color olive();
  static Color yellow();
  static Color navy();
  static Color blue();
  static Color teal();
  static Color aqua();

  // 保留ALPHAW值颜色拷贝
  void ReplaceRGB(uint c);
  uint GetGDIValue();

  void GetHSL(HSL &hsl);
  void GetHSV(HSV &hsv);
  void SetHSL(HSL &hsl);
  void SetHSV(HSV &hsv);

  void ToHexString(char *szBuffer);
  void ToWebString(char *szBuffer);

  bool operator==(const Color& c) const;
  bool IsTransparnt() const;

  union {
    uint value;
    struct {
      byte b; // 注：位图内存中颜色的排列顺序为b g r a
      byte g;
      byte r;
      byte a;
    };
  };

};

} // namespace ui
#endif // _UI_COLOR_H_
