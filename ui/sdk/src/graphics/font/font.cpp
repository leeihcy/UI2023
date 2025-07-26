#include "src/graphics/font/font.h"
#include <memory>

namespace ui {

// static
FontPool &FontPool::GetInstance() {
  static FontPool s_font_pool;
  return s_font_pool;
}

SkFont &FontCache::LoadSkia(FontDesc &key) {
  if (m_skia_font) {
    return *m_skia_font;
  }

  // TODO:
  SkFontStyle style(key.weight, SkFontStyle::kNormal_Width,
                    SkFontStyle::kUpright_Slant);
  sk_sp<SkTypeface> typeface =
      SkTypeface::MakeFromName(key.face.c_str(), style);
  m_skia_font = std::make_unique<SkFont>(typeface, key.size);
  return *m_skia_font;
}

SkFont &FontPool::GetSkiaFont(FontDesc &desc, float scale) {
  FontDesc desc_copy = desc;
  desc_copy.size = desc_copy.size*scale;
  FontCache &cache = GetFont(desc_copy);
  return cache.LoadSkia(desc_copy);
}

FontCache &FontPool::GetFont(FontDesc &key) {
  auto iter = m_caches.find(key);
  if (iter != m_caches.end()) {
    return iter->second;
  }

  m_caches[key] = FontCache();
  return m_caches[key];
}

} // namespace ui