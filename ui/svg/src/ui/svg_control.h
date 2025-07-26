#ifndef _UI_SVG_SRC_SVG_SVG_H_
#define _UI_SVG_SRC_SVG_SVG_H_

#include "sdk/include/interface/imessage.h"
#include "include/interface/isvg.h"
#include "src/element/svg/svg.h"


namespace ui { 
struct SerializeParam;

class Svg : public ui::MessageProxy {
public:
  Svg(ISvg*);
  ISvg* GetISvg() { return m_pISvg; }

public:
  void onRouteMessage(Msg* msg);

  bool Load(const char* stream);
  void SetAttribute(ui::SerializeParam& data);

protected:
  void onFinalConstruct();
  void onSerialize(SerializeParam *pData);
  // void onPaintBkgnd(ui::IRenderTarget* pRenderTarget);
  void onPaint(ui::IRenderTarget *rt);
  ui::Size onGetDesiredSize();
  
  void setImageResourceId(const char *);
  const char* getImageResourceId();

private:
  ISvg* m_pISvg;
  std::unique_ptr<svg::Svg> m_root;
};

}

#endif