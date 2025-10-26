#include "src/graphics/skia/skia_bitmap.h"
#include "third_party/skia/src/include/core/SkColorSpace.h"
#include "third_party/skia/src/include/core/SkData.h"
#include "third_party/skia/src/include/core/SkImageGenerator.h"
#include <cstddef>

namespace ui {

std::shared_ptr<IRenderBitmap> SkiaRenderBitmap::CreateInstance() {
  return std::shared_ptr<IRenderBitmap>(new SkiaRenderBitmap());
}

SkiaRenderBitmap::~SkiaRenderBitmap() {
  SkiaRenderBitmap::Destroy();
}

eGraphicsLibraryType SkiaRenderBitmap::GetGraphicsRenderLibraryType() {
  return eGraphicsLibraryType::Skia;
}

bool SkiaRenderBitmap::LoadFromFile(const char *path) {
  sk_sp<SkData> image_data = SkData::MakeFromFileName(path);
  if (!image_data) {
    return false;
  }

  std::unique_ptr<SkImageGenerator> gen(
      SkImageGenerator::MakeFromEncoded(std::move(image_data)));
  if (!gen) {
    return false;
  }

  if (!m_bitmap.tryAllocPixels(gen->getInfo())) {
    return false;
  }

  gen->getPixels(gen->getInfo().makeColorSpace(nullptr), m_bitmap.getPixels(),
                 m_bitmap.rowBytes());
  return true;
}

bool SkiaRenderBitmap::LoadFromData(IBufferData* data) {
  // TODO: 是否能不做Copy，节省一次数据拷贝
  sk_sp<SkData> image_data = SkData::MakeWithoutCopy(data->Data(), data->Size()); 
  if (!image_data) {
    return false;
  }

  std::unique_ptr<SkImageGenerator> gen(
      SkImageGenerator::MakeFromEncoded(std::move(image_data)));
  if (!gen) {
    return false;
  }

  if (!m_bitmap.tryAllocPixels(gen->getInfo())) {
    return false;
  }

  gen->getPixels(gen->getInfo().makeColorSpace(nullptr), m_bitmap.getPixels(),
                 m_bitmap.rowBytes());

  // 需要保留缓存数据引用
  m_buffer = data;
  m_buffer->AddRef();
  return true;
}

eImageItemType SkiaRenderBitmap::GetImageType() {
  return eImageItemType::Image;
}

bool SkiaRenderBitmap::Create(int nWidth, int nHeight) {
  // m_bitmap->Create();
  return false;
}

void SkiaRenderBitmap::Destroy() { 
  m_bitmap.reset(); 
  if (m_buffer) {
    m_buffer->Release();
    m_buffer = nullptr;
  }
}
int SkiaRenderBitmap::GetWidth() { return m_bitmap.width(); }
int SkiaRenderBitmap::GetHeight() { return m_bitmap.height(); }
int SkiaRenderBitmap::GetBPP() { return m_bitmap.bytesPerPixel(); }

} // namespace ui