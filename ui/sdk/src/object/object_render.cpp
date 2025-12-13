#include "include/inc.h"
#include "include/util/log.h"
#include "object.h"
// #include "src/UIObject\Window\windowbase.h"
// #include "src/Atl\image.h"
#include "include/interface/irenderbase.h"
#include "include/interface/graphics.h"
#include "object_layer.h"
#include "src/application/config/config.h"
// #include "src/layer/software_layer.h"

// 2016.6.1 渲染逻辑改造
//
// 1. 废弃UpdateObject方法，统一改成Invalidate方法
// 2. 废弃ListCtrl中的LF_REDRAW标识位，外部不再需要关心刷新

using namespace ui;

#if 0
HBITMAP  Object::TakeSnapshot()
{
    Image image;
    image.Create(GetWidth(), GetHeight(), 32, Image::createAlphaChannel);
    HDC hDC = image.GetDC();
    SetBkMode(hDC, TRANSPARENT);
    SetStretchBltMode(hDC, HALFTONE);
    
    IRenderTarget*  pRenderTarget = nullptr;
    GetWindowRender()->CreateRenderTarget(&pRenderTarget);
    pRenderTarget->BindHDC(hDC);
    pRenderTarget->BeginDraw();
    {
        Rect rcRenderRegion(0, 0, GetWidth(), GetHeight());
        RenderContext  context(&rcRenderRegion, &rcRenderRegion, true);
        this->RealDrawObject(pRenderTarget, context);
    }
    pRenderTarget->EndDraw();

    pRenderTarget->Release();
    image.ReleaseDC();
#ifdef _DEBUGx
    image.Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
#endif
    return image.Detach();
}


// 不支持zorder重叠，直接取祖先的背景
HBITMAP  Object::TakeBkgndSnapshot()
{
    if (!m_pParent)
        return nullptr;

    Image image;
    image.Create(m_rcParent.Width(), m_rcParent.Height(), 32, Image::createAlphaChannel);
    HDC hDC = image.GetDC();
    SetBkMode(hDC, TRANSPARENT);
    SetStretchBltMode(hDC, HALFTONE);

	IRenderTarget*  pRenderTarget = nullptr;
    GetWindowRender()->CreateRenderTarget(&pRenderTarget);

    pRenderTarget->BindHDC(hDC);
    pRenderTarget->BeginDraw();
    {
        // 检查祖父对象中有没有不透明的，从该父对象画起
        Object*  pObjAncestorFirst2DrawBkgnd = nullptr;
        Object*  pChild = this;
        Rect    rcInAncestor(0, 0, GetWidth(), GetHeight());
        while (pObjAncestorFirst2DrawBkgnd = this->EnumParentObject(pObjAncestorFirst2DrawBkgnd))
        {
            ChildRect2ParentRect(pChild, &rcInAncestor, &rcInAncestor);

            // 找到不透明对象或者层对象
            if (!pObjAncestorFirst2DrawBkgnd->IsTransparent())
                break;

            pChild = pObjAncestorFirst2DrawBkgnd;
        }
        if (nullptr == pObjAncestorFirst2DrawBkgnd)
            pObjAncestorFirst2DrawBkgnd = GetWindowObject();

        Rect  rcClip(0, 0, GetWidth(), GetHeight());
        Rect  rcCurrentClip(&rcInAncestor);
        rcCurrentClip.OffsetRect(-rcInAncestor.left*2, -rcInAncestor.top*2);
        RenderContext  context(&rcClip, &rcCurrentClip, false);
        context.m_ptOffset.x = rcCurrentClip.left;
        context.m_ptOffset.y = rcCurrentClip.top;

        Object*  pParent = pObjAncestorFirst2DrawBkgnd;
        context.Update(pRenderTarget);
        ::UISendMessage(pParent, WM_ERASEBKGND, (long)pRenderTarget, (long)&context);
        while (pChild = REnumParentObject(pParent))
        {
            if (!pChild->IsNcObject())
                context.DrawClient(pParent->m_pIObject);
            context.DrawChild(pChild->m_pIObject);
            context.Update(pRenderTarget);

            ::UISendMessage(pParent, WM_ERASEBKGND, (long)pRenderTarget, (long)&context);
            pParent = pChild;
        }
    }
    pRenderTarget->EndDraw();
    pRenderTarget->Release();
    image.ReleaseDC();

#ifdef _DEBUGx
    image.Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
#endif
    return image.Detach();
}


void Object::DoPostPaint(IRenderTarget* pRenderTarget, RenderContext context)
{
	if (m_objStyle.post_paint)
	{
		// 子控件可能设置了剪裁区域。这里得先恢复再画postpaint
		context.m_bUpdateClip = true;
		context.Update(pRenderTarget);
		m_pIObject->SendMessage(,
			UI_WM_POSTPAINT, 
			(long)pRenderTarget, 
			(long)&context);
	}
}

#endif

void Object::OnEraseBkgnd(IRenderTarget *pRenderTarget) {
  if (m_back_render) {
    Rect rc = {0, 0, this->GetWidth(), this->GetHeight()};
    m_back_render->DrawState(pRenderTarget, &rc, 0);
  }
}

// 从上到下刷新方式(更简单),但对分层窗口无效。
// 分层窗口BeginPaint之后，拿到的ps.rcPaint为空
//
// InvalidateObject可以多次调用，但只在/*程序空闲时*/ 下一次消息循环时
// 才真正刷新。
//
// 1. 用于解决实践开发过程中，滥用UpdateObject的问题
// 2. 用于实现动画属性变化时延时刷新，提高效率。
// 3. 用于解决实践过程当中，代码调用复杂时，不知道当前是否需要调用刷新函数，
//    调多了可能导致效率低下，调少了界面没法刷新
// 4. 实践过程中，很多开发人员对刷新不了解，不知道什么时候需要调刷新
//
void Object::Invalidate() {
  Rect rc = {0, 0, m_rcParent.width(), m_rcParent.height()};
  Invalidate(&rc);
}

void Object::Invalidate(const Rect *prcObj) {
  if (!prcObj)
    return;

  if (!IsVisible())
    return;

  ObjectLayer *pLayer = GetLayerEx();
  if (!pLayer) // 刚创建，还未初始化layer阶段
    return;

  Object &layerObj = pLayer->GetObject();

  Rect rc = {0};
  if (!CalcRectInAncestor(&layerObj, prcObj, true, &rc))
    return;

  if (Config::GetInstance().debug.log_paint) {
    UI_LOG_DEBUG("[Object] Invalidate id=%s, rect=%d,%d (%d,%d)",
      GetId(), rc.left, rc.top, rc.Width(), rc.Height());
  }
  pLayer->GetLayer()->Invalidate(&rc);
}

void Object::Invalidate(Rect *prcObjArray, int nCount) {
  for (int i = 0; i < nCount; i++) {
    this->Invalidate(prcObjArray + i);
  }
}

//////////////////////////////////////////////////////////////////////////

void Object::DrawToLayer__(IRenderTarget *pRenderTarget) {
  // 1. 非客户区，不受padding scroll影响
  PaintBkgndMessage message;
  message.rt = pRenderTarget;
  m_pIObject->RouteMessage(&message);
  
  this->virtualOnPostDrawObjectErasebkgnd();

  // 2. 客户区，更新裁剪、偏移
  Rect rcObj = {0, 0, m_rcParent.width(), m_rcParent.height()};
  Rect rcClient;
  this->GetClientRectInObject(&rcClient);

  int xOffset = 0, yOffset = 0;
  GetScrollOffset(&xOffset, &yOffset);
  xOffset = -xOffset + rcClient.left;
  yOffset = -yOffset + rcClient.top;

  bool bNeedClip = true;
  if (rcClient == rcObj)
    bNeedClip = false;

  if (bNeedClip)
    pRenderTarget->PushRelativeClipRect(rcClient);
  {
    pRenderTarget->OffsetOrigin(xOffset, yOffset);
    {
      PaintMessage msg;
      msg.rt = pRenderTarget;
      m_pIObject->RouteMessage(&msg);

      if (m_pChild) {
        this->DrawChildObject__(pRenderTarget, m_pChild);
      }
    }
    pRenderTarget->OffsetOrigin(-xOffset, -yOffset);
  }
  if (bNeedClip)
    pRenderTarget->PopRelativeClipRect();

  // 3. 非客户区子对象
  if (m_pNcChild) {
    this->DrawChildObject__(pRenderTarget, m_pNcChild);
  }

  if (m_objStyle.post_paint) {
    PostPaintMessage msg;
    msg.rt = pRenderTarget;
    m_pIObject->RouteMessage(&msg);
  }
}

// 使用脏区域异步刷新方法，天然就支持z序绘制，只要有重叠区域就会被绘制
void Object::DrawChildObject__(IRenderTarget *pRenderTarget,
                               Object *pChildStart) {
  Object *pChild = pChildStart;
  while (pChild) {
    if (!pChild->IsSelfVisible()) {
      pChild = pChild->m_pNext;
      continue;
    }

    Layer *pChildLayer = pChild->GetSelfLayer();
    if (pChildLayer) {
      // 硬件合成，每个层单独绘制。
      if (pChildLayer->GetType() == Layer_Hardware) {
        pChild = pChild->m_pNext;
        continue;
      }
    }

    if (!pRenderTarget->IsRelativeRectInClip(pChild->m_rcParent)) {
      pChild = pChild->m_pNext;
      continue;
    }

    // 如果父对象不需要剪裁，那么自己也不剪裁
    bool bChildNeedClip = pChild->NeedClip();

    if (bChildNeedClip)
      pRenderTarget->PushRelativeClipRect(pChild->m_rcParent);

    // save,m_rcParent在绘制过程中可能被修改
    Point ptOffset = {pChild->m_rcParent.left, pChild->m_rcParent.top};
    pRenderTarget->OffsetOrigin(ptOffset.x, ptOffset.y);

    if (pChildLayer /* && pChildLayer->GetType() == Layer_Software*/) {
      // 软件渲染模式下面，子层要画在父层上面
      pChildLayer->UpdateDirty();

      Render2TargetParam param = {0};
      param.xSrc = param.xDst = 0;
      param.ySrc = param.yDst = 0;
      param.wSrc = param.wDst = pChild->GetWidth();
      param.hSrc = param.hDst = pChild->GetHeight();
      param.bAlphaBlend = true;
      param.opacity = pChildLayer->GetOpacity();
      pChildLayer->GetRenderTarget()->Render2Target(pRenderTarget, &param);
    } else {
      pChild->DrawToLayer__(pRenderTarget);
    }

    pRenderTarget->OffsetOrigin(-ptOffset.x, -ptOffset.y);

    if (bChildNeedClip)
      pRenderTarget->PopRelativeClipRect();

    pChild = pChild->m_pNext;
  }
}