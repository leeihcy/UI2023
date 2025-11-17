#ifndef _UI_GPU_SRC_METAL2_TEXTURETILE_H_
#define _UI_GPU_SRC_METAL2_TEXTURETILE_H_
#include "src/metal2/metal2_bridge.h"
#include "src/texture_tile.h"

namespace ui {

class Metal2TextureTile : public TextureTile {
public:
  Metal2TextureTile(IMetal2Bridge &bridge);
  ~Metal2TextureTile();

  void Compositor(id<MTLBuffer> index_buffer, long vertexStartIndex,
                  ui::GpuLayerCommitContext *pContext);

protected:
  void Upload(ui::Rect &dirty_of_tile, ui::Rect &dirty_of_layer,
              ui::GpuUploadBitmap &source) override;

private:
  bool create();

private:
  IMetal2Bridge &m_bridge;

  id<MTLTexture> m_texture = nil;
  MTLTextureDescriptor *m_texture_descriptor = nullptr;
};

} // namespace ui

#endif