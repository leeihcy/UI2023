#include "imagerender.h"
#include "include/inc.h"
#include "include/interface/irenderbase.h"
#include "include/interface/graphics.h"
#include "include/macro/msg.h"
#include "include/util/struct.h"
#include "src/attribute/9region_attribute.h"
#include "src/attribute/attribute.h"
#include "src/attribute/bool_attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/int_attribute.h"
#include "src/attribute/rect_attribute.h"
#include "src/attribute/string_attribute.h"
#include "src/helper/scale/scale_factor.h"
#include "src/object/object.h"
#include "src/render/render_meta.h"
#include <memory>


namespace ui {

ImageRender::ImageRender(IImageRender *p) : RenderBase(p) {
  m_pIImageRender = p;
  
  m_pColorBk = nullptr;
  m_nImageDrawType = DRAW_BITMAP_BITBLT;
  m_nAlpha = 255;
  m_rcSrc.SetEmpty();
  m_eBkColorFillType = BKCOLOR_FILL_ALL;
}
ImageRender::~ImageRender() {
  // SAFE_RELEASE(m_pColorBk);
}

void ImageRender::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_RENDERBASE_DRAWSTATE) {
    DrawState(&((RenderBaseDrawStateMessage*)msg)->draw_state);
    return;
  }
  else if (msg->message == UI_MSG_SERIALIZE) {
    RenderBase::onRouteMessage(msg);
    OnSerialize(static_cast<SerializeMessage*>(msg)->param);
    return;
  }
  else if (msg->message == UI_MSG_GETDESIREDSIZE) {
    GetDesiredSize(&static_cast<GetDesiredSizeMessage*>(msg)->size);
    return;
  }
  else if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto* m = static_cast<QueryInterfaceMessage*>(msg);
    if (m->uuid == ImageRenderMeta::Get().UUID()) {
      *(m->pp) = m_pIImageRender;
      return;
    }
  }
  RenderBase::onRouteMessage(msg);
}

void ImageRender::SetAlpha(int nAlpha) { m_nAlpha = nAlpha; }
int ImageRender::GetAlpha() { return m_nAlpha; }

Color ImageRender::GetColor() {
  if (nullptr == m_pColorBk)
    return Color::Make(0);
  else
    return *m_pColorBk;
}

void ImageRender::SetImageDrawType(int n) { m_nImageDrawType = n; }
int ImageRender::GetImageDrawType() { return m_nImageDrawType; }

void ImageRender::SetImageStretch9Region(const C9Region &r) { m_Region = r; }

void ImageRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "ImageRender");
#if 0
  s.AddString(XML_RENDER_IMAGE, Slot(&ImageRender::LoadBitmap, this),
              Slot(&ImageRender::GetBitmapId, this));
#endif
  // 背景颜色 TODO:
#if 0
  s.AddString(XML_RENDER_COLOR, Slot(&ImageRender::LoadColor, this),
              Slot(&ImageRender::GetColorId, this));
#endif
  // 拉伸区域
  s.Add9Region(XML_RENDER_IMAGE9REGION, m_Region);

  // 绘制透明度
  s.AddInt(XML_RENDER_IMAGE_ALPHA, m_nAlpha)->SetDefault(255);

  // 源区域
  s.AddRect(XML_RENDER_IMAGE_SRC_REGION, m_rcSrc);

  // 绘制类型
  s.AddEnum(XML_RENDER_IMAGE_DRAWTYPE, m_nImageDrawType)
      ->AddOption(DRAW_BITMAP_BITBLT, XML_RENDER_IMAGE_DRAWTYPE_BITBLT)
      ->AddOption(DRAW_BITMAP_TILE, XML_RENDER_IMAGE_DRAWTYPE_TILE)
      ->AddOption(DRAW_BITMAP_STRETCH, XML_RENDER_IMAGE_DRAWTYPE_STRETCH)
      ->AddOption(DRAW_BITMAP_STRETCH_DISABLE_AA,
                  XML_RENDER_IMAGE_DRAWTYPE_STRETCH_DISABLE_AA)
      ->AddOption(DRAW_BITMAP_ADAPT, XML_RENDER_IMAGE_DRAWTYPE_ADAPT)
      ->AddOption(DRAW_BITMAP_CENTER, XML_RENDER_IMAGE_DRAWTYPE_CENTER)
      ->AddOption(DRAW_BITMAP_STRETCH_BORDER,
                  XML_RENDER_IMAGE_DRAWTYPE_STRETCH_BORDER)
      ->AddOption(DRAW_BITMAP_BITBLT_RIGHTTOP,
                  XML_RENDER_IMAGE_DRAWTYPE_BITBLT_RIGHTTOP)
      ->AddOption(DRAW_BITMAP_BITBLT_LEFTBOTTOM,
                  XML_RENDER_IMAGE_DRAWTYPE_BITBLT_LEFTBOTTOM)
      ->AddOption(DRAW_BITMAP_BITBLT_RIGHTBOTTOM,
                  XML_RENDER_IMAGE_DRAWTYPE_BITBLT_RIGHTBOTTOM)
      ->AddOption(DRAW_BITMAP_BITBLT_LEFTVCENTER,
                  XML_RENDER_IMAGE_DRAWTYPE_BITBLT_LEFTVCENTER)
      ->SetDefault(DRAW_BITMAP_BITBLT);
}

// void ImageRender::SetRenderBitmap(IRenderBitmap *pBitmap) {
//   m_render_bitmap.reset();
//   m_render_bitmap = pBitmap;
// }

void ImageRender::SetColor(Color c) {
  // SAFE_RELEASE(m_pColorBk);
  // m_pColorBk = Color::CreateInstance(c);
}

void ImageRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  Rect *prc = &pDrawStruct->rc;

  // if (m_pColorBk && m_eBkColorFillType == BKCOLOR_FILL_ALL) {
  //   Color c = m_pColorBk->m_col;
  //   c.a = (byte)m_nAlpha;

  //   pDrawStruct->pRenderTarget->DrawRect(prc, &c);
  // }

  Rect rcRealDraw = {0, 0, 0, 0};
  if (m_render_bitmap) {
    DrawBitmapParam param;
    param.nFlag = m_nImageDrawType;
    param.xDest = prc->left;
    param.yDest = prc->top;
    param.wDest = prc->right - prc->left;
    param.hDest = prc->bottom - prc->top;
    if (!m_rcSrc.IsEmpty()) {
      param.xSrc = m_rcSrc.left;
      param.ySrc = m_rcSrc.top;
      param.wSrc = m_rcSrc.right - m_rcSrc.left;
      param.hSrc = m_rcSrc.bottom - m_rcSrc.top;
    } else if (m_render_bitmap) {
      param.xSrc = 0;
      param.ySrc = 0;
      param.wSrc = m_render_bitmap->GetWidth();
      param.hSrc = m_render_bitmap->GetHeight();
    }
    if (!m_Region.IsAll_0()) {
      param.nine_region = m_Region;
    }
    param.opacity = 255-(byte)m_nAlpha;

    if (pDrawStruct->nState & RENDER_STATE_DISABLE) {
      param.nFlag |= DRAW_BITMAP_DISABLE;
    }

    // if (m_pColorBk && m_eBkColorFillType == BKCOLOR_FILL_EMPTY) {
    //   param.prcRealDraw = &rcRealDraw;
    // }

    // param.scale_factor = ScaleFactorHelper::GetObjectScaleFactor(m_pObject);
    pDrawStruct->pRenderTarget->DrawBitmap(m_render_bitmap, &param);
  }

  // if (m_pColorBk && m_eBkColorFillType == BKCOLOR_FILL_EMPTY) {
  //   Color c = m_pColorBk->m_col;
  //   c.a = (byte)m_nAlpha;

  //   // Top
  //   {
  //     Rect rc = {prc->left, prc->top, prc->right, rcRealDraw.top};
  //     if (rc.Width() > 0 && rc.Height() > 0) {
  //       pDrawStruct->pRenderTarget->DrawRect(&rc, &c);
  //     }
  //   }
  //   // Left
  //   {
  //     Rect rc = {prc->left, rcRealDraw.top, rcRealDraw.left, rcRealDraw.bottom};
  //     if (rc.Width() > 0 && rc.Height() > 0) {
  //       pDrawStruct->pRenderTarget->DrawRect(&rc, &c);
  //     }
  //   }
  //   // Right
  //   {
  //     Rect rc = {rcRealDraw.right, rcRealDraw.top, prc->right,
  //                rcRealDraw.bottom};
  //     if (rc.Width() > 0 && rc.Height() > 0) {
  //       pDrawStruct->pRenderTarget->DrawRect(&rc, &c);
  //     }
  //   }
  //   // Bottom
  //   {
  //     Rect rc = {prc->left, rcRealDraw.bottom, prc->right, prc->bottom};
  //     if (rc.Width() > 0 && rc.Height() > 0) {
  //       pDrawStruct->pRenderTarget->DrawRect(&rc, &c);
  //     }
  //   }
  // }
}
void ImageRender::GetDesiredSize(Size *pSize) {
  pSize->width = pSize->height = 0;
  if (!m_render_bitmap)
    return;

  pSize->width = m_render_bitmap->GetWidth();
  pSize->height = m_render_bitmap->GetHeight();
}

//////////////////////////////////////////////////////////////////////////

ImageListItemRender::ImageListItemRender(IImageListItemRender *p)
    : ImageRender(p) {
  m_pIImageListItemRender = p;
  m_nImagelistIndex = -1;
}
ImageListItemRender::~ImageListItemRender() {
  m_nImagelistIndex = 0;
}

void ImageListItemRender::OnSerialize(SerializeParam *pData) {
  ImageRender::OnSerialize(pData);

  {
    AttributeSerializer s(pData, "ImageListItemRender");
    s.AddInt(XML_RENDER_IMAGELISTITEM_INDEX, m_nImagelistIndex);
  }

  if (pData->IsLoad()) {
    if (m_render_bitmap) {
      if (m_render_bitmap->GetImageType() != eImageType::ImageList) {
        m_render_bitmap.reset();
      }
    }
    if (m_render_bitmap) {
      m_image_list = std::static_pointer_cast<IImageListRenderBitmap>(m_render_bitmap);

      Point pt = {0, 0};
      m_image_list->GetIndexPos(m_nImagelistIndex, &pt);
      m_rcSrc.left = pt.x;
      m_rcSrc.top = pt.y;

      Size s;
      this->GetDesiredSize(&s);
      m_rcSrc.right = m_rcSrc.left + s.width;
      m_rcSrc.bottom = m_rcSrc.top + s.height;
    }
  }
}

void ImageListItemRender::GetDesiredSize(Size *pSize) {
  pSize->width = 0;
  pSize->height = 0;
  if (!m_image_list)
    return;

  pSize->width = m_image_list->GetItemWidth();
  pSize->height = m_image_list->GetItemHeight();
}

void ImageListItemRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  if (DRAW_BITMAP_TILE == m_nImageDrawType) {
    UI_LOG_WARN("image list item donot support tile draw");
    UIASSERT(0 && "TODO:");
    return;
  }

  if (-1 == m_nImagelistIndex && m_image_list) {
    Point pt = {0, 0};
    Size s = {0, 0};

    if (false == m_image_list->GetIndexPos(pDrawStruct->nState, &pt))
      return;
    this->GetDesiredSize(&s);

    m_rcSrc.Set(pt.x, pt.y, pt.x + s.width, pt.y + s.height);
  }
  assert(false);
#if 0 // 废弃，使用RouteMessage代替。
  SetMsgHandled(false);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ImageListRender //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

ImageListRender::ImageListRender(IImageListRender *p) : RenderBase(p) {
  m_pIImageListRender = p;

  m_nImageDrawType = DRAW_BITMAP_BITBLT;

  m_nPrevState = RENDER_STATE_NORMAL;
  m_nCurrentAlpha = 255;
  m_bIsAnimate = false;
  m_bUseAlphaAnimate = false;
}
ImageListRender::~ImageListRender() {
  DestroyAnimate();
}

void ImageListRender::OnSerialize(SerializeParam *pData) {
  {
    AttributeSerializer s(pData, "ImageListItemRender");
    s.AddBool(XML_RENDER_IMAGELIST_ALPHA_ANIMATE, m_bUseAlphaAnimate);

    s.Add9Region(XML_RENDER_IMAGE9REGION, m_9Region);

    s.AddEnum(XML_RENDER_IMAGE_DRAWTYPE, m_nImageDrawType)
        ->AddOption(DRAW_BITMAP_BITBLT, XML_RENDER_IMAGE_DRAWTYPE_BITBLT)
        ->AddOption(DRAW_BITMAP_TILE, XML_RENDER_IMAGE_DRAWTYPE_TILE)
        ->AddOption(DRAW_BITMAP_STRETCH, XML_RENDER_IMAGE_DRAWTYPE_STRETCH)
        ->AddOption(DRAW_BITMAP_ADAPT, XML_RENDER_IMAGE_DRAWTYPE_ADAPT)
        ->AddOption(DRAW_BITMAP_CENTER, XML_RENDER_IMAGE_DRAWTYPE_CENTER)
        ->AddOption(DRAW_BITMAP_STRETCH_BORDER,
                    XML_RENDER_IMAGE_DRAWTYPE_STRETCH_BORDER)
        ->AddOption(DRAW_BITMAP_BITBLT_LEFTVCENTER,
                    XML_RENDER_IMAGE_DRAWTYPE_BITBLT_LEFTVCENTER)
        ->SetDefault(DRAW_BITMAP_BITBLT);

    s.AddString(XML_RENDER_LIST_STATEMAPINDEX,
                Slot(&ImageListRender::SetState2Index, this),
                Slot(&ImageListRender::GetState2Index, this));

    s.AddString(XML_RENDER_IMAGE, Slot(&ImageListRender::LoadImageList, this),
                Slot(&ImageListRender::GetImageListId, this));
  }
}

void ImageListRender::LoadImageList(const char *szText) {
#if 0
  m_image_list.reset();

  std::shared_ptr<IRenderBitmap> pBitmap = _LoadBitmap(szText);
  if (nullptr == pBitmap)
    return;

  if (pBitmap->GetImageType() != IMAGE_ITEM_TYPE_IMAGE_LIST) {
    UI_LOG_WARN("ImageType != IMAGE_ITEM_TYPE_IMAGE_LIST. Bitmap: %s",
                szText);
    return;
  }
  m_image_list = std::static_pointer_cast<IImageListRenderBitmap>(pBitmap);

  // 如果图片没有alpha通道，则不能支持alpha动画
  if (m_image_list) {
    if (m_image_list->GetBPP() != 32) {
      m_bUseAlphaAnimate = false;
    }
  }
#endif
}
const char *ImageListRender::GetImageListId() {
  return nullptr;
#if 0
  ui::IRenderBitmap *p = static_cast<ui::IRenderBitmap *>(m_image_list.get());
  return _GetBitmapId(p);
#endif
}

void ImageListRender::SetState2Index(const char *szText) {
  m_mapState2Index.clear();

  util::ISplitStringEnum *pEnum = nullptr;
  int nCount = util::SplitString(szText, XML_SEPARATOR, &pEnum);
  for (int i = 0; i < nCount; i++) {
    const char *szStateIndex = pEnum->GetText(i);

    util::ISplitStringEnum *pEnumInner = nullptr;
    if (2 != util::SplitString(szStateIndex, L':', &pEnumInner)) {
      UI_LOG_WARN("invalid state index: %s", szStateIndex);
      SAFE_RELEASE(pEnumInner);
      continue;
    }
    int nState = atoi(pEnumInner->GetText(0));
    int nIndex = atoi(pEnumInner->GetText(1));
    m_mapState2Index[nState] = nIndex;
    SAFE_RELEASE(pEnumInner);
  }
  SAFE_RELEASE(pEnum);
}
const char *ImageListRender::GetState2Index() {
  if (m_mapState2Index.empty())
    return nullptr;

  std::string &strTempBuffer = GetTempBufferString();

  char szItem[16] = {0};
  std::map<int, int>::iterator iter = m_mapState2Index.begin();
  for (; iter != m_mapState2Index.end(); ++iter) {
    snprintf(szItem, 16, "%d%c%d", iter->first, XML_KEYVALUE_SEPARATOR,
            iter->second);

    if (!strTempBuffer.empty())
      strTempBuffer.push_back(XML_SEPARATOR);
    strTempBuffer.append(szItem);
  }

  return strTempBuffer.c_str();
}

void ImageListRender::SetIImageListRenderBitmap(
    IImageListRenderBitmap *pBitmap) {
  // SAFE_RELEASE(m_pImageList);
  // m_pImageList = pBitmap;
  // if (m_pImageList)
  //   m_pImageList->AddRef();
  assert(false); // TODO:
}

IRenderBitmap *ImageListRender::GetRenderBitmap() { return m_image_list.get(); }
#if 0
uia::eAnimateTickResult ImageListRender::OnAnimateTick(uia::IStoryboard* pStoryboard)
{
    m_nCurrentAlpha = pStoryboard->FindTimeline(0)
		->GetCurrentIntValue();

    bool bFinish = pStoryboard->IsFinish();
    //m_pObject->UpdateObject();  // 注：在listbox这种控件中，会使用当前foregnd render连续绘制所有的item，导致Imagelistrender中的动画被不断的删除和添加，最后崩溃
	m_pObject->Invalidate();

    if (bFinish)
        m_bIsAnimate = false;

    return uia::eAnimateTickResult::Continue;
}
#endif
void ImageListRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  if (!m_image_list)
    return;

  IRenderTarget *pRenderTarget = pDrawStruct->pRenderTarget;
  Rect rc = pDrawStruct->rc;
  int &nState = pDrawStruct->nState;

  int nRenderState = (pDrawStruct->nState) & RENDER_STATE_MASK;
  int nRealIndex = (nState)&0xFFFF;
  // 	if (nRealIndex >= m_pImageList->GetItemCount())  // 用GetStateIndex获取
  // 		nRealIndex = 0;

  if (false == m_bUseAlphaAnimate) {
    DrawIndexWidthAlpha(pRenderTarget, &rc, nRealIndex, 255);
    return;
  }

  // 从Normal->Hover或者Hover->Normal时，开启动画计时
  if ((m_nPrevState & (RENDER_STATE_NORMAL | RENDER_STATE_DEFAULT)) &&
      (nRenderState & RENDER_STATE_HOVER)) {
    m_nCurrentAlpha =
        0; // 避免在第一次Tick响应之前被控件强制刷新了，结果此时的m_nCurrentAlpha不是计算得到的值。
    CreateAnimate(0, 255);

    DrawIndexWidthAlpha(pRenderTarget, &rc, (m_nPrevState)&0xFFFF, 255);
  } else if ((nRenderState & (RENDER_STATE_NORMAL | RENDER_STATE_DEFAULT)) &&
             (m_nPrevState & RENDER_STATE_HOVER)) {
    m_nCurrentAlpha = 255;
    CreateAnimate(255, 0);

    DrawIndexWidthAlpha(pRenderTarget, &rc, (m_nPrevState)&0xFFFF, 255);
  } else {
    if (m_bIsAnimate) {
      if (0 == (nRenderState & (RENDER_STATE_NORMAL | RENDER_STATE_DEFAULT |
                                RENDER_STATE_HOVER))) {
        // 在动画过程中按下了，应该立即停止动画
        DestroyAnimate();
        DrawIndexWidthAlpha(pRenderTarget, &rc, nRealIndex, 255);
      } else {
        bool bSelected = (nRenderState & RENDER_STATE_SELECTED) ? true : false;
        if (bSelected) // checkbox
        {
          DrawIndexWidthAlpha(pRenderTarget, &rc, 4,
                              (byte)(255 - m_nCurrentAlpha));
          DrawIndexWidthAlpha(pRenderTarget, &rc, 5, (byte)m_nCurrentAlpha);
        } else // pushbutton/defaultbutton
        {
          bool bDefault = (nRenderState & RENDER_STATE_DEFAULT) ? true : false;

          DrawIndexWidthAlpha(pRenderTarget, &rc, bDefault ? 4 : 0,
                              (byte)(255 - m_nCurrentAlpha));
          DrawIndexWidthAlpha(pRenderTarget, &rc, 1, (byte)m_nCurrentAlpha);
        }
      }
    } else {
      DrawIndexWidthAlpha(pRenderTarget, &rc, nRealIndex, 255);
    }
  }
  m_nPrevState = nState;
}

void ImageListRender::DestroyAnimate() {
#if 0
    if (m_bUseAlphaAnimate && m_bIsAnimate)
    {
        IApplication* pUIApp = m_pObject->GetIUIApplication();
        uia::IAnimate* pAnimateMgr = pUIApp->GetAnimate();
        pAnimateMgr->ClearStoryboardByNotify(
            static_cast<IAnimateEventCallback*>(this));
        m_bIsAnimate = false;
    }
#endif
}

void ImageListRender::CreateAnimate(int nFrom, int nTo) {
#if 0
    IApplication* pUIApp = m_pObject->GetIUIApplication();
    uia::IAnimate* pAnimateMgr = pUIApp->GetAnimate();
    pAnimateMgr->ClearStoryboardByNotify(
        static_cast<IAnimateEventCallback*>(this));

    uia::IStoryboard*  pStoryboard = pAnimateMgr->
            CreateStoryboard(static_cast<IAnimateEventCallback*>(this));

    uia::IFromToTimeline* pTimeline = pStoryboard->CreateTimeline(0);
    pTimeline->SetParam((float)nFrom, (float)nTo, 200)
            ->SetEaseType(uia::linear);
    pStoryboard->Begin();

    m_bIsAnimate = true;
#endif
}

void ImageListRender::DrawIndexWidthAlpha(IRenderTarget *pRenderTarget,
                                          const Rect *prc, int nIndex,
                                          byte bAlpha) {
  if (!m_image_list)
    return;

  //    UI_LOG_ERROR("%s  nIndex=%d, nAlpha=%d", FUNC_NAME, nIndex, bAlpha);
  int nRealIndex = GetStateIndex(nIndex);
  if (nRealIndex < 0)
    return;

  DrawBitmapParam param;
  param.nFlag = m_nImageDrawType;
  param.xDest = prc->left;
  param.yDest = prc->top;
  param.wDest = prc->Width();
  param.hDest = prc->Height();
  param.wSrc = m_image_list->GetItemWidth();
  param.hSrc = m_image_list->GetItemHeight();
  if (!m_9Region.IsAll_0()) {
    param.nine_region = m_9Region;
  }
  param.opacity = 255-bAlpha;

  Point pt = {0, 0};
  m_image_list->GetIndexPos(nRealIndex, &pt);
  param.xSrc = pt.x;
  param.ySrc = pt.y;
  // 	if (m_eImageLayout == IMAGELIST_LAYOUT_TYPE_H)
  // 	{
  // 		param.xSrc = nIndex*m_nItemWidth;
  // 		param.ySrc = 0;
  // 	}
  // 	else
  // 	{
  // 		param.xSrc = 0;
  // 		param.ySrc = nIndex*m_nItemHeight;
  // 	}
  pRenderTarget->DrawBitmap(m_image_list, &param);
}

void ImageListRender::GetDesiredSize(Size *pSize) {
  pSize->width = pSize->height = 0;
  if (!m_image_list)
    return;

  pSize->width = m_image_list->GetItemWidth();
  pSize->height = m_image_list->GetItemHeight();
}

int ImageListRender::GetItemWidth() {
  if (!m_image_list)
    return 0;

  return m_image_list->GetItemWidth();
}
int ImageListRender::GetItemHeight() {
  if (!m_image_list)
    return 0;

  return m_image_list->GetItemHeight();
}
int ImageListRender::GetItemCount() {
  if (!m_image_list)
    return 0;

  return m_image_list->GetItemCount();
}

void ImageListRender::SetImageStretch9Region(C9Region *p) {
  if (p)
    m_9Region.Copy(*p);
  else
    m_9Region.Set(0);
}
//
//	获取指定状态对应的图片项
//
int ImageListRender::GetStateIndex(int nState) {
  if (!m_image_list)
    return -1;

  if (m_image_list->GetItemCount() <= 0)
    return -1;

  if (m_mapState2Index.empty())
    return nState;

  std::map<int, int>::iterator iter = m_mapState2Index.end();
  if (m_mapState2Index.size() > 0)
    iter = m_mapState2Index.find(nState);

  if (m_mapState2Index.end() == iter) {
    // 没有配置该状态或者没有配置
    // 		if (nState < m_pImageList->GetItemCount())  //
    // 如果图片数量大于当前状态，直接取当前状态值对应的图片
    // 		{
    // 			return nState;
    // 		}
    // 		else                    // 状态值大于图片数量，取默认值0
    // 		{
    // 			return 0;
    // 		}
    return -1;
  } else {
    int &nRet = iter->second;
    if (nRet < 0 || nRet >= m_image_list->GetItemCount())
      return -1;

    return nRet;
  }
  return -1;
}

} // namespace ui