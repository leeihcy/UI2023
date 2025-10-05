#pragma once
#include "compositor.h"

namespace ui
{
class GpuLayerCommitContext;
struct IGpuCompositor;

class HardwareCompositor : public Compositor
{
public:
    HardwareCompositor();
    ~HardwareCompositor();
    virtual Layer*  createLayerObject() override;
    virtual void  onBindWindow(Window* w) override;
    virtual void  doCommit(const RectRegion& arrDirtyInWindow) override;
    virtual bool  UpdateDirty(RectRegion* outArrDirtyInWindow) override;
    virtual void  Resize(uint nWidth, uint nSize) override;

private:
    void  draw_full_recursion(Layer* p);
    void  update_dirty_recursion(Layer* p);
    void  commit_recursion(Layer* p, GpuLayerCommitContext* pContext);

private:
    // 窗口交换链
    IGpuCompositor*  m_gpu_composition = nullptr;
};

}