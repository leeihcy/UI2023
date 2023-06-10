#include "stdafx.h"
/*#include "gdiplusfix.h"*/
#include "gdiplusfont.h"
#include "UISDK\Kernel\Src\atl\image.h"

GdiplusRenderFont::GdiplusRenderFont()
{
	m_pFont = NULL;
	m_hFontAttach = NULL;
	m_hFontForGDI = NULL;
    m_eTextRenderingHint = Gdiplus::TextRenderingHintClearTypeGridFit;
}
GdiplusRenderFont::~GdiplusRenderFont()
{
	UI_LOG_DEBUG(_T("GdiplusRenderFont Delete. ptr=0x%08X"), this);

	SAFE_DELETE(m_pFont);
	SAFE_DELETE_GDIOBJECT(m_hFontForGDI);
	m_hFontAttach = NULL;
}
void  GdiplusRenderFont::CreateInstance(IRenderFont** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if(NULL == ppOutRef)
		return ;

	GdiplusRenderFont* p = new GdiplusRenderFont();
    p->AddRef();
	*ppOutRef = p;
}

bool  GdiplusRenderFont::Load(LOGFONT* plogfont)
{
	if (NULL == plogfont)
		return false;

	SAFE_DELETE(m_pFont);

	INT  nSize = abs(Util::FontHeight2Size(plogfont->lfHeight));
	//Gdiplus::FontStyle style = Gdiplus::FontStyleRegular;   // FontStyle不支持|=，改用int代替
	int  nStyle = Gdiplus::FontStyleRegular;

	if(plogfont->lfWeight >= FW_BOLD)
		nStyle |= (int)Gdiplus::FontStyleBold;
	if(plogfont->lfItalic)
		nStyle |= (int)Gdiplus::FontStyleItalic;
	if(plogfont->lfStrikeOut)
		nStyle |= (int)Gdiplus::FontStyleStrikeout;
	if(plogfont->lfUnderline)
		nStyle |= (int)Gdiplus::FontStyleUnderline;

    m_eTextRenderingHint = Gdiplus::TextRenderingHintClearTypeGridFit;
    switch (plogfont->lfQuality)
    {
    case CLEARTYPE_QUALITY:
    case CLEARTYPE_NATURAL_QUALITY:
        m_eTextRenderingHint = Gdiplus::TextRenderingHintClearTypeGridFit;
        break;

    case ANTIALIASED_QUALITY:
        m_eTextRenderingHint = Gdiplus::TextRenderingHintAntiAliasGridFit;
        break;

    default:
        m_eTextRenderingHint = Gdiplus::TextRenderingHintSystemDefault;
        break;

    }
        
	m_pFont = new Gdiplus::Font(plogfont->lfFaceName, Gdiplus::REAL(nSize), nStyle); 
	return true;
}
bool  GdiplusRenderFont::ModifyFont(LOGFONT* plogfont)
{
	bool b = this->Load(plogfont);
	if (b)
	{
	//	__super::NotifyListener();
	}
	return b;
}

void  GdiplusRenderFont::Attach(HFONT hFont)
{
	if( NULL == hFont )
		return;

	m_hFontAttach = hFont;

	LOGFONT lf;
	::GetObject(hFont, sizeof(LOGFONT),&lf);
	this->Load(&lf);
}
HFONT GdiplusRenderFont::Detach()
{
	HFONT hSave = m_hFontAttach;
	m_hFontAttach = NULL;
	SAFE_DELETE(m_pFont);

	return hSave;
}

bool  GdiplusRenderFont::IsAttach()
{
	if (m_hFontAttach)
		return true;

	return false;
}


SIZE GdiplusRenderFont::MeasureString( const TCHAR* szText, int nLimitWidth)
{
	CSize sizeText(0, 0);

	if (NULL == m_pFont)
		return sizeText;

	if (NULL == szText || _tcslen(szText)==0)
		return sizeText;

	HDC hDC = Image::GetCacheDC();
	Gdiplus::Graphics  g(hDC);

	const Gdiplus::StringFormat* pStringFormat = Gdiplus::StringFormat::GenericTypographic();  // 不计算GDIPLUS的左右间距
    Gdiplus::StringFormat format(pStringFormat);
    format.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);  // 计算最后的空格
    pStringFormat = &format;

	if (-1 != nLimitWidth)
	{
		Gdiplus::RectF  layoutRect((Gdiplus::REAL)0,(Gdiplus::REAL)0, (Gdiplus::REAL)nLimitWidth, (Gdiplus::REAL)0 );
		Gdiplus::RectF  boundingBox;

		g.MeasureString( szText, -1/*_tcslen(szText)*/, m_pFont, layoutRect, pStringFormat, &boundingBox, NULL,NULL );

		sizeText.cx = (int)boundingBox.Width+1;
		sizeText.cy = (int)boundingBox.Height+1;
	}
	else
	{
		Gdiplus::PointF origin((Gdiplus::REAL)0,(Gdiplus::REAL)0 );
		Gdiplus::RectF  boundingBox;

		g.MeasureString( szText, -1/*_tcslen(szText)*/, m_pFont, origin, pStringFormat, &boundingBox );

		sizeText.cx = (int)boundingBox.Width+1;
		sizeText.cy = (int)boundingBox.Height+1;
	}

	Image::ReleaseCacheDC(hDC);
	return sizeText;
}


UINT GdiplusRenderFont::GetCaretHeight( )
{
	if (NULL == m_pFont)
		return 0;

	HDC hDC = ::Image::GetCacheDC();
	Gdiplus::Graphics g(hDC);
	UINT nRet = (UINT)m_pFont->GetHeight(&g);
	Image::ReleaseCacheDC(hDC);

	return nRet;
}


HFONT GdiplusRenderFont::GetHFONT()
{
	HFONT hFont = m_hFontAttach;
	if (NULL == hFont)  
	{
		if (NULL == m_hFontForGDI)  // TODO: 如果m_pFont属性改变，那m_hFontForGDI也会一起改变吗？是否是每次获取前都将m_hFontForGDI干掉？
		{
			LOGFONT lf;
			this->GetLogFont(&lf);
			m_hFontForGDI = ::CreateFontIndirect(&lf);  // 析构时，释放
		}
		hFont = m_hFontForGDI;
	}
	return hFont;
}

bool GdiplusRenderFont::GetLogFont(LOGFONT* plf)
{
	if (NULL == plf || NULL == m_pFont)
		return false;

	HDC hCacheDC = Image::GetCacheDC();
	Gdiplus::Graphics g(hCacheDC);

#ifdef _UNICODE
	m_pFont->GetLogFontW(&g, plf);	
#else
	m_pFont->GetLogFontA(&g, plf);
#endif

	Image::ReleaseCacheDC(hCacheDC);
	return true;
}
