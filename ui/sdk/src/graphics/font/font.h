#ifndef _UI_SDK_SRC_GRAPHICS_FONT_FONT_H_
#define _UI_SDK_SRC_GRAPHICS_FONT_FONT_H_
#include "include/core/SkFont.h"
#include "include/interface/renderlibrary.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace ui {

// struct FontDesc {
//   long lfHeight;
//   long lfWidth;
//   long lfEscapement;
//   long lfOrientation;
//   long lfWeight;
//   unsigned char lfItalic;
//   unsigned char lfUnderline;
//   unsigned char lfStrikeOut;
//   unsigned char lfCharSet;
//   unsigned char lfOutPrecision;
//   unsigned char lfClipPrecision;
//   unsigned char lfQuality;
//   unsigned char lfPitchAndFamily;
//   char lfFaceName[LF_FACESIZE];
// };


class FontCache {
public:
  SkFont &LoadSkia(FontDesc &key);

private:
  // skia
  std::unique_ptr<SkFont> m_skia_font;

  // others.. e.g. gdi/gdiplus
};

}; // namespace ui

namespace std {
template <> struct hash<ui::FontDesc> {
  size_t operator()(const ui::FontDesc &key) const {
    return key.hash(); // 调用上述任一方法
  }
};
} // namespace std

namespace ui {

class FontPool {
public:
  static FontPool &GetInstance();

  SkFont &GetSkiaFont(const FontDesc &key, float scale);
private:
  FontCache& GetFont(const FontDesc &key);

private:
  std::unordered_map<FontDesc, FontCache> m_caches;
};

} // namespace ui

#endif