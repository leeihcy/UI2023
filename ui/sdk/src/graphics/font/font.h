#ifndef _UI_SDK_SRC_GRAPHICS_FONT_FONT_H_
#define _UI_SDK_SRC_GRAPHICS_FONT_FONT_H_
#include "include/core/SkFont.h"
#include "include/interface/renderlibrary.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace ui {

class FontCache {
public:
  SkFont &LoadSkia(const FontDesc &key);

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

  SkFont &GetSkiaFont(const FontDesc &key);
  Size MeasureString(const FontDesc &key, const char* text);
private:
  FontCache& GetFont(const FontDesc &key);

private:
  std::unordered_map<FontDesc, FontCache> m_caches;
};

} // namespace ui

#endif