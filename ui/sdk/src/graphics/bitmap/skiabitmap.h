#ifndef _UI_SDK_SRC_GRAPHICS_BITMAP_SKIABITMAP_H_
#define _UI_SDK_SRC_GRAPHICS_BITMAP_SKIABITMAP_H_

#include "ui/sdk/src/graphics/bitmap/bitmap.h"
#include "third_party/skia/src/include/core/SkBitmap.h"

namespace ui {

class SkiaBitmap : public IBitmap32 {
public:
  SkiaBitmap() {}
  ~SkiaBitmap() {}
  
  bool IsNull() override;
  void Create(int width, int height) override;
  void Destroy() override;
  int Width() override;
  int Height() override;

public:
  void Dump(const char* path);

public:
  SkBitmap m_skbitmap;
};

}
#endif