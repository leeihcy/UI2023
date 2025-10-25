#ifndef _UI_SDK_SRC_PARSER_PARSEENGINE_H_
#define _UI_SDK_SRC_PARSER_PARSEENGINE_H_

namespace ui {
struct IUIElement;
struct UIElement;
struct UIDocument;
struct BundleSource;
class ResourceBundle;
struct IResourceBundle;
class Application;

class SkinParseEngine {
public:
  SkinParseEngine(ResourceBundle *resource_bundle);
  ~SkinParseEngine();

  bool Parse(BundleSource *pDataSource, const char *szXmlFile);
  void NewChild(UIElement *pElement);

  static int UIParseIncludeTagCallback(IUIElement *,
                                       IResourceBundle *resource_bundle);

private:
  bool loadUIDocument(BundleSource *pDataSource, UIDocument *pDocument,
                      const char *path);

  Application *m_pUIApplication;
  ResourceBundle *m_resource_bundle;
};

} // namespace ui

#endif