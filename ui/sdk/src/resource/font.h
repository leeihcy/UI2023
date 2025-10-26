#ifndef _UI_SDK_SRC_RESOURCE_FONT_H_
#define _UI_SDK_SRC_RESOURCE_FONT_H_
#include "include/core/SkFont.h"
#include "include/interface/iresbundle.h"
#include "include/interface/renderlibrary.h"
#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

namespace std {
template <> struct hash<ui::FontDesc> {
  size_t operator()(const ui::FontDesc &key) const {
    return key.hash(); // 调用上述任一方法
  }
};
} // namespace std

namespace ui {
class FontRes;
struct FontFace {
  std::string font_family;
  std::string src;
};

class FontCache {
public:
  FontCache(FontRes& res);
  SkFont &LoadSkia(const FontDesc &key);

private:
  // skia
  std::unique_ptr<SkFont> m_skia_font;
  // others.. e.g. gdi/gdiplus

  FontRes& m_fontres;
};

class FontRes {
public:
  FontRes(ResourceBundle&);

  // parse
  void AddFontFace(const FontFace& font_face);

public:
  SkFont &GetSkiaFont(const FontDesc &key);
  Size MeasureString(const FontDesc &key, const char* text);
  
  FontFace* GetFontFace(const std::string& font_family);
  ResourceBundle& GetResourceBundle() { return m_resource_bundle; }

private:
  FontCache& GetFont(const FontDesc &key);

private:
  ResourceBundle& m_resource_bundle;

  // 字体样式缓存
  std::unordered_map<FontDesc, std::shared_ptr<FontCache>> m_caches;
  
  // 自定义字体列表，对于<style>下的<font-face>
  std::map<std::string, std::shared_ptr<FontFace>> m_custom_faces;
};

} // namespace ui

#endif