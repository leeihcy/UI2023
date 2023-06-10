#pragma once
#include "layer.h"
#if 1//ENABLE_HARDCOMPOSITION
namespace UI
{
class GpuLayerCommitContext;

class HardwareLayer : public Layer
{
public:
    HardwareLayer();
    ~HardwareLayer();

    void  Commit(GpuLayerCommitContext* pContext);

    virtual LayerType  GetType() override { return Layer_Hardware; }
    virtual void  UpdateDirty() override;
    virtual void  virtualOnSize(uint nWidth, uint nHeight) override;
    virtual void  MapView2Layer(POINT* pPoint);

protected:
	void  draw_layer();
	void  upload_2_gpu();

    //IRenderTarget*  GetRenderTarget();

protected:

    // Ó²¼þºÏ³É
    IGpuRenderLayer*  m_pGpuTexture;
};

}
#endif