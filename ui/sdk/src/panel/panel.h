#pragma once

#include "include/interface/ipanel.h"
#include "panel_meta.h"
#include "src/object/object.h"
#include <memory>

namespace ui {

class Panel : public Object {
public:
  Panel(IPanel *p);
  ~Panel();

  void onRouteMessage(ui::Msg *msg);

  IPanel *GetIPanel() { return static_cast<IPanel*>(m_imessage); }

public:
  ILayout *GetLayout();
  void SetLayout(ILayout*);
  void SetLayoutType(const char* layout_name);

  const char* GetLayoutName();
  void SetLayoutName(const char* name);

  void SetTextureRender(std::shared_ptr<IRenderBase> p);
  std::shared_ptr<IRenderBase> GetTextureRender();

protected:
  // virtual void virtualOnSize(unsigned int nType, unsigned int nWidth,
  //                            unsigned int nHeight, float scale) override;

  void onPaintBkgnd(IRenderTarget *);
  void onPaint(IRenderTarget *pRenderTarget);
  void onPostPaint(IRenderTarget *pRenderTarget);
  void onSerialize(SerializeParam *pData);
  void onGetDesiredSize(Size *pSize); 
  void onSize(int width, int height);
  
protected:
  ILayout *m_pLayout = nullptr;

  // 用于支持换肤功能的皮肤图片
  std::shared_ptr<IRenderBase> m_pTextureRender;
  // 最上面的遮罩层
  std::shared_ptr<IRenderBase> m_pMaskRender;

  std::string m_layout_name;
};

} // namespace ui
