#include "stdafx.h"
#include "gdifont.h"
#include "src/Atl\image.h"


// 注：
// 多行绘制时，DT_WORDBREAK不加dt_editcontrol的话，
// 遇到长单词不会换行，直接显示在一行上面

GDIRenderFont::GDIRenderFont()
{
	m_hFont = nullptr;
	m_eFontType = CREATE;
}
GDIRenderFont::~GDIRenderFont()
{
	UI_LOG_DEBUG(_T("GDIRenderFont Delete. ptr=0x%08X"), this);
	this->DestroyFont();
}
//
//	根据FONT的创建方式来释放字体
//
void  GDIRenderFont::DestroyFont()
{
	if(m_eFontType == CREATE)
	{
		SAFE_DELETE_GDIOBJECT(m_hFont);
	}
	m_hFont = nullptr;
	m_eFontType = CREATE;
}
void  GDIRenderFont::CreateInstance(IRenderFont** ppOutRef)
{
	UIASSERT(nullptr != ppOutRef);
	if (nullptr == ppOutRef)
		return;

	GDIRenderFont* p = new GDIRenderFont();
    p->AddRef();
	*ppOutRef = p;
}

bool GDIRenderFont::Load( LOGFONT* plogfont )
{
	if (nullptr == plogfont)
	{
		UI_LOG_WARN(_T("plogfont == nullptr"));
		return false;
	}

	this->DestroyFont();
	m_hFont = ::CreateFontIndirect(plogfont);
	if (nullptr == m_hFont)
	{
		UI_LOG_WARN(_T("CreateFontIndirect failed, facename=%s"), plogfont->lfFaceName );
		return false;
	}
	m_eFontType = CREATE;
	return true;
}
bool GDIRenderFont::ModifyFont(LOGFONT* plogfont)
{
	bool bRet = this->Load(plogfont);
	if (false == bRet)
	{
		UI_LOG_WARN(_T("Load failed."));
	}
	else
	{
	//	__super::NotifyListener();
	}
	return bRet;
}

void  GDIRenderFont::Attach(HFONT hFont)
{
	this->DestroyFont();
	m_hFont = hFont;
	m_eFontType = ATTACH;
}
HFONT GDIRenderFont::Detach()
{
	HFONT hSave = m_hFont;
	m_hFont = nullptr;
	m_eFontType = CREATE;

	return hSave;
}

bool  GDIRenderFont::IsAttach()
{
	if (m_eFontType == ATTACH)
		return true;

	return false;
}

SIZE GDIRenderFont::MeasureString( const TCHAR* szText, int nLimitWidth)
{
	CSize sizeText(0, 0);
    uint nLength = (uint)wcslen(szText);
    if (!nLength)
        return sizeText;

	HDC hDC = Image::GetCacheDC();
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);

// 	if (-1 == nLimitWidth && nullptr == _tcsrchr(szText,_T('\n')))
// 	{
// 		::GetTextExtentPoint32(hDC, szText, nLength, &sizeText);
// 	}
// 	else
	{
		RECT rcText = {0,0,0x7FFFFFFF,0};  // 给{0,0}不行
		if (-1 != nLimitWidth )
		{
			rcText.right = nLimitWidth;
		}
		::DrawText(hDC, szText, nLength, &rcText, 
			DT_CALCRECT|DT_EDITCONTROL|DT_WORDBREAK|DT_NOPREFIX); // 默认DT_NOPREFIX
		sizeText.cx = rcText.right;
		sizeText.cy = rcText.bottom;
	}
	::SelectObject(hDC, hOldFont);
	Image::ReleaseCacheDC(hDC);

	return sizeText;
}

unsigned int GDIRenderFont::GetCaretHeight()
{
	HDC hDC = Image::GetCacheDC();
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);

	TEXTMETRIC  t;
	GetTextMetrics( hDC, &t );

	::SelectObject(hDC, hOldFont);
	Image::ReleaseCacheDC(hDC);
	return t.tmHeight + t.tmExternalLeading;
}

HFONT GDIRenderFont::GetHFONT()
{
	return m_hFont;
}

bool GDIRenderFont::GetLogFont(LOGFONT* plf)
{
	if (nullptr == plf || nullptr == m_hFont)
		return false;

	GetObject(m_hFont, sizeof(LOGFONT), plf);
	return true;
}