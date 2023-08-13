#pragma once

#include "include/interface/ipanel.h"
#include "panel_desc.h"
#include "src/object/object.h"

namespace ui {

class Panel : public Object {
public:
  Panel(IPanel *p);
  ~Panel();

  UI_BEGIN_MSG_MAP()
  UIMSG_ERASEBKGND(OnEraseBkgnd)
  UIMSG_PAINT(OnPaint)
  UIMSG_POSTPAINT(OnPostPaint)
  UIMSG_GETDESIREDSIZE(OnGetDesiredSize)
  UIMSG_HANDLER_EX(UI_MSG_GETLAYOUT, OnGetLayoutPtr)
  UIMSG_QUERYINTERFACE(Panel)
  UIMSG_SERIALIZE(OnSerialize)
  UI_END_MSG_MAP_CHAIN_PARENT(Object)

  IPanel *GetIPanel() { return m_pIPanel; }

public:
  ILayout *GetLayout();
  LAYOUTTYPE GetLayoutType();
  void SetLayoutType(LAYOUTTYPE eLayoutType);
  long GetLayoutType2();
  void SetLayoutType2(long eLayoutType);

  void SetTextureRender(IRenderBase *p);
  IRenderBase *GetTextureRender();

  IObjectDescription *GetObjectDescription() { return PanelDescription::Get(); }

protected:
  virtual void virtualOnSize(unsigned int nType, unsigned int nWidth,
                             unsigned int nHeight) override;

  void OnEraseBkgnd(IRenderTarget *);
  void OnPaint(IRenderTarget *pRenderTarget);
  void OnPostPaint(IRenderTarget *pRenderTarget);
  long OnGetLayoutPtr(unsigned int uMsg, long wParam, long lParam);
  void OnSerialize(SERIALIZEDATA *pData);
  void OnGetDesiredSize(Size *pSize); 
protected:
  IPanel *m_pIPanel;
  ILayout *m_pLayout;
  Rect m_rcBkgndRenderRegion;
  Rect m_rcForegndRenderRegion;

  // 用于支持换肤功能的皮肤图片
  IRenderBase *m_pTextureRender;
  Rect m_rcTextureRenderRegion;

  // 最上面的遮罩层
  IRenderBase *m_pMaskRender;
  Rect m_rcMaskRenderRegion;
};

} // namespace ui
