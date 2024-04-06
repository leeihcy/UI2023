#ifndef _UI_SDK_SRC_GRAPHICS_BITMAP_H_
#define _UI_SDK_SRC_GRAPHICS_BITMAP_H_

namespace ui {

// 封装各平台的bitmap，如 atl image, skia bitmap
// 这里只使用是32位位图格式
struct IBitmap32 {
  virtual bool IsNull() = 0;
  virtual void Create(int width, int height) = 0;
  virtual void Destroy() = 0;
  virtual int Width() = 0;
  virtual int Height() = 0;
};

}

#endif