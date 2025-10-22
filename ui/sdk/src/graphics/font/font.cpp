#include "src/graphics/font/font.h"
#include "include/core/SkTypeface.h"
#include "third_party/skia/src/include/core/SkFont.h"
#include "third_party/skia/src/include/core/SkFontMetrics.h"
#include <memory>
#include <string.h>

namespace ui {

// static
FontPool &FontPool::GetInstance() {
  static FontPool s_font_pool;
  return s_font_pool;
}

SkFont &FontCache::LoadSkia(const FontDesc &key) {
  if (m_skia_font) {
    return *m_skia_font;
  }

  // TODO:
  SkFontStyle style(key.weight, SkFontStyle::kNormal_Width,
                    SkFontStyle::kUpright_Slant);

  bool is_font_file = false;
  if (key.face.length() > 4) {
     const char* key_face_ext = key.face.c_str() + key.face.length() - 4;
     if (strcmp(key_face_ext, ".ttf") == 0) {
      is_font_file = true;
     }
  }
  sk_sp<SkTypeface> typeface;
  if (is_font_file) {
    typeface = SkTypeface::MakeFromFile(key.face.c_str());
  // } else if (is_font_stream) {
  //   // typeface = SKTypeface::MakeFromStream(stream);
  } else {
    typeface = SkTypeface::MakeFromName(key.face.c_str(), style);
  }

  m_skia_font = std::make_unique<SkFont>(typeface, key.size);
  m_skia_font->setEdging(SkFont::Edging::kSubpixelAntiAlias);

  return *m_skia_font;
}

SkFont &FontPool::GetSkiaFont(const FontDesc &desc/*, float scale*/) {
#if 0 // skia已经整体缩放了，不需要字体再单独缩放。
  FontDesc desc_copy = desc; // TODO: 能否减少这一步拷贝

  desc_copy.size = desc_copy.size*scale;
  FontCache &cache = GetFont(desc_copy);

  return cache.LoadSkia(desc_copy);
#else
  FontCache &cache = GetFont(desc);
  return cache.LoadSkia(desc);
#endif
}

FontCache &FontPool::GetFont(const FontDesc &key) {
  auto iter = m_caches.find(key);
  if (iter != m_caches.end()) {
    return iter->second;
  }

  m_caches[key] = FontCache();
  return m_caches[key];
}

Size FontPool::MeasureString(const FontDesc &key, const char* text) {
  // TBD: 只支持skia
  SkFont& font = GetSkiaFont(key);
  int width = font.measureText(text, strlen(text), SkTextEncoding::kUTF8);
  
  SkFontMetrics metrics;
  font.getMetrics(&metrics);
  int height = metrics.fDescent - metrics.fAscent + metrics.fLeading;

  return {width, height};
}

} // namespace ui