#pragma once
#include "gpu/include/api.h"
#include "src/texture_tile.h"
#include "src/d3d11/inc.h"
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

struct SOURCE_BITMAP_DATA;

namespace ui {
class GpuLayerCommitContext;
class Hard3DTransform;

class D3D11TextureTile : public TextureTile {
public:
  D3D11TextureTile();
  ~D3D11TextureTile();

  void OnBeginCommit(GpuLayerCommitContext *ctx) override {}
  void Upload(ui::Rect &rcSrc, ui::UploadGpuBitmapInfo &source);
  void Compositor(long xOffset, long yOffset, long vertexStartIndex,
                  ui::GpuLayerCommitContext *pContext);

private:
  bool create();

private:
  ID3D11Texture2D *m_pTextureBuffer;
  ID3D11ShaderResourceView *m_pShaderResourceView;
};

} // namespace ui