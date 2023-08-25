#pragma once


namespace ui
{
struct IUIElement;
struct UIElement;
struct SkinDataSource;
class Resource;
struct IResource;
class Application;

class SkinParseEngine
{
public:
    SkinParseEngine(Resource* pSkinRes);
    ~SkinParseEngine();

    bool  Parse(SkinDataSource* pDataSource, const wchar_t* szXmlFile);
    void  NewChild(UIElement* pElement);

    static int UIParseIncludeTagCallback(IUIElement*, IResource* pSkinRes);

    Application*  m_pUIApplication;
    Resource*  m_pSkinRes;
};

}