#include "src/graphics/clip_origin.h"

namespace ui {

  ClipOriginImpl::ClipOriginImpl() {
    m_ptOffset.x = m_ptOffset.y = 0;
  }

void ClipOriginImpl::SetDirtyRegion(const DirtyRegion& dirty_region) {
  // while (!m_stackClipRect.empty())
  //   m_stackClipRect.pop();
  m_stackClipRect.clear();
  m_dirty_region = dirty_region;
}

void ClipOriginImpl::PushRelativeClipRect(const Rect &rect) {
  Rect rc = rect;
  rc.Offset(m_ptOffset.x, m_ptOffset.y);
  m_stackClipRect.push_back(rc);
}
void ClipOriginImpl::PopRelativeClipRect() {
  assert(!m_stackClipRect.empty());
  m_stackClipRect.pop_back();
}

void ClipOriginImpl::GetOrigin(int &x, int &y) {
  x = m_ptOffset.x;
  y = m_ptOffset.y;
}
void ClipOriginImpl::SetOrigin(int x, int y) {
  m_ptOffset.x = x;
  m_ptOffset.y = y;
}
void ClipOriginImpl::OffsetOrigin(int x, int y) {
  m_ptOffset.x += x;
  m_ptOffset.y += y;
}

bool ClipOriginImpl::IsRelativeRectInClip(const Rect &rect) {
  if (m_stackClipRect.empty() && m_dirty_region.Count() == 0)
    return true;

  Rect rcTest = rect;
  rcTest.Offset(m_ptOffset.x, m_ptOffset.y);

  if (!m_dirty_region.IntersectRect(&rcTest, true))
    return false;

  if (m_stackClipRect.empty())
    return true;

  Rect rcIntersect = {0, 0, 1, 1};

  auto iter = m_stackClipRect.begin();
  rcIntersect.CopyFrom(*iter);
  iter++;
  for (; iter != m_stackClipRect.end(); ++iter) {
    rcIntersect.Intersect((*iter), &rcIntersect);
  }

  if (!rcIntersect.Intersect(rcTest, &rcIntersect))
    return false;

  return true;
}

} // namespace ui