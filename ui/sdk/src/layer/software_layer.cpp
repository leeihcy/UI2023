#include "software_layer.h"
#include "compositor.h"
#include "include/interface/renderlibrary.h"
#include "src/application/config/config.h"

namespace ui {
SoftwareLayer::SoftwareLayer() {}

SoftwareLayer::~SoftwareLayer() {}

bool SoftwareLayer::UpdateDirty() {
  if (!m_pLayerContent)
    return false;

  if (!m_dirty_region.Count())
    return false;

  IRenderTarget *pRenderTarget = GetRenderTarget();

  float scale = m_pLayerContent->GetLayerScale();
  pRenderTarget->BeginDraw(scale);
  pRenderTarget->SetDirtyRegion(m_dirty_region);

  // 先begin draw，设置好缩放比例，再clear，否则clear区域不正确。
  if (m_need_clear_background) {
    uint count = m_dirty_region.Count();
    for (uint i = 0; i < count; i++)
      pRenderTarget->Clear(*m_dirty_region.GetRectPtrAt(i));
  }

  // 立即销毁无效区域，避免在Draw中再次触发Invalidate逻辑后，dirtyrect又被清空
  // 例如listitem.draw->listitem.delayop->listitem.onsize->invalidate
  m_dirty_region.Destroy();

  m_pLayerContent->Draw(pRenderTarget);
  pRenderTarget->EndDraw();

  if (Config::GetInstance().debug.dump_render_target) {
    static int i = 0;
    char path[64];
#if defined(OS_WIN)
    sprintf(path, "D:\\images\\%p_%d.png", pRenderTarget, i++);
#else
    sprintf(path, "/tmp/images/%p_%d.png", pRenderTarget, i++);
#endif
    pRenderTarget->DumpToImage(path);
  }
  return true;
}

} // namespace ui