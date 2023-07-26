#pragma once
#include "inc.h"
#include "private_inc.h"

namespace ui
{
struct IFontResItem;
struct IRenderFont;
struct IFontRes;
class ResBundle;

//
//	一条字体信息
//
class GDIRenderFont;
class GdiplusRenderFont;

class FontResItem
{
public:
	FontResItem();
	~FontResItem();

    IFontResItem*  GetIFontResItem();

public:
	const wchar_t*  GetId() { return m_strId.c_str(); }
	void  SetId(const wchar_t* szValue) { SETSTRING(m_strId, szValue); }
	LOGFONT* GetLogFont(){ return &m_lf; }
	void  GetFaceName( wchar_t* szOut ) { wcscpy( szOut, m_lf.lfFaceName ); }
	void  SetFaceName( const std::wstring& str ) { wcsncpy( m_lf.lfFaceName, str.c_str(), 31 ); m_lf.lfFaceName[31] = 0; }
	void  SetFontHeight( int nHeight ) { m_lf.lfHeight = nHeight; }
	int   GetFontSize();
	void  SetFontOrientation( int o ) { m_lf.lfOrientation=o; }
	int   GetFontOrientation(){return (int)m_lf.lfOrientation; }
	void  SetFontBold( bool b ) { b ? m_lf.lfWeight=FW_BOLD : m_lf.lfWeight=FW_NORMAL; }
	bool  GetFontBold( ) { return m_lf.lfWeight==FW_BOLD; }
	void  SetFontItalic( bool b ) { b ? m_lf.lfItalic=1:m_lf.lfItalic=0; }
	bool  GetFontItalic( ) { if (m_lf.lfItalic==0){return false;}else{return true;} }
	void  SetFontUnderline( bool b ) { b ? m_lf.lfUnderline=1:m_lf.lfUnderline=0; }
	bool  GetFontUnderline( ) { if (m_lf.lfUnderline==0){return false;}else{return true;} }
	void  SetFontStrikeout( bool b ) { b ? m_lf.lfStrikeOut=1:m_lf.lfStrikeOut=0; }
	bool  GetFontStrikeout( ) { if (m_lf.lfStrikeOut==0){return false;}else{return true;} }

	void   SetLogFont( LOGFONT* pLogFont );
	void   ModifyFont( LOGFONT* pLogFont );
	IRenderFont* GetFont(ResBundle* pSkinRes, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI );

	bool   IsMyRenderFont(IRenderFont* pRenderFont);

private:
    IFontResItem*  m_pIFontResItem;

	std::wstring   m_strId;
	LOGFONT  m_lf;
#if 0
	GDIRenderFont*  m_pGdiFont;
#endif
};

//
//	font 列表
//
class FontRes
{
public:
    FontRes(ResBundle*  m_pSkinRes);
	~FontRes();

    IFontRes&  GetIFontRes();
	 
	long  GetFontCount();
	bool  GetFontResItem(long lIndex, IFontResItem** ppResItem);
	bool  GetFont(const wchar_t* szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, /*__out*/ IRenderFont** ppOut);
	bool  GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, /*__out*/ IRenderFont** ppFont);
	const wchar_t*  GetDefaultFontId();

public:
	FontResItem* GetFontItem( int nIndex );
	FontResItem* GetFontItem( const String& strID );
	bool InsertFont(const wchar_t* szId, LOGFONT* pLogFont );
    FontResItem* InsertFont(const wchar_t* szId, LOGFONT* pLogFont, long wParam, long lParam);
	bool ModifyFont(const String& strID, LOGFONT* pLogFont );
	bool RemoveFont(const String& strID );
	void Clear();

	const wchar_t* GetRenderFontId(IRenderFont* pFont);

private:
    ResBundle*  m_pSkinRes;
    IFontRes*  m_pIFontRes;
    std::vector<FontResItem*>  m_vFonts;
};
}