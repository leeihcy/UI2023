#include "sdk/src/graphics/bitmap/skiabitmap.h"
#include "SkStream.h"
#include "SkImage.h"

namespace ui {

bool SkiaBitmap::IsNull() {
  return m_skbitmap.isNull();
}

void SkiaBitmap::Create(int width, int height) {
  m_skbitmap.allocN32Pixels(width, height);
}

void SkiaBitmap::Destroy() {
  m_skbitmap.reset();
}

int SkiaBitmap::Width() {
  return m_skbitmap.width();
}
int SkiaBitmap::Height() {
  return m_skbitmap.height();
}

void SkiaBitmap::Dump(const char* path) {
#if defined (_DEBUG)
  sk_sp<SkData> png(SkImage::MakeFromBitmap(m_skbitmap)->encodeToData());
  if (!png) {
    return;
  }
  
  SkFILEWStream out(path);
  (void)out.write(png->data(), png->size());
#endif
} 

}