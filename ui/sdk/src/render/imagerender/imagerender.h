#pragma once
#include "include/interface/iimagerender.h"
#include "include/macro/xmldefine.h"
#include "include/util/struct.h"
#include "src/object/message.h"
#include "src/render/renderbase.h"
#include <map>
#include <memory>

namespace ui {

class ImageRender : public RenderBase {
public:
  ImageRender(IImageRender *p);
  ~ImageRender();

  void onRouteMessage(ui::Msg *msg);
public:
  void SetAlpha(int nAlpha);
  int GetAlpha();
  // void SetColor(Color c);
  // Color GetColor();
  void SetImageDrawType(int n);
  int GetImageDrawType();
  void SetImageStretch9Region(const C9Region &r);

  BKCOLOR_FILL_TYPE GetBkColorFillType() { return m_eBkColorFillType; }
  void SetBkColorFillType(BKCOLOR_FILL_TYPE eType) {
    m_eBkColorFillType = eType;
  }

  IRenderBitmap *GetRenderBitmap() { return m_render_bitmap.get(); }
  void loadBitmap(const char* src);
  const char* saveBitmap();

protected:
  void OnSerialize(SerializeParam *pData);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);
  void GetDesiredSize(Size *pSize);

protected:
  IImageRender *m_pIImageRender;

  std::shared_ptr<IRenderBitmap> m_render_bitmap;
  std::string m_image_src;

  // Color *m_pColorBk;
  C9Region m_Region; // 拉伸绘制时才用
  
  // 绘制区域(为空表示未设置)，有可能只是绘制图片的一部分.子类可以通过修改该值实现某些需求
  Rect m_src_range;

  BKCOLOR_FILL_TYPE m_eBkColorFillType;
  int m_nImageDrawType;
  int m_nAlpha;
};

// 绘制图片列表中指定索引的一项
class ImageListItemRender : public ImageRender {
public:
  ImageListItemRender(IImageListItemRender *p);
  ~ImageListItemRender();

  // UI_BEGIN_MSG_MAP()
  // UIMSG_RENDERBASE_DRAWSTATE(DrawState)
  // // TODO: onRouteMessage UIMSG_GETDESIREDSIZE(GetDesiredSize)
  // UIMSG_SERIALIZE(OnSerialize)
  // UIMSG_QUERYINTERFACE(ImageListItemRender)
  // UI_END_MSG_MAP_CHAIN_PARENT(ImageRender)

  void OnSerialize(SerializeParam *pData);
  void GetDesiredSize(Size *pSize);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);

protected:
  IImageListItemRender *m_pIImageListItemRender;

  std::shared_ptr<IImageListRenderBitmap> m_image_list;
  int m_nImagelistIndex;
};

//
//	图片背景按钮
//
struct IStoryboard;
class ImageListRender : public RenderBase
//			, public uia::IAnimateEventCallback
{
public:
  ImageListRender(IImageListRender *p);
  ~ImageListRender();

  // UI_BEGIN_MSG_MAP()
  // UIMSG_RENDERBASE_DRAWSTATE(DrawState)
  // UIMSG_QUERYINTERFACE(ImageListRender)
  // // TODO: onRouteMessage UIMSG_GETDESIREDSIZE(GetDesiredSize)
  // UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

  void SetIImageListRenderBitmap(IImageListRenderBitmap *pBitmap);
  IRenderBitmap *GetRenderBitmap();
  void LoadImageList(const char *);
  const char *GetImageListId();

  void SetImageDrawType(int n) { m_nImageDrawType = n; }
  int GetImageDrawType() { return m_nImageDrawType; }
  void SetImageStretch9Region(C9Region *p);
  void SetState2Index(const char *);
  const char *GetState2Index();

  void OnSerialize(SerializeParam *pData);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);
  void GetDesiredSize(Size *pSize);
  // virtual uia::eAnimateTickResult OnAnimateTick(uia::IStoryboard*)
  // override;

  int GetItemWidth();
  int GetItemHeight();
  int GetItemCount();

  int GetStateIndex(int nState);

protected:
  void DrawIndexWidthAlpha(IRenderTarget *, const Rect *prc, int nIndex,
                           byte bAlpha);
  void CreateAnimate(int nFrom, int nTo);
  void DestroyAnimate();

protected:
  IImageListRender *m_pIImageListRender;

  std::shared_ptr<IImageListRenderBitmap> m_image_list;
  int m_nImageDrawType;
  C9Region m_9Region;

  int m_nPrevState;    // 用于判断当前状态变化是否需要进行渐变
  int m_nCurrentAlpha; // 动画过程中的alpha值
  bool m_bIsAnimate;   // 当前是否正在动画过程中
  bool m_bUseAlphaAnimate; // 是否启用alpha渐变动画

  std::map<int, int> m_mapState2Index; // 要绘制的状态对应图片的哪个item
};

} // namespace ui