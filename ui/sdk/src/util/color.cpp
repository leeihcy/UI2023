#include "include/util/color.h"
#include "include/inc.h"
#include "include/util/math.h"
#include <math.h>

//
//  色调调节方法：
//    1)正常模式：将所有像素的H值偏移h，
//    2)着色模式：将所有像素的H值设置为h
//
//	  H的取值范围是0 - 240的环形值，到达240之后又回到0起点
//
//	饱和度调节方法：
//   饱和度的变化规律是： dS = s/100;
//	  当dS > 0, newS = S / (1-dS)
//	  当dS < 0, newS = S * (1+dS)
//
//	  S的取值范围为 -100 ~ +100
//
//	亮度调节方法：
//    不需要转换为HLS模式，采用的方法是将一张白色图片或者一张黑色图片盖上原图上的方法
//    R = 255*d + R*(1-d) = 255*d + R - R*d = R + (255-R)*d
//    R = 0*d + R*(1-d) = R - R*d
//
//    L的取值范围为 -100 ~ +100
//

/*  附表（HSL/V/B在各种环境下的取值范围）：
        Applications       Space       H Range      S Range      L/V/B Range

        Paint Shop Pro     HSL         0 - 255      0 - 255      L  0 - 255
        Gimp               HSV         0 - 360°     0 - 100      V  0 - 100
        Photoshop          HSV         0 - 360°     0 - 100%     B  0 - 100%
        Windows            HSL         0 - 240      0 - 240      L  0 - 240
        Linux / KDE        HSV         0 - 360°     0 - 255      V  0 - 255
        GTK                HSV         0 - 360°     0 - 1.0      V  0 - 1.0
        Java (awt.Color)   HSV         0 - 1.0      0 - 1.0      B  0 - 1.0
        Apple              HSV         0 - 360°     0 - 100%     L  0 - 100%
*/

namespace ui {

// static 
Color Color::Make(uint v) {
  Color c;
  c.value = v;
  return c;
}
// static 
Color Color::MakeRGBA(byte r1, byte g1, byte b1, byte a1) {
  Color c;
  c.r = r1;
  c.g = g1;
  c.b = b1;
  c.a = a1;
  return c;
}
// static 
Color Color::MakeARGB(byte a1, byte r1, byte g1, byte b1)
{
  return Color::MakeRGBA(r1, g1, b1, a1);
}
// static 
Color Color::MakeRGB(byte r1, byte g1, byte b1) {
  return Color::MakeRGBA(r1, g1, b1, 255);
}
Color Color::transparent()  { return Color::Make(0); }

Color Color::black() { return Color::MakeRGB(0, 0, 0); }
Color Color::silver() { return Color::MakeRGB(192, 192, 192); }
Color Color::gray() { return Color::MakeRGB(128, 128, 128); }
Color Color::white() { return Color::MakeRGB(255, 255, 255); }
Color Color::maroon() { return Color::MakeRGB(128, 0, 0); }
Color Color::red() { return Color::MakeRGB(255, 0, 0); }
Color Color::purple() { return Color::MakeRGB(128, 0, 128); }
Color Color::fuchsia() { return Color::MakeRGB(255, 0, 255); }
Color Color::green() { return Color::MakeRGB(0, 128, 0); }
Color Color::lime() { return Color::MakeRGB(0, 255, 0); }
Color Color::olive() { return Color::MakeRGB(128, 128, 0); }
Color Color::yellow() { return Color::MakeRGB(255, 255, 0); }
Color Color::navy() { return Color::MakeRGB(0, 0, 128); }
Color Color::blue() { return Color::MakeRGB(0, 0, 255); }
Color Color::teal() { return Color::MakeRGB(0, 128, 128); }
Color Color::aqua() { return Color::MakeRGB(0, 255, 255); }

// 保留ALPHAW值颜色拷贝
void Color::ReplaceRGB(uint c) {
  byte a_old = a;
  value = c;
  a = a_old;
}

bool Color::operator==(const Color& c) const {
  return this->value == c.value;
}

bool Color::IsTransparnt() const {
  return this->a == 0;
}

// uint Color::GetGDIValue() {
//   uint gdiColor = 0;
//   // gdiColor = RGB(r, g, b);

//   if (255 == a) {
//     gdiColor = RGB(r, g, b);
//   } else if (0 == a) {
//   } else {
//     // 这种算法其实不对，例如a=0时，画出来的是黑色，但其它是全透明的
//     // 只有在需要alpha通道的时候才能这么获取，用于预乘
//     byte br = r * a >> 8;
//     byte bg = g * a >> 8;
//     byte bb = b * a >> 8;

//     gdiColor = RGB(br, bg, bb);
//   }

//   return gdiColor;
// }

void Color::ToHexString(char *szBuffer) {
  snprintf(szBuffer, 16, "0x%02X%02X%02X%02X", a, r, g, b);
}
void Color::ToWebString(char *szBuffer) {
  if (a == 255) {
    snprintf(szBuffer, 16, "#%02X%02X%02X", r, g, b);
    if (szBuffer[1] == szBuffer[2] && szBuffer[3] == szBuffer[4] &&
        szBuffer[5] == szBuffer[6]) {
      szBuffer[2] = szBuffer[3];
      szBuffer[3] = szBuffer[5];
      szBuffer[4] = 0;
    }
  } else {
    snprintf(szBuffer, 16, "#%02X%02X%02X%02X", a, r, g, b);
    if (szBuffer[1] == szBuffer[2] && szBuffer[3] == szBuffer[4] &&
        szBuffer[5] == szBuffer[6] && szBuffer[7] == szBuffer[8]) {
      szBuffer[2] = szBuffer[3];
      szBuffer[3] = szBuffer[5];
      szBuffer[4] = szBuffer[7];
      szBuffer[5] = 0;
    }
  }
}

// http://en.wikipedia.org/wiki/HSL_and_HSV
void Color::GetHSL(HSL &hsl) {
  double dr = r / 255.0;
  double dg = g / 255.0;
  double db = b / 255.0;

  double max = std::max(std::max(dr, dg), db);
  double min = std::min(std::min(dr, dg), db);

  double &hue = hsl.hue;
  double &lightness = hsl.lightness;
  double &saturation = hsl.saturation;

  if (max == min)
    hue = 0;
  else if (max == dr)
    hue = (60.0 * ((dg - db) / (max - min))) + 360.0;
  else if (max == dg)
    hue = (60.0 * ((db - dr) / (max - min))) + 120.0;
  else
    hue = (60.0 * ((dr - dg) / (max - min))) + 240.0;

  if (hue >= 360.0)
    hue -= 360.0;

  // hue /= 360.0;  <-- 使hue在[0,1)区间内

  lightness = 0.5 * (max + min);
  if (max == min)
    saturation = 0.0;
  else if (lightness <= 0.5)
    saturation = ((max - min) / (max + min));
  else
    saturation = ((max - min) / (2.0 - (max + min)));
}

void Color::GetHSV(HSV &hsv) {
  double dr = r / 255.0;
  double dg = g / 255.0;
  double db = b / 255.0;

  double max = std::max(std::max(dr, dg), db);
  double min = std::min(std::min(dr, dg), db);

  double &hue = hsv.hue;
  double &value = hsv.value;
  double &saturation = hsv.saturation;

  if (max == min)
    hue = 0;
  else if (max == dr)
    hue = (60.0 * ((dg - db) / (max - min))) + 360.0;
  else if (max == dg)
    hue = (60.0 * ((db - dr) / (max - min))) + 120.0;
  else
    hue = (60.0 * ((dr - dg) / (max - min))) + 240.0;

  if (hue >= 360.0)
    hue -= 360.0;

  // hue /= 360.0;  <-- 使hue在[0,1)区间内

  value = max;
  if (value == 0)
    saturation = 0;
  else
    saturation = (max - min) / value;
}

double calcHue(double temp1, double temp2, double hueVal) {
  if (hueVal < 0.0)
    hueVal++;
  else if (hueVal > 1.0)
    hueVal--;

  if (hueVal * 6.0 < 1.0)
    return temp1 + (temp2 - temp1) * hueVal * 6.0;
  if (hueVal * 2.0 < 1.0)
    return temp2;
  if (hueVal * 3.0 < 2.0)
    return temp1 + (temp2 - temp1) * (2.0 / 3.0 - hueVal) * 6.0;

  return temp1;
}

void Color::SetHSL(HSL &hsl) {
  a = 255;

#if 0 // webkit
    if (hsl.saturation == 0)
    {
        r = static_cast<byte>(round(255.0 * hsl.lightness));
        g = static_cast<byte>(round(255.0 * hsl.lightness));
        b = static_cast<byte>(round(255.0 * hsl.lightness));
    }
    else
    {
        double  temp2 = 0;
        if (hsl.lightness < 0.5) 
            temp2 = hsl.lightness * (1.0 + hsl.saturation);
        else
            temp2 = (hsl.lightness + hsl.saturation) - (hsl.lightness * hsl.saturation);

        double temp1 = 2.0 * hsl.lightness - temp2;
        double hue = hsl.hue / 360.0;  // [0~1)

        r = static_cast<byte>(round(255.0 * calcHue(temp1, temp2, hue + 1.0 / 3.0)));
        g = static_cast<byte>(round(255.0 * calcHue(temp1, temp2, hue)));
        b = static_cast<byte>(round(255.0 * calcHue(temp1, temp2, hue - 1.0 / 3.0)));
    }
#else // wiki

  double C = (1 - abs(2 * hsl.lightness - 1)) * hsl.saturation;
  double H = hsl.hue / 60.0;

  // 求H % 2的结果
  double mod2 = H;
  if (H > 4)
    mod2 -= 4;
  else if (H > 2)
    mod2 -= 2;

  double X = C * (1 - abs(mod2 - 1));

  double dr = 0, dg = 0, db = 0;
  if (H >= 0 && H < 1) {
    dr = C;
    dg = X;
    db = 0;
  } else if (H >= 1 && H < 2) {
    dr = X;
    dg = C;
    db = 0;
  } else if (H >= 2 && H < 3) {
    dr = 0;
    dg = C;
    db = X;
  } else if (H >= 3 && H < 4) {
    dr = 0;
    dg = X;
    db = C;
  } else if (H >= 4 && H < 5) {
    dr = X;
    dg = 0;
    db = C;
  } else if (H >= 5 && H < 6) {
    dr = C;
    dg = 0;
    db = X;
  } else {
    dr = dg = db = 0;
  }

  double m = hsl.lightness - C / 2;

  r = static_cast<byte>(_round(255.0 * (dr + m)));
  g = static_cast<byte>(_round(255.0 * (dg + m)));
  b = static_cast<byte>(_round(255.0 * (db + m)));

#endif
}

void Color::SetHSV(HSV &hsv) {
  a = 255;

  double C = hsv.value * hsv.saturation; // chroma

  double H = hsv.hue / 60.0;

  // 求H % 2的结果
  double mod2 = H;
  if (H > 4)
    mod2 -= 4;
  else if (H > 2)
    mod2 -= 2;

  double X = C * (1 - abs(mod2 - 1));

  double dr = 0, dg = 0, db = 0;
  if (H >= 0 && H < 1) {
    dr = C;
    dg = X;
    db = 0;
  } else if (H >= 1 && H < 2) {
    dr = X;
    dg = C;
    db = 0;
  } else if (H >= 2 && H < 3) {
    dr = 0;
    dg = C;
    db = X;
  } else if (H >= 3 && H < 4) {
    dr = 0;
    dg = X;
    db = C;
  } else if (H >= 4 && H < 5) {
    dr = X;
    dg = 0;
    db = C;
  } else if (H >= 5 && H < 6) {
    dr = C;
    dg = 0;
    db = X;
  } else {
    dr = dg = db = 0;
  }

  double m = hsv.value - C;

  r = static_cast<byte>(_round(255.0 * (dr + m)));
  g = static_cast<byte>(_round(255.0 * (dg + m)));
  b = static_cast<byte>(_round(255.0 * (db + m)));
}

// #ifdef EDITOR_MODE
// void Color::SetResId(const wchar_t *szId) {
//   if (m_szResId) {
//     SAFE_ARRAY_DELETE(m_szResId);
//   }

//   if (szId && szId[0]) {
//     m_szResId = new wchar_t[strlen(szId) + 1];
//     wcscpy(m_szResId, szId);
//   }
// }
// const wchar_t *Color::GetResId() { return m_szResId; }
// #endif
} // namespace ui
