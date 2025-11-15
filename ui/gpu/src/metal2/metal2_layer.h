#ifndef _UI_GPU_SRC_METAL2_METAL2_LAYER_H_
#define _UI_GPU_SRC_METAL2_METAL2_LAYER_H_

#include "src/gpu_layer.h"
#include "src/metal2/inc.h"
#include "src/texture_tile.h"

namespace ui {
class Metal2GpuLayerTile : public TextureTile {

  void Upload(ui::Rect &dirty_of_tile, ui::Rect &dirty_of_layer,
              ui::GpuUploadBitmap &source) override {}
  void Compositor(long xOffset, long yOffset, long vertexStartIndex,
                  ui::GpuLayerCommitContext *pContext) override {}
};

class Metal2GpuLayer : public GpuLayer {
public:
  void Resize(int nWidth, int nHeight) override {}
  void Compositor(GpuLayerCommitContext *pContext,
                  float *pMatrixTransform) override {}
  TextureTile *newTile() override { return new Metal2GpuLayerTile(); }
};
} // namespace ui

#endif