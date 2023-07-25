#pragma once


namespace ui
{
struct IUIElement;
struct UIElement;
struct SkinDataSource;
class SkinRes;
class UIApplication;

class SkinParseEngine
{
public:
    SkinParseEngine(SkinRes* pSkinRes);
    ~SkinParseEngine();

    bool  Parse(SkinDataSource* pDataSource, const wchar_t* szXmlFile);
    void  NewChild(UIElement* pElement);

    static long UIParseIncludeTagCallback(IUIElement*, ISkinRes* pSkinRes);

    UIApplication*  m_pUIApplication;
    SkinRes*  m_pSkinRes;
};

}