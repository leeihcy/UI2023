#ifndef _UI_SDK_SRC_GRAPHICS_FONT_FONT_H_
#define _UI_SDK_SRC_GRAPHICS_FONT_FONT_H_
#include "include/core/SkFont.h"
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

struct FontDesc {
  std::string face;
  int size = 0;
  int weight = 0;
  int orientation = 0;
  union {
    uint32_t style = 0;
    struct {
      bool italic : 1;
      bool underline : 1;
      bool strikeout : 1;
      bool cleartype : 1;
      bool subpixel : 1;
    };
  };
  bool operator==(const FontDesc& o) const {
    return face == o.face && size == o.size && weight == o.weight &&
      orientation == o.orientation && style == o.style;
  }

  size_t hash() const {
    size_t seed = 0;
    seed ^=
        std::hash<std::string>()(face) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<unsigned short>()(size) + 0x9e3779b9 + (seed << 6) +
            (seed >> 2);
    seed ^= std::hash<unsigned short>()(weight) + 0x9e3779b9 + (seed << 6) +
            (seed >> 2);
    seed ^= std::hash<unsigned short>()(orientation) + 0x9e3779b9 +
            (seed << 6) + (seed >> 2);
    seed ^= std::hash<uint32_t>()(style);
    return seed;
  }
};

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

  SkFont &GetSkiaFont(FontDesc &key, float scale);
private:
  FontCache& GetFont(FontDesc &key);

private:
  std::unordered_map<FontDesc, FontCache> m_caches;
};

} // namespace ui

#endif