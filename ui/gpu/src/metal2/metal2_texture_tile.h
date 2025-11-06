#ifndef _UI_GPU_SRC_METAL2_TEXTURETILE_H_
#define _UI_GPU_SRC_METAL2_TEXTURETILE_H_
#include "src/texture_tile.h"
#include "src/metal2/metal2_bridge.h"

namespace ui {

class Metal2TextureTile : public TextureTile {
public:
  Metal2TextureTile(IMetal2Bridge& bridge);
  ~Metal2TextureTile();

protected:
  void Upload(ui::Rect &rcSrc, ui::UploadGpuBitmapInfo &source) override;
  void Compositor(long, long, long vertexStartIndex,
                  ui::GpuLayerCommitContext *pContext) override;
private:
  bool create();
  
private:
  IMetal2Bridge& m_bridge;
};

} // namespace ui

#endif