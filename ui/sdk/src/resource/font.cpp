#include "src/resource/font.h"
#include "include/core/SkData.h"
#include "include/core/SkTypeface.h"
#include "include/interface/ibundlesource.h"
#include "include/macro/helper.h"
#include "include/util/log.h"
#include "src/parser/datasource/bundle_source.h"
#include "src/resource/res_bundle.h"
#include "third_party/skia/src/include/core/SkFont.h"
#include "third_party/skia/src/include/core/SkFontMetrics.h"
#include <memory>
#include <string.h>
#include <vector>


namespace ui {

FontCache::FontCache(FontRes &res) : m_fontres(res) {}
SkFont &FontCache::LoadSkia(const FontDesc &key) {
  if (m_skia_font) {
    return *m_skia_font;
  }

  // TODO:
  SkFontStyle style(key.weight, SkFontStyle::kNormal_Width,
                    SkFontStyle::kUpright_Slant);

  FontFace *custom_fontface = m_fontres.GetFontFace(key.face);

  sk_sp<SkTypeface> typeface;
  if (custom_fontface) {
    // 如果是自定义字体，加载对应的本地资源文件

    BundleSource *bundle_resouce = m_fontres.GetResourceBundle().GetSource();
    UIASSERT(bundle_resouce);

    std::string font_path;
    if (bundle_resouce->GetType() == eBundleFormat::Directory &&
        bundle_resouce->loadFullPath(custom_fontface->src.c_str(), font_path)) {
      typeface = SkTypeface::MakeFromFile(font_path.c_str());
      m_skia_font = std::make_unique<SkFont>(typeface, key.size);
    } else {
      std::vector<unsigned char> buffer;
      bundle_resouce->loadBuffer(custom_fontface->src.c_str(), buffer);

      // TODO: reduce the memory alloc
      // sk_sp<SkData> sk_data =
      //     SkData::MakeWithoutCopy(buffer.data(), buffer.size());
      sk_sp<SkData> sk_data =
          SkData::MakeWithCopy(buffer.data(), buffer.size());

      typeface = SkTypeface::MakeFromData(sk_data);
      m_skia_font = std::make_unique<SkFont>(typeface, key.size);
    }
  } else {
    typeface = SkTypeface::MakeFromName(key.face.c_str(), style);
    m_skia_font = std::make_unique<SkFont>(typeface, key.size);
  }
  if (!typeface) {
    UI_LOG_WARN("Create font failed: %s", key.face.c_str());
  }

  // m_skia_font = std::make_unique<SkFont>(typeface, key.size);
  m_skia_font->setEdging(SkFont::Edging::kSubpixelAntiAlias);

  return *m_skia_font;
}

//----------------------------------------------------------------

FontRes::FontRes(ResourceBundle &rb) : m_resource_bundle(rb) {}

SkFont &FontRes::GetSkiaFont(const FontDesc &desc /*, float scale*/) {
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

FontCache &FontRes::GetFont(const FontDesc &key) {
  auto iter = m_caches.find(key);
  if (iter != m_caches.end()) {
    return *(iter->second.get());
  }

  std::shared_ptr<FontCache> cache = std::make_shared<FontCache>(*this);
  m_caches[key] = cache;
  return *(cache.get());
}

void FontRes::AddFontFace(const FontFace &font_face) {
  if (font_face.font_family.empty()) {
    return;
  }
  m_custom_faces[font_face.font_family] = std::make_shared<FontFace>(font_face);
}
FontFace *FontRes::GetFontFace(const std::string& font_family) {

  auto face_iter = m_custom_faces.find(font_family);
  if (face_iter != m_custom_faces.end()) {
    return face_iter->second.get();
  }
  return nullptr;
}

Size FontRes::MeasureString(const FontDesc &key, const char *text) {
  // TBD: 只支持skia
  SkFont &font = GetSkiaFont(key);
  int width = font.measureText(text, strlen(text), SkTextEncoding::kUTF8);

  SkFontMetrics metrics;
  font.getMetrics(&metrics);
  int height = metrics.fDescent - metrics.fAscent + metrics.fLeading;

  return {width, height};
}

} // namespace ui