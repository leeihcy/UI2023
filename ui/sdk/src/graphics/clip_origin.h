#ifndef _UI_SDK_SRD_GRAPHICS_CLIP_ORIGIN_H_
#define _UI_SDK_SRD_GRAPHICS_CLIP_ORIGIN_H_
#include "include/interface/graphics.h"
#include "include/util/rect.h"
#include <vector>
#include "include/util/rect_region.h"

namespace ui {

class ClipOriginImpl : public IClipOrigin {
public:
  ClipOriginImpl();
  
  void SetDirtyRegion(const DirtyRegion& dirty_region) override;
  const DirtyRegion& GetDirtyRegion() { return m_dirty_region; }
  void PushRelativeClipRect(const Rect& rect) override;
  void PopRelativeClipRect() override;

  void SetOrigin(int x, int y) override;
  void OffsetOrigin(int x, int y) override;
  void GetOrigin(int& x, int& y);

  bool IsRelativeRectInClip(const Rect& rect) override;

public:
  // 剪裁数据
  DirtyRegion m_dirty_region;
  // 需要遍历，不使用std::stack
  std::vector<Rect> m_stackClipRect;
  Point m_ptOffset;

};

} // namespace ui

#endif // _UI_SDK_SRD_GRAPHICS_CLIP_ORIGIN_H_