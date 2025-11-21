#ifndef _UI_GPU_SRC_METAL2_METAL2_LAYER_H_
#define _UI_GPU_SRC_METAL2_METAL2_LAYER_H_

#include "src/gpu_layer.h"
#include "src/metal2/inc.h"
#include "src/metal2/metal2_bridge.h"
#include "src/texture_tile.h"
#include <objc/objc.h>

namespace ui {
class TextureTile;

class Metal2GpuLayer : public GpuLayer {
public:
  Metal2GpuLayer(IMetal2Bridge& bridge);
  ~Metal2GpuLayer();

  void Resize(int nWidth, int nHeight) override;
  void Compositor(GpuLayerCommitContext *pContext,
                  float *pMatrixTransform) override;
  TextureTile *newTile() override;
  void UpdateTileBitmap(int row, int col, ui::Rect &dirty_of_tile,
                        ui::Rect &dirty_of_layer,
                        ui::GpuUploadBitmap &source) override;

private:
  void createVertexBuffer();
  void createTileBuffer();
  void createTextures();
private:
  IMetal2Bridge& m_bridge;

  id <MTLBuffer>  m_vertices_buffer = nil;
  id <MTLBuffer>  m_tiles_buffer = nil;
  id <MTLTexture> m_texture_arr = nil;
};
} // namespace ui

#endif