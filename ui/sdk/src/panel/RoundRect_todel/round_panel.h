#pragma once
// #include "Src\Atl\image.h"
#include "panel.h"
#include "src/graphics/bitmap/skiabitmap.h"

// 2016.4.1  libo
// 实现圆角Panel，以尽量少的内存占用实现。
//
// 原理：
//   摒弃以前将内容画在一个单独的buffer上，然后对buffer做roundrect，最后将
//   该buffer提交到dc上的方法。
//
//   1. 在画panel内容之前，将四个圆角的父背景内容缓存起来；
//   2. 画panel内容；
//   3. 对panel的内容直接做roundrect，不启用新缓存；
//   4. 将panel的四个新圆角贴到之前缓存的父背景缓存上去，此时这个缓存上的
//      内容将是四个圆角的最终显示；
//   5. 将缓存上的四个圆角还原到dc上去；
//      完成
//

// 注：back.render的数据不会做圆角化，fore.render+child的数据会被圆角化

namespace ui {



// 负责保存原始内容、更新panel圆角内容、将内容贴回到窗口上去
class BufferSwitcher {
public:
  BufferSwitcher();
  ~BufferSwitcher();

  void AddRef();
  void Release();

  void SaveLeftTop(IRenderTarget *pRenderTarget, uint radius);
  void SaveRightTop(IRenderTarget *pRenderTarget, uint radius, int width);
  void SaveLeftBottom(IRenderTarget *pRenderTarget, uint radius, int height);
  void SaveRightBottom(IRenderTarget *pRenderTarget, uint radius, int width, int height);

  void UpdateLeftTop(IRenderTarget *pRenderTarget, uint radius);
  void UpdateRightTop(IRenderTarget *pRenderTarget, uint radius, int width);
  void UpdateLeftBottom(IRenderTarget *pRenderTarget, uint radius, int height);
  void UpdateRightBottom(IRenderTarget *pRenderTarget, uint radius, int width, int height);
#if 0
  void RestoreLeftTop(IRenderTarget *pRenderTarget, uint radius);
  void RestoreRightTop(IRenderTarget *pRenderTarget, uint radius, int width);
  void RestoreLeftBottom(IRenderTarget *pRenderTarget, uint radius, int height);
  void RestoreRightBottom(IRenderTarget *pRenderTarget, uint radius, int width, int height);
#endif

private:
  void save(SkiaBitmap&, IRenderTarget *pRenderTarget, uint size, int x, int y);
  void update(SkiaBitmap&, IRenderTarget *pRenderTarget, uint size, int x, int y);
  void restore(SkiaBitmap&, IRenderTarget *pRenderTarget, uint size, int x, int y);

private:
  SkiaBitmap m_lefttop;
  SkiaBitmap m_righttop;
  SkiaBitmap m_leftbottom;
  SkiaBitmap m_rightbottom;

  long m_lRef;
};


class RoundPanel : public Panel {
public:
  RoundPanel(IRoundPanel *);
  ~RoundPanel();

  void onRouteMessage(ui::Msg *msg);

public:
  void SetRadius(int lefttop, int righttop, int leftbottom, int rightbottom);

protected:
  void onSerialize(SerializeParam *pData);
  void onEraseBkgnd(IRenderTarget *pRenderTarget);
  void onPaint(IRenderTarget *pRenderTarget);
  void onPostPaint(IRenderTarget *pRenderTarget);

  void loadCorner(int);
  int saveCorner();

  void prePaint(IRenderTarget *pRenderTarget, int width, int height);
  void postPaint(IRenderTarget *pRenderTarget, int width, int height);
private:
  IRoundPanel *m_pIRoundPanel;

  uint m_lefttop;
  uint m_righttop;
  uint m_leftbottom;
  uint m_rightbottom;

  BufferSwitcher s_parentBuf;
};

} // namespace ui