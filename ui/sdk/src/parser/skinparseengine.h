#pragma once


namespace ui
{
struct IUIElement;
struct UIElement;
struct BundleSource;
class ResourceBundle;
struct IResourceBundle;
class Application;

class SkinParseEngine
{
public:
    SkinParseEngine(ResourceBundle* resource_bundle);
    ~SkinParseEngine();

    bool  Parse(BundleSource* pDataSource, const char* szXmlFile);
    void  NewChild(UIElement* pElement);

    static int UIParseIncludeTagCallback(IUIElement*, IResourceBundle* resource_bundle);

    Application*  m_pUIApplication;
    ResourceBundle*  m_resource_bundle;
};

}