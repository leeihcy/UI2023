#ifndef GDIPLUSFONT_H_EE7FC00E_CB32_4c6f_B308_464F36137108
#define GDIPLUSFONT_H_EE7FC00E_CB32_4c6f_B308_464F36137108
#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"

namespace Gdiplus { class Font; }
namespace UI
{
class  GdiplusRenderFont : public IRenderResourceImpl<IRenderFont>
{
protected:
	GdiplusRenderFont();

public:
	~GdiplusRenderFont();
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS; }
	static  void  CreateInstance( IRenderFont** ppOutRef );

public:
	virtual bool    Load( LOGFONT* plogfont );
	virtual bool    ModifyFont(LOGFONT* plogfont);

	virtual void    Attach(HFONT hFont);
	virtual HFONT   Detach();
	virtual bool    IsAttach();

	virtual UINT    GetCaretHeight( );
	virtual SIZE    MeasureString( const TCHAR* szText, int nLimitWidth = -1 );

	virtual HFONT   GetHFONT();
	Gdiplus::Font*  GetFont() { return m_pFont; }
	virtual bool    GetLogFont(LOGFONT* plf);

    Gdiplus::TextRenderingHint  GetTextRenderingHint()
    {
        return m_eTextRenderingHint;
    }

private:
	Gdiplus::Font*  m_pFont;
    Gdiplus::TextRenderingHint  m_eTextRenderingHint;
	HFONT    m_hFontAttach;
	HFONT    m_hFontForGDI;
};
}

#endif // GDIPLUSFONT_H_EE7FC00E_CB32_4c6f_B308_464F36137108