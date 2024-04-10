#include "round_panel.h"

#include <SkCanvas.h>
#include <SkClipOp.h>
#include <SkImage.h>
#include <SkPath.h>
#include <SkRRect.h>
#include <SkRect.h>

#include "include/macro/msg.h"
#include "src/attribute/attribute.h"
#include "src/graphics/skia/skia_render.h"
#include "src/panel/panel_meta.h"

namespace ui {

RoundPanel::RoundPanel(IRoundPanel *p) : Panel(p) {
  m_pIRoundPanel = p;
  m_corner.SetEmpty();

  m_objStyle.post_paint = 1;
  m_meta = &RoundPanelMeta::Get();
}
RoundPanel::~RoundPanel() {}

void RoundPanel::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_PAINT) {
    onPaint(static_cast<PaintMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_ERASEBKGND) {
    onEraseBkgnd(static_cast<PaintMessage *>(msg)->rt);
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

void RoundPanel::onSerialize(SerializeParam *pData) {
  Panel::onSerialize(pData);

  AttributeSerializer s(pData, "RoundPanel");
  s.AddRect(XML_CORNER, Slot(&RoundPanel::loadCorner, this),
           Slot(&RoundPanel::saveCorner, this));
}

void RoundPanel::SetRadius(int lefttop, int righttop, int leftbottom,
                           int rightbottom) {
  m_corner.Set(lefttop, righttop, leftbottom, rightbottom);
}

void RoundPanel::loadCorner(Rect* rc) {
  if (!rc) {
    m_corner.SetEmpty();
    return;
  }
  m_corner.CopyFrom(*rc);
}
void RoundPanel::saveCorner(Rect* rc) {
  rc->CopyFrom(m_corner);
}

void RoundPanel::onEraseBkgnd(IRenderTarget *pRenderTarget) {
  // 背景不进行剪裁。
  Panel::onEraseBkgnd(pRenderTarget);
}
void RoundPanel::onPaint(IRenderTarget *pRenderTarget) {
  prePaint(pRenderTarget, GetWidth(), GetHeight());
  Panel::onPaint(pRenderTarget);
}

void RoundPanel::onPostPaint(IRenderTarget *pRenderTarget) {
  postPaint(pRenderTarget, GetWidth(), GetHeight());
}

void RoundPanel::prePaint(IRenderTarget *pRenderTarget, int width, int height) {
  if (m_corner.IsZero())
    return;

  SkScalar ul = (SkScalar)m_corner.left;
  SkScalar ur = (SkScalar)m_corner.top;
  SkScalar ll = (SkScalar)m_corner.right;
  SkScalar lr = (SkScalar)m_corner.bottom;

  SkRRect rr;
  SkVector radii[4] = {
      {ul, ul},
      {ur, ur},
      {lr, lr},
      {ll, ll},
  };
  rr.setRectRadii(SkRect::MakeXYWH(0, 0, width, height), radii);

  SkPath path;
  path.addRRect(rr);

  SkCanvas *canvas = (SkCanvas *)pRenderTarget->GetHandle();
  canvas->save();
  canvas->clipPath(path, SkClipOp::kIntersect, true);
}

void RoundPanel::postPaint(IRenderTarget *pRenderTarget, int width,
                           int height) {
  if (m_corner.IsZero())
    return;

  SkCanvas *canvas = (SkCanvas *)pRenderTarget->GetHandle();
  canvas->restore();
}

} // namespace ui