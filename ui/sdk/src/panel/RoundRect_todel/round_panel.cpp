#include "round_panel.h"
// #include "src/Util\RoundRect\roundrect.h"
#include "include/core/SkPath.h"
#include "include/core/SkRasterHandleAllocator.h"
#include "include/core/SkRect.h"
#include "include/macro/msg.h"
#include "src/attribute/attribute.h"
#include "src/graphics/skia/skia_render.h"
#include "src/panel/panel_meta.h"
#include <SkCanvas.h>
#include <SkImage.h>
#include <SkRRect.h>
#include <SkClipOp.h>
#include "RoundRect/roundrect.h"

namespace ui {

static RoundRectHandler s_RoundRectHandler;

RoundPanel::RoundPanel(IRoundPanel *p) : Panel(p) {
  m_lefttop = 0;
  m_righttop = 0;
  m_leftbottom = 0;
  m_rightbottom = 0;
  m_pIRoundPanel = p;

  m_objStyle.post_paint = 1;
  s_parentBuf.AddRef();
}
RoundPanel::~RoundPanel() { 
  s_parentBuf.Release(); 
}

void RoundPanel::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_PAINT) {
    onPaint(static_cast<PaintMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_PAINTBKGND) {
    onPaintBkgnd(static_cast<PaintMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_POSTPAINT) {
    Panel::onRouteMessage(msg);
    onPostPaint(static_cast<PostPaintMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto *m = static_cast<QueryInterfaceMessage *>(msg);
    if (m->uuid == RoundPanelMeta::Get().UUID()) {
      *(m->pp) = m_pIRoundPanel;
      return;
    }
  } else if (msg->message == UI_MSG_SERIALIZE) {
    auto *m = static_cast<SerializeMessage *>(msg);
    onSerialize(m->param);
    return;
  }
  Panel::onRouteMessage(msg);
}

#define XML_CORNER "corner"
#define XML_ROUNDPANEL_CORNER_LEFTTOP        "corner.lefttop"
#define XML_ROUNDPANEL_CORNER_RIGHTTOP       "corner.righttop"
#define XML_ROUNDPANEL_CORNER_LEFTBOTTOM     "corner.leftbottom"
#define XML_ROUNDPANEL_CORNER_RIGHTBOTTOM    "corner.rightbottom"

void RoundPanel::onSerialize(SerializeParam *pData) {
  Panel::onSerialize(pData);

  AttributeSerializer s(pData, "RoundPanel");
  s.AddInt(XML_CORNER, Slot(&RoundPanel::loadCorner, this),
           Slot(&RoundPanel::saveCorner, this));

  // TODO: XML_CORNER的值会被0覆盖掉。
  // s.AddInt(XML_ROUNDPANEL_CORNER_LEFTTOP, *(int *)&m_lefttop);
  // s.AddInt(XML_ROUNDPANEL_CORNER_RIGHTTOP, *(int *)&m_righttop);
  // s.AddInt(XML_ROUNDPANEL_CORNER_LEFTBOTTOM, *(int *)&m_leftbottom);
  // s.AddInt(XML_ROUNDPANEL_CORNER_RIGHTBOTTOM, *(int *)&m_rightbottom);
}

void RoundPanel::onPaintBkgnd(IRenderTarget *pRenderTarget) {
  // 背景不进行剪裁。
  Panel::onPaintBkgnd(pRenderTarget);
}
void RoundPanel::onPaint(IRenderTarget *pRenderTarget) {
  prePaint(pRenderTarget, GetWidth(), GetHeight());
  Panel::onPaint(pRenderTarget);
}

void RoundPanel::onPostPaint(IRenderTarget *pRenderTarget) {
  postPaint(pRenderTarget, GetWidth(), GetHeight());
}

void RoundPanel::prePaint(IRenderTarget *pRenderTarget, int width, int height) {
  if (m_lefttop == 0 && m_righttop == 0 && m_leftbottom == 0 &&
      m_rightbottom == 0)
    return;

#if 0
  if (m_lefttop > 0) {
    s_parentBuf.SaveLeftTop(pRenderTarget, m_lefttop);
  }
  if (m_righttop > 0) {
    s_parentBuf.SaveRightTop(pRenderTarget, m_righttop, width);
  }
  if (m_leftbottom > 0) {
    s_parentBuf.SaveLeftBottom(pRenderTarget, m_leftbottom, height);
  }
  if (m_rightbottom > 0) {
    s_parentBuf.SaveRightBottom(pRenderTarget, m_rightbottom, width, height);
  }
#endif
  SkCanvas* canvas = (SkCanvas*)pRenderTarget->GetHandle();
  canvas->save();

  SkPath path;
  
  SkRRect rr;
  rr.setNinePatch(SkRect::MakeXYWH(0, 0, width, height), 
    m_lefttop, m_righttop, m_leftbottom, m_rightbottom);

  path.addRRect(rr);
  canvas->clipPath(path, SkClipOp::kIntersect, true);
}


void RoundPanel::SetRadius(int lefttop, int righttop, int leftbottom,
                           int rightbottom) {
  m_lefttop = lefttop;
  m_righttop = righttop;
  m_leftbottom = leftbottom;
  m_rightbottom = rightbottom;
}


void RoundPanel::loadCorner(int n) {
  if (n <= 0) {
    return;
  }
  m_lefttop = m_leftbottom = m_righttop = m_rightbottom = n;
}
int RoundPanel::saveCorner() {
  return m_lefttop;
}

void RoundPanel::postPaint(IRenderTarget *pRenderTarget, int width, int height) {
  if (m_lefttop == 0 && m_righttop == 0 && m_leftbottom == 0 &&
      m_rightbottom == 0)
    return;

  SkCanvas* canvas = (SkCanvas*)pRenderTarget->GetHandle();
  canvas->restore();

    // 1. 圆角化新背景
#if 0
  HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
  RECT rcPanel = {0, 0, width, height};
  POINT ptOffset;
  ::GetViewportOrgEx(hDC, &ptOffset);
  OffsetRect(&rcPanel, ptOffset.x, ptOffset.y);

    // 直接change bitmap bit无视了剪裁区域，而后面的还原用的bitblt又受
    // 剪裁区影响，因此这里需要更进一步的限制一下，不在剪裁区域内的，不
    // 能调用Apply2Bitmap
  RECT rcClip;
  GetClipBox(hDC, &rcClip);
  ::OffsetRect(&rcClip, ptOffset.x, ptOffset.y);

  // TODO:
  s_RoundRectHandler.Set(m_lefttop, m_righttop, m_rightbottom, m_leftbottom);
  s_RoundRectHandler.Apply2Bitmap(hBitmap, true, &rcPanel, &rcClip);
    // 2. 将圆角图画到保存的背景上去
    // 3. 还原背景
  if (m_lefttop) {
    s_parentBuf.UpdateLeftTop(hDC, m_lefttop);
    s_parentBuf.RestoreLeftTop(hDC, m_lefttop);
  }
  if (m_righttop) {
    s_parentBuf.UpdateRightTop(hDC, m_righttop, width);
    s_parentBuf.RestoreRightTop(hDC, m_righttop, width);
  }
  if (m_leftbottom) {
    s_parentBuf.UpdateLeftBottom(hDC, m_leftbottom, height);
    s_parentBuf.RestoreLeftBottom(hDC, m_leftbottom, height);
  }
  if (m_rightbottom) {
    s_parentBuf.UpdateRightBottom(hDC, m_rightbottom, width, height);
    s_parentBuf.RestoreRightBottom(hDC, m_rightbottom, width, height);
  }
  #endif
}

//////////////////////////////////////////////////////////////////////////

BufferSwitcher::BufferSwitcher() { m_lRef = 0; }
BufferSwitcher::~BufferSwitcher() {}

void BufferSwitcher::AddRef() { m_lRef++; }
void BufferSwitcher::Release() {
  --m_lRef;

  if (0 == m_lRef) {
#if 0
    m_lefttop.Destroy();
    m_righttop.Destroy();
    m_leftbottom.Destroy();
    m_rightbottom.Destroy();
#endif
  }
}


void BufferSwitcher::SaveLeftTop(IRenderTarget *pRenderTarget, uint radius) {
  save(m_lefttop, pRenderTarget, radius, 0, 0);
}

void BufferSwitcher::SaveRightTop(IRenderTarget *pRenderTarget, uint radius, int width) {
  save(m_righttop, pRenderTarget, radius, width - radius, 0);
}

void BufferSwitcher::SaveLeftBottom(IRenderTarget *pRenderTarget, uint radius, int height) {
  save(m_leftbottom, pRenderTarget, radius, 0, height - radius);
}

void BufferSwitcher::SaveRightBottom(IRenderTarget *pRenderTarget, uint radius, int width,
                                     int height) {
  save(m_rightbottom, pRenderTarget, radius, width - radius, height - radius);
}

void BufferSwitcher::UpdateLeftTop(IRenderTarget *pRenderTarget, uint radius) {
  update(m_lefttop, pRenderTarget, radius, 0, 0);
}

void BufferSwitcher::UpdateRightTop(IRenderTarget *pRenderTarget, uint radius, int width) {
  update(m_righttop, pRenderTarget, radius, width - radius, 0);
}

void BufferSwitcher::UpdateLeftBottom(IRenderTarget *pRenderTarget, uint radius, int height) {
  update(m_leftbottom, pRenderTarget, radius, 0, height - radius);
}

void BufferSwitcher::UpdateRightBottom(IRenderTarget *pRenderTarget, uint radius, int width,
                                       int height) {
  update(m_rightbottom, pRenderTarget, radius, width - radius, height - radius);
}
#if 0
void BufferSwitcher::RestoreLeftTop(IRenderTarget *pRenderTarget, uint radius) {
  restore(m_lefttop, pRenderTarget, radius, 0, 0);
}

void BufferSwitcher::RestoreRightTop(IRenderTarget *pRenderTarget, uint radius, int width) {
  restore(m_righttop, pRenderTarget, radius, width - radius, 0);
}
void BufferSwitcher::RestoreLeftBottom(IRenderTarget *pRenderTarget, uint radius, int height) {
  restore(m_leftbottom, pRenderTarget, radius, 0, height - radius);
}
void BufferSwitcher::RestoreRightBottom(IRenderTarget *pRenderTarget, uint radius, int width,
                                        int height) {
  restore(m_rightbottom, pRenderTarget, radius, width - radius, height - radius);
}
#endif

void BufferSwitcher::save(SkiaBitmap &image, IRenderTarget *rt,
                          uint radius, int x, int y) {
  if (image.IsNull() || image.Width() < (int)radius ||
      image.Height() < (int)radius) {
    image.Destroy();
    image.Create(radius, radius);
  }


  SkCanvas canvas(image.m_skbitmap);
  
  SkSamplingOptions options;
  SkPaint paint;


  // SkCanvas* target_canvas = (SkCanvas *)rt->GetHandle();
  SkSurface* surface = static_cast<SkiaRenderTarget*>(rt)->GetSkiaSurface();
  canvas.drawImageRect(
        surface->makeImageSnapshot().get(),
        SkRect::MakeXYWH(x, y, radius, radius),
        SkRect::MakeXYWH(0, 0, radius, radius),
        options, &paint, SkCanvas::kFast_SrcRectConstraint);

// #if defined(_DEBUG)
//   rt->Save("/tmp/uisdk/test2.png");
// #endif
  // HDC hDestDC = image.GetDC();
  // ::BitBlt(hDestDC, 0, 0, radius, radius, hDC, x, y, SRCCOPY);
  // image.ReleaseDC();

  // image.Dump("/tmp/uisdk/test.png");
}


void BufferSwitcher::update(SkiaBitmap &image, IRenderTarget *pRenderTarget, uint radius, int x,
                            int y) {
  if (image.IsNull())
    return;
#if 0
  HDC hDestDC = image.GetDC();
  BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
  ::AlphaBlend(hDestDC, 0, 0, radius, radius, hDC, x, y, radius, radius, bf);
  image.ReleaseDC();
#endif
}

void BufferSwitcher::restore(SkiaBitmap &image, IRenderTarget *pRenderTarget, uint radius, int x,
                             int y) {
  if (image.IsNull())
    return;
#if 0
  image.BitBlt(hDC, x, y, radius, radius, 0, 0, SRCCOPY);
  // image.Draw(hDC, x, y, radius, radius);
#endif
}

//////////////////////////////////////////////////////////////////////////

} // namespace ui