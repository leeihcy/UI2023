#include "src/graphics/skia/skia_bitmap.h"
#include "third_party/skia/src/include/core/SkColorSpace.h"
#include "third_party/skia/src/include/core/SkData.h"
#include "third_party/skia/src/include/core/SkImageGenerator.h"
namespace ui {

std::shared_ptr<IRenderBitmap> SkiaRenderBitmap::CreateInstance() {
  return std::shared_ptr<IRenderBitmap>(new SkiaRenderBitmap());
}

GRAPHICS_RENDER_LIBRARY_TYPE SkiaRenderBitmap::GetGraphicsRenderLibraryType() {
  return GRAPHICS_RENDER_LIBRARY_TYPE_SKIA;
}

bool SkiaRenderBitmap::LoadFromFile(const char * path,
                                    RENDER_BITMAP_LOAD_FLAG e) {
  sk_sp<SkData> image_data = SkData::MakeFromFileName(path);
  if (!image_data) {
    return false;
  }

  std::unique_ptr<SkImageGenerator> gen(SkImageGenerator::MakeFromEncoded(std::move(image_data)));
  if (!gen) {
    return false;
  }
   
  if (!m_bitmap.tryAllocPixels(gen->getInfo())) {
    return false;
  }
  
  gen->getPixels(
    gen->getInfo().makeColorSpace(nullptr), 
    m_bitmap.getPixels(), 
    m_bitmap.rowBytes());

  return true;
}

bool SkiaRenderBitmap::LoadFromData(unsigned char *pData, unsigned int nSize,
                                    RENDER_BITMAP_LOAD_FLAG e) {
  return false;
}

IMAGE_ITEM_TYPE SkiaRenderBitmap::GetImageType() {
  return IMAGE_ITEM_TYPE_IMAGE;
}

bool SkiaRenderBitmap::Create(int nWidth, int nHeight) { 
  // m_bitmap->Create();
  return false; 
}

void SkiaRenderBitmap::Destroy() { return m_bitmap.reset(); }
int SkiaRenderBitmap::GetWidth() { return m_bitmap.width(); }
int SkiaRenderBitmap::GetHeight() { return m_bitmap.height(); }
int SkiaRenderBitmap::GetBPP() { return m_bitmap.bytesPerPixel(); }

} // namespace ui