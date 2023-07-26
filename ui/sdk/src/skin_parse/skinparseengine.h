#pragma once


namespace ui
{
struct IUIElement;
struct UIElement;
struct SkinDataSource;
class ResBundle;
class UIApplication;

class SkinParseEngine
{
public:
    SkinParseEngine(ResBundle* pSkinRes);
    ~SkinParseEngine();

    bool  Parse(SkinDataSource* pDataSource, const wchar_t* szXmlFile);
    void  NewChild(UIElement* pElement);

    static long UIParseIncludeTagCallback(IUIElement*, IResBundle* pSkinRes);

    UIApplication*  m_pUIApplication;
    ResBundle*  m_pSkinRes;
};

}