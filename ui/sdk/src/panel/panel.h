#pragma once

#include "include/interface/ipanel.h"
#include "panel_meta.h"
#include "src/object/object.h"

namespace ui {

class Panel : public Object {
public:
  Panel(IPanel *p);
  ~Panel();

  UI_BEGIN_MSG_MAP()
  UIMSG_GETDESIREDSIZE(OnGetDesiredSize)
  UIMSG_HANDLER_EX(UI_MSG_GETLAYOUT, OnGetLayoutPtr)
  UI_END_MSG_MAP_CHAIN_PARENT(Object)

  void onRouteMessage(ui::Msg *msg);

  IPanel *GetIPanel() { return m_pIPanel; }

public:
  ILayout *GetLayout();
  void SetLayout(ILayout*);
  void SetLayoutType(const char* layout_name);

  const char* GetLayoutName();
  void SetLayoutName(const char* name);

  void SetTextureRender(IRenderBase *p);
  IRenderBase *GetTextureRender();

protected:
  virtual void virtualOnSize(unsigned int nType, unsigned int nWidth,
                             unsigned int nHeight) override;

  void onEraseBkgnd(IRenderTarget *);
  void onPaint(IRenderTarget *pRenderTarget);
  void onPostPaint(IRenderTarget *pRenderTarget);
  long OnGetLayoutPtr(unsigned int uMsg, long wParam, long lParam);
  void onSerialize(SerializeParam *pData);
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

  std::string m_layout_name;
};

} // namespace ui
