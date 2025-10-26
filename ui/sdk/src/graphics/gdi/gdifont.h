#pragma once
#include "..\renderresourceimpl.h"
#include "include/interface/graphics.h"

namespace ui
{

class  GDIRenderFont : public IRenderResourceImpl<IRenderFont>
{
protected:
	GDIRenderFont();

public:
	~GDIRenderFont();
	static  void  CreateInstance( IRenderFont** ppOutRef );
	virtual eGraphicsLibraryType 
            GetGraphicsRenderLibraryType() 
    {
            return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; 
    }

public:
	virtual bool  Load( LOGFONT* plogfont );
	virtual bool  ModifyFont(LOGFONT* plogfont);

	virtual void  Attach(HFONT hFont);
	virtual HFONT Detach();
	virtual bool  IsAttach();

	virtual SIZE  MeasureString( 
            const TCHAR* szText, 
            int nLimitWidth = -1 );
	virtual unsigned int  GetCaretHeight( );

	virtual HFONT GetHFONT();
	virtual bool  GetLogFont(LOGFONT* plf);

protected:
	void    DestroyFont();

private:
	HFONT   m_hFont;
	FontType   m_eFontType; // 标明该m_hFont的来源，用于区分如何释放
};
}
