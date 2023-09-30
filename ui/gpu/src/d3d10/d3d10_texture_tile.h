#pragma once
#include "src/texture_tile.h"
#include "gpu/include/api.h"
#include <d3d10.h>
struct SOURCE_BITMAP_DATA;

namespace ui {
class GpuLayerCommitContext;
class Hard3DTransform;
} // namespace ui


class D3D10TextureTile : public TextureTile{
public:
  D3D10TextureTile();
  ~D3D10TextureTile();

  void Upload(ui::Rect &rcSrc, ui::UploadGpuBitmapInfo &source);
  void Compositor(long xOffset, long yOffset, long vertexStartIndex,
                  ui::GpuLayerCommitContext *pContext);

private:
  bool create();

private:
  ID3D10Texture2D *m_pTextureBuffer;
  ID3D10ShaderResourceView *m_pShaderResourceView;
};
