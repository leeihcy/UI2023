#include "StdAfx.h"
#include "fontmanager.h"
#include "Src\SkinParse\xml\xmlwrap.h"
#include "Inc\Interface\imapattr.h"
#include "Inc\Interface\iuires.h"
#include "Inc\Interface\ixmlwrap.h"
#include "skinres.h"
#include "skinmanager.h"

//
//	将字体的大小转换成CreateFont需要使用的字体高度
//
//	Parameter
//		nSize	
//			[in]	字体界面上显示的大小
//
//	Return
//		返回字体所对应的height;
//
int FontSize2Height(int nSize)
{
	long dpi = UI::GetDpi();
	int nHeight = 0;
	nHeight = -MulDiv(nSize, dpi, 72);
	return nHeight;
}

//
//	将字体LOGFONT.lfHeight转换成字体大小
//
//	Parameter
//		nHeight	
//			[in]	字体界面上显示的大小
//
//	Return
//		返回字体所对应的size;
//
int FontHeight2Size(int nHeight)
{
	long dpi = UI::GetDpi();
	int nSize = 0;
	nSize = -MulDiv(nHeight, 72, dpi);  // 96
	return nSize;
}

int CALLBACK IsFontExistEnumFontsProc(
						   CONST LOGFONT *lplf,     // logical-font data
						   CONST TEXTMETRIC *lptm,  // physical-font data
						   DWORD dwType,            // font type
						   LPARAM lpData            // application-defined data
						   )
{
	BOOL* pbFind = (BOOL*)lpData;
	if (pbFind)
	{
		*pbFind = TRUE;
	}
	return 0;
}

//
//  判断指定的字体在该系统中是否存在
//
//	Parameter
//		pszFaceName
//			[in]	要检测的字体名称
//
BOOL IsFontExist(LPCTSTR pszFaceName)
{
	if (nullptr == pszFaceName)
		return FALSE;

	HDC hDC = GetDC(nullptr);
	BOOL bFind = FALSE;
	int nRet = EnumFonts(hDC, pszFaceName, IsFontExistEnumFontsProc, (LPARAM)&bFind);
	::ReleaseDC(nullptr, hDC);

	return bFind;
}

FontManager::FontManager(SkinRes* p):m_resFont(p)
{
    m_pIFontManager = nullptr;
	m_pSkinRes = p;
}

FontManager::~FontManager(void)
{
	this->Clear();
    SAFE_DELETE(m_pIFontManager);

	_MyListItem* pItem = m_listUIElement.GetFirst();
	while (pItem)
	{
		(**pItem).pXmlElement->Release();
		pItem = pItem->GetNext();
	}
	m_listUIElement.Clear();
}

IFontManager&  FontManager::GetIFontManager()
{
    if (!m_pIFontManager)
        m_pIFontManager = new IFontManager(this);

    return *m_pIFontManager;
}

/*
**	清除所有图片资源
*/
void FontManager::Clear()
{
	m_resFont.Clear();
}

int FontManager::GetFontCount()
{
	return m_resFont.GetFontCount();
}
IFontResItem*  FontManager::GetFontItemInfo(int nIndex)
{
	FontResItem* pItem = m_resFont.GetFontItem(nIndex);
	if (nullptr == pItem)
		return nullptr;

    return pItem->GetIFontResItem();
}

FontRes&  FontManager::GetFontRes()
{
	return m_resFont;
}


HRESULT  FontManager::UIParseFontTagCallback(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IFontManager&  pFontMgr = pSkinRes->GetFontManager();
    return pFontMgr.GetImpl()->ParseNewElement(pElem->GetImpl());
}

HRESULT  FontManager::ParseNewElement(UIElement* pElem)
{
	FontTagElementInfo info;
	info.pXmlElement = pElem;
	pElem->AddRef();

	CComBSTR bstrId;
	if (pElem->GetAttrib(XML_ID, &bstrId))
	{
		info.strId = bstrId;
	}
	else
	{
		// TODO: 取当前xml名称作为id
	}

	m_listUIElement.Add(info);

    HDC hDC = GetDC(nullptr);
    // 遍历其子元素
    UIElementProxy  childElement = pElem->FirstChild();
    while (childElement)
    {
        this->OnNewChild(childElement.get(), hDC);
        childElement = childElement->NextElement();
    }
    ReleaseDC(nullptr, hDC);

    return S_OK;
}

int CALLBACK _EnumFontsProc(
                   CONST LOGFONT *lplf,     // logical-font data
                   CONST TEXTMETRIC *lptm,  // physical-font data
                   DWORD dwType,            // font type
                   LPARAM lpData            // application-defined data
                           )
{
    BYTE* pbChatSet = (BYTE*)lpData;
    *pbChatSet = lplf->lfCharSet;
    return 0;
}



// 支持多语言环境下的设置
//  <font id="hyperlink.font" face="微软雅黑">
//      <zh_cn size="10"/>
//      <en_us size="8"/>
//  </font>
//  上面这种方式有个缺点：不支持动态切换语言。在切换语言的时候，还要重新加载字体。
//  重新修改为：在控件的字体中使用#国际化#字段来匹配不同的字体id
//  
//   <font id = "hyperlink.font.zh_cn" face = "微软雅黑" size = "10" / >
//   <font id = "hyperlink.font.en_us" face = "微软雅黑" size = "8" / >
//
//   <i18n>
//         <string id="hyperlink.font" 
//                 zh_cn="hyperlink.font.zh_cn"
//                 en_us="hyperlink.font.en_us"/>
//   </i18n>
//
//   <prop key = "text.render" type = "colorlist" font = "#hyperlink.font"
//
//
void  FontManager::OnNewChild(UIElement* pElem, HDC hDC)
{
    LPCTSTR szText = nullptr;

    //	先加载默认配置，再遍历检查当前语言的配置
    IMapAttribute* pMapAttrib = nullptr;
    pElem->GetAttribList(&pMapAttrib);

    String  strID;
    String  strFaceName;
    int   nFontHeight = 9;
    int   lfOrientation = 0;
    bool  bBold = false;
    bool  bItalic = false;
    bool  bUnderline = false;
    bool  bStrikeout = false;
    bool  bClearType = false;
    
    szText = pMapAttrib->GetAttr(XML_ID, true);
    if (szText)
        strID = szText;

    // 1. 支持前一个字体不存在时，使用后一个字体
    // 2. 支持不同的语言使用不同的字体
    LPCTSTR szFaceId[] = { XML_FONT_FACENAME, XML_FONT_FACENAME2 };
    int nFaceIdCount = sizeof(szFaceId) / sizeof(LPCTSTR);

    for (int i = 0; i < nFaceIdCount; ++i)
    {
        szText = pMapAttrib->GetAttr(szFaceId[i], true);
        if (!szText)
            continue;

        if (IsFontExist(szText))
        {
            strFaceName = szText;
            break;
        }
    }

    pMapAttrib->GetAttr_int(XML_FONT_HEIGHT, true, &nFontHeight);
    pMapAttrib->GetAttr_int(XML_FONT_ORIENTATION, true, &lfOrientation);
    pMapAttrib->GetAttr_bool(XML_FONT_BOLD, true, &bBold);
    pMapAttrib->GetAttr_bool(XML_FONT_ITALIC, true, &bItalic);
    pMapAttrib->GetAttr_bool(XML_FONT_UNDERLINE, true, &bUnderline);
    pMapAttrib->GetAttr_bool(XML_FONT_STRIKEOUT, true, &bStrikeout);

	// gdi的微软雅黑不使用cleartype会很模糊
	bClearType = true;
    pMapAttrib->GetAttr_bool(XML_FONT_CLEARTYPE, true, &bClearType);

    if (strFaceName.empty() )
        strFaceName = _T("SYSTEM");
    
    LOGFONT lf;
    ::ZeroMemory( &lf, sizeof(lf) );
    _tcsncpy(lf.lfFaceName, strFaceName.c_str(), 31);

    lf.lfHeight = FontSize2Height(nFontHeight);
    lf.lfOrientation = lfOrientation;
    lf.lfEscapement = lf.lfOrientation;
    lf.lfWeight = bBold?FW_BOLD:FW_NORMAL;
    lf.lfItalic = bItalic?1:0;
    lf.lfUnderline = bUnderline?1:0;
    lf.lfStrikeOut = bStrikeout?1:0;
    lf.lfQuality = bClearType ? CLEARTYPE_QUALITY : ANTIALIASED_QUALITY;

    // 获取这个字体的chatset。
    // 场景 ♪ 在xp下面，gdi显示成口，将chatset修改为gb2312之后显示正常
    
    EnumFonts(hDC, lf.lfFaceName, _EnumFontsProc, (WPARAM)&lf.lfCharSet);
    

    if (false == m_resFont.InsertFont(strID.c_str(), &lf))
    {
        UI_LOG_WARN(_T("insert font failed. m_strID=%s, facename=%s"), 
            strID.c_str(), strFaceName.c_str());
    }

    SAFE_RELEASE(pMapAttrib);
}
