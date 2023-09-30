#ifndef _UI_GPU_SRC_TEXTURE_TILE_H_
#define _UI_GPU_SRC_TEXTURE_TILE_H_
#include "sdk/include/common/math/rect.h"
#include "gpu/include/api.h"

namespace ui {

class TextureTile {
public:
  virtual ~TextureTile() {}
  
  void SetIndex(int xIndex, int yIndex);

  virtual void Upload(ui::Rect &rcSrc, ui::UploadGpuBitmapInfo &source) = 0;

protected:
  long m_width = 0;
  long m_height = 0;
  long m_xIndex = 0;
  long m_yIndex = 0;
};

}
#endif