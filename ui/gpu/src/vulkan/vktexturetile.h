#ifndef _UI_GPU_SRC_VULKAN_TEXTURETILE_H_
#define _UI_GPU_SRC_VULKAN_TEXTURETILE_H_
#include "src/texture_tile.h"

namespace ui {

class VkTextureTile : public TextureTile {
protected:
  void Upload(ui::Rect &rcSrc, ui::UploadGpuBitmapInfo &source) override;
};

}

#endif