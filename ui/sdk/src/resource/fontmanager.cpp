#include "fontmanager.h"
#include "src/skin_parse/xml/xmlwrap.h"
#include "include/interface/imapattr.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "res_bundle.h"
#include "resource_manager.h"

namespace ui
{
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
	long dpi = ui::GetDpi();
	int nHeight = 0;
	nHeight = -(nSize * dpi / 72);  // MulDiv
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
	long dpi = ui::GetDpi();
	int nSize = 0;
	nSize = -(nHeight * 72 / dpi);  // 96  // MulDiv
	return nSize;
}

#if defined(OS_WIN)
int /*CALLBACK*/ IsFontExistEnumFontsProc(
						   const LOGFONT *lplf,     // logical-font data
						   const TEXTMETRIC *lptm,  // physical-font data
						   unsigned int dwType,     // font type
						   long lpData              // application-defined data
						   )
{
	bool* pbFind = (bool*)lpData;
	if (pbFind) {
		*pbFind = true;
	}
	return 0;
}
#endif

//
//  判断指定的字体在该系统中是否存在
//
//	Parameter
//		pszFaceName
//			[in]	要检测的字体名称
//
bool IsFontExist(const wchar_t* pszFaceName)
{
	if (nullptr == pszFaceName)
		return false;
#if defined(OS_WIN)
	HDC hDC = GetDC(nullptr);
	bool bFind = false;
	int nRet = EnumFonts(hDC, pszFaceName, IsFontExistEnumFontsProc, (long)&bFind);
	::ReleaseDC(nullptr, hDC);

	return bFind;
#else
    return false;
#endif
}

FontManager::FontManager(Resource* p):m_resFont(p)
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


long  FontManager::UIParseFontTagCallback(IUIElement* pElem, IResource* pSkinRes)
{
    IFontManager&  pFontMgr = pSkinRes->GetFontManager();
    return pFontMgr.GetImpl()->ParseNewElement(pElem->GetImpl());
}

long  FontManager::ParseNewElement(UIElement* pElem)
{
	FontTagElementInfo info;
	info.pXmlElement = pElem;
	pElem->AddRef();

	std::wstring bstrId = pElem->GetAttrib(XML_ID);
	if (!bstrId.empty())
	{
		info.strId = bstrId;
	}
	else
	{
		// TODO: 取当前xml名称作为id
	}

	m_listUIElement.Add(info);

#if defined(OS_WIN)
    HDC hDC = GetDC(nullptr);
#else
    HDC hDC = 0;
#endif
    // 遍历其子元素
    UIElementProxy  childElement = pElem->FirstChild();
    while (childElement)
    {
        this->OnNewChild(childElement.get(), hDC);
        childElement = childElement->NextElement();
    }
#if defined(OS_WIN)
    ReleaseDC(nullptr, hDC);
#endif
    return 0;
}

#if defined(OS_WIN)
int CALLBACK _EnumFontsProc(
                   CONST LOGFONT *lplf,     // logical-font data
                   CONST TEXTMETRIC *lptm,  // physical-font data
                   unsigned int dwType,            // font type
                   long lpData            // application-defined data
                           )
{
    BYTE* pbChatSet = (BYTE*)lpData;
    *pbChatSet = lplf->lfCharSet;
    return 0;
}
#endif


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
    const wchar_t* szText = nullptr;

    //	先加载默认配置，再遍历检查当前语言的配置
    IMapAttribute* pMapAttrib = UICreateIMapAttribute();
    pElem->GetAttribList(pMapAttrib);

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
    const wchar_t* szFaceId[] = { XML_FONT_FACENAME, XML_FONT_FACENAME2 };
    int nFaceIdCount = sizeof(szFaceId) / sizeof(const wchar_t*);

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
    ::memset( &lf, 0, sizeof(lf) );
    wcsncpy(lf.lfFaceName, strFaceName.c_str(), 31);

    lf.lfHeight = FontSize2Height(nFontHeight);
    lf.lfOrientation = lfOrientation;
    lf.lfEscapement = lf.lfOrientation;
    lf.lfWeight = bBold?FW_BOLD:FW_NORMAL;
    lf.lfItalic = bItalic?1:0;
    lf.lfUnderline = bUnderline?1:0;
    lf.lfStrikeOut = bStrikeout?1:0;
#if defined(OS_WIN)
    lf.lfQuality = bClearType ? CLEARTYPE_QUALITY : ANTIALIASED_QUALITY;
#endif

    // 获取这个字体的chatset。
    // 场景 ♪ 在xp下面，gdi显示成口，将chatset修改为gb2312之后显示正常
#if defined(OS_WIN)
    EnumFonts(hDC, lf.lfFaceName, _EnumFontsProc, (long)&lf.lfCharSet);
#else
    UIASSERT(false);
#endif

    if (false == m_resFont.InsertFont(strID.c_str(), &lf))
    {
        UI_LOG_WARN(_T("insert font failed. m_strID=%s, facename=%s"), 
            strID.c_str(), strFaceName.c_str());
    }

    pMapAttrib->Delete();
}
}