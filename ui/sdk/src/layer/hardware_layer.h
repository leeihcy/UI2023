#pragma once
#include "layer.h"

namespace ui {
class GpuLayerCommitContext;
struct IGpuRenderLayer;

class HardwareLayer : public Layer {
public:
  HardwareLayer();
  ~HardwareLayer();

  void Commit(GpuLayerCommitContext *pContext);

  virtual LayerType GetType() override { return Layer_Hardware; }
  virtual void UpdateDirty() override;
  virtual void virtualOnSize(uint nWidth, uint nHeight) override;
  virtual void MapView2Layer(Point *pPoint) override;

protected:
  void draw_layer();
  void upload_2_gpu();

  // IRenderTarget*  GetRenderTarget();

protected:
  // 硬件合成
  IGpuRenderLayer *m_pGpuTexture;
};

} // namespace ui