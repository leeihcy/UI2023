#ifndef UI_SDK_SRC_OBJECT_OBJECTATTRIB_H_
#define UI_SDK_SRC_OBJECT_OBJECTATTRIB_H_

#include "src/property/property_store.h"

namespace ui {
struct UIElement;
struct IRenderFont;

class ObjectProp : public PropertyStoreDelegate{
public:
  ObjectProp();

  const char *GetId();
  void SetId(const char *szText);

  const Rect& GetPadding();
  const Rect& GetMargin();
  const Rect& GetBorder();
  const Rect& GetExtNonClient();
  void SetPadding(const Rect&);
  void SetMargin(const Rect&);
  void SetBorder(const Rect&);
  void SetExtNonClient(const Rect&);
  
  void LoadAttributes(bool bReload);
  void LoadAttributeFromXml(UIElement *pElement, bool bReload);
  const char *GetAttribute(const char *szKey, bool bErase);
  void AddAttribute(const char *szKey, const char *szValue);
  std::shared_ptr<IAttributeMap> GetMapAttribute();
  void ClearMapAttribute();
  void InitDefaultAttrib();

  
  void SetBackRender(std::shared_ptr<IRenderBase> p);
  void SetForegndRender(std::shared_ptr<IRenderBase> p);
  void SetTextRender(std::shared_ptr<ITextRenderBase> p);
  std::shared_ptr<ITextRenderBase> GetTextRender();
  std::shared_ptr<IRenderBase> GetBackRender();
  std::shared_ptr<IRenderBase> GetForeRender();
  IRenderFont *GetRenderFont();

  void LoadBkgndRender(const char *szName);
  void LoadForegndRender(const char *szName);
  void LoadTextRender(const char *szName);
  const char *SaveBkgndRender();
  const char *SaveForegndRender();
  const char *SaveTextRender();

  void load_renderbase(const char *szName, std::shared_ptr<IRenderBase> &pRender);
  void load_textrender(const char *szName, std::shared_ptr<ITextRenderBase> &pTextRender);
  const char *get_renderbase_name(std::shared_ptr<IRenderBase> &pRender);
  const char *get_textrender_name(std::shared_ptr<ITextRenderBase> &pTextRender);

protected:
  void SerializeProperty(SerializeParam *pData);

  PropertyStore* GetInheritPropertyStore() override;

protected:
  // 属性集
  PropertyStore m_property_store;
  
  std::shared_ptr<IRenderBase> m_back_render;     // 背景渲染
  std::shared_ptr<IRenderBase> m_fore_render;     // 前景渲染
  std::shared_ptr<ITextRenderBase> m_text_render; // 文字渲染，由control负责读取该属性
  
  // TODO: 干掉！
  std::shared_ptr<IAttributeMap> m_attribute_map_remaining; // 用于扩展。未解析的属性
};

}

#endif