#pragma once
#include "compositor.h"

namespace ui
{
class GpuLayerCommitContext;
struct IHardwareComposition;

class HardwareCompositor : public Compositor
{
public:
    HardwareCompositor();
    ~HardwareCompositor();

    virtual void  virtualBindHWND(HWND) override;

    virtual void  UpdateDirty(RectRegion* outArrDirtyInWindow) override;
    virtual void  virtualCommit(const RectRegion& arrDirtyInWindow) override;
    virtual void  Resize(uint nWidth, uint nSize) override;

    virtual Layer*  virtualCreateLayer() override;

    IGpuRenderLayer*  CreateGpuLayerTexture();

private:
    void  draw_full_recursion(Layer* p);
    void  update_dirty_recursion(Layer* p);
    void  commit_recursion(Layer* p, GpuLayerCommitContext* pContext);

private:
    // 窗口交换链
    IHardwareComposition*  m_pHardwareComposition;
};

}