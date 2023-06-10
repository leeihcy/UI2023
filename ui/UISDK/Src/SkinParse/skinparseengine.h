#pragma once


namespace UI
{
interface IUIElement;
interface UIElement;
interface SkinDataSource;
class SkinRes;
class UIApplication;

class SkinParseEngine
{
public:
    SkinParseEngine(SkinRes* pSkinRes);
    ~SkinParseEngine();

    bool  Parse(SkinDataSource* pDataSource, LPCTSTR szXmlFile);
    void  NewChild(UIElement* pElement);

    static HRESULT  UIParseIncludeTagCallback(IUIElement*, ISkinRes* pSkinRes);

    UIApplication*  m_pUIApplication;
    SkinRes*  m_pSkinRes;
};

}