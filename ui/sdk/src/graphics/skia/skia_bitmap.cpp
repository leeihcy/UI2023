#include "src/graphics/skia/skia_bitmap.h"
#include "include/core/SkShader.h"
#include "third_party/skia/src/include/core/SkColorSpace.h"
#include "third_party/skia/src/include/core/SkImageGenerator.h"

namespace ui {

std::shared_ptr<IRenderBitmap> SkiaRenderBitmap::CreateInstance() {
  return std::shared_ptr<IRenderBitmap>(new SkiaRenderBitmap());
}

SkiaRenderBitmap::~SkiaRenderBitmap() { SkiaRenderBitmap::Destroy(); }

eGraphicsLibraryType SkiaRenderBitmap::GetGraphicsRenderLibraryType() {
  return eGraphicsLibraryType::Skia;
}

bool SkiaRenderBitmap::LoadFromFile(const char *path) {
  sk_sp<SkData> image_data = SkData::MakeFromFileName(path);
  return loadBySkData(image_data);
}

bool SkiaRenderBitmap::LoadFromData(IBufferData *data) {
  sk_sp<SkData> image_data =
      SkData::MakeWithoutCopy(data->Data(), data->Size());

  bool ret = loadBySkData(image_data);

  // 需要保留缓存数据引用
  if (m_buffer) {
    m_buffer->Release();
  }
  m_buffer = data;
  m_buffer->AddRef();

  return ret;
}

bool SkiaRenderBitmap::loadBySkData(sk_sp<SkData> image_data) {
  if (!image_data) {
    return false;
  }

  std::unique_ptr<SkImageGenerator> gen(
      SkImageGenerator::MakeFromEncoded(std::move(image_data)));
  if (!gen) {
    return false;
  }

  SkBitmap bitmap;
  if (!bitmap.tryAllocPixels(gen->getInfo())) {
    return false;
  }

  gen->getPixels(gen->getInfo().makeColorSpace(nullptr), bitmap.getPixels(),
                 bitmap.rowBytes());

  // m_image = SkImage::MakeFromBitmap(bitmap);
  bitmap.setImmutable();
  m_image = bitmap.asImage();
  return true;
}

eImageType SkiaRenderBitmap::GetImageType() {
  return eImageType::Image;
}

bool SkiaRenderBitmap::Create(int nWidth, int nHeight) {
  // m_bitmap->Create();
  return false;
}

void SkiaRenderBitmap::Destroy() {
  m_image.reset();
  if (m_buffer) {
    m_buffer->Release();
    m_buffer = nullptr;
  }
}
int SkiaRenderBitmap::GetWidth() { return m_image->width(); }
int SkiaRenderBitmap::GetHeight() { return m_image->height(); }
int SkiaRenderBitmap::GetBPP() { return m_image->imageInfo().bytesPerPixel(); }

void SkiaRenderBitmap::Dump(const char *path) {
#if 0 // defined (_DEBUG)
  sk_sp<SkData> png(SkImage::MakeFromBitmap(m_bitmap)->encodeToData());
  if (!png) {
    return;
  }
  
  SkFILEWStream out(path);
  (void)out.write(png->data(), png->size());
#endif
}

std::shared_ptr<IRenderBitmap> SkiaRenderGif::CreateInstance() {
  return std::shared_ptr<IRenderBitmap>(new SkiaRenderGif());
}

bool SkiaRenderGif::loadBySkData(sk_sp<SkData> data) {
  std::unique_ptr<SkCodec> codec;
  codec = SkCodec::MakeFromData(data);

  // 获取帧信息
  m_frame_count = codec->getFrameCount();
  if (m_frame_count <= 0) {
    return false;
  }

  m_frame_infos.resize(m_frame_count);
  m_frames.reserve(m_frame_count);

  // 解码每一帧
  for (int i = 0; i < m_frame_count; i++) {
    SkBitmap bitmap;
    bitmap.allocPixels(codec->getInfo());

    SkCodec::Options options;
    options.fFrameIndex = i;
    options.fPriorFrame = SkCodec::kNoFrame;

    codec->getPixels(bitmap.info(), bitmap.getPixels(), bitmap.rowBytes(),
                       &options);
    bitmap.setImmutable();
    m_frames.push_back(bitmap.asImage());

    codec->getFrameInfo(i, &m_frame_infos[i]);
    // 将默认帧延时设置为 0.1 秒。
    if (m_frame_infos[i].fDuration <= 0) {
      m_frame_infos[i].fDuration = 100;
    }
  }

  // 使用父类的m_image来指向当前帧。
  m_image = m_frames[0];
  m_last_frame_time = std::chrono::steady_clock::now();
  return true;
}

bool SkiaRenderGif::Tick() {
  auto now = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                     now - m_last_frame_time)
                     .count();

  int diff = elapsed - m_frame_infos[m_current_frame].fDuration;
  if (diff > 0) {
    advanceFrame();

    // 补上误差。
    // 但最大误差限制在1帧(16ms)以内，避免拖拽窗口时阻塞UI后积累了很大的误差。
    m_last_frame_time = now - std::chrono::milliseconds(std::min(16, diff));
    return true;
  }
  return false;
}

void SkiaRenderGif::advanceFrame() {
  m_current_frame = (m_current_frame + 1) % m_frame_count;
  m_image = m_frames[m_current_frame];
}

} // namespace ui