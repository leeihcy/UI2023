#pragma once
#include "compositor.h"
#if 1//ENABLE_HARDCOMPOSITION
namespace UI
{
class GpuLayerCommitContext;
struct IHardwareComposition;

class HardwareCompositor : public Compositor
{
public:
    HardwareCompositor();
    ~HardwareCompositor();

    virtual void  virtualBindHWND(HWND) override;

    virtual void  UpdateDirty(__out_opt  RectArray& arrDirtyInWindow) override;
    virtual void  virtualCommit(const RectArray& arrDirtyInWindow) override;
    virtual void  Resize(uint nWidth, uint nSize) override;

    virtual Layer*  virtualCreateLayer() override;

    IGpuRenderLayer*  CreateGpuLayerTexture();

private:
    void  draw_full_recursion(Layer* p);
    void  update_dirty_recursion(Layer* p);
    void  commit_recursion(Layer* p, GpuLayerCommitContext* pContext);

private:
    // ´°¿Ú½»»»Á´
    UI::IHardwareComposition*  m_pHardwareComposition;
};

}
#endif