#pragma once

namespace UI
{
interface IFontResItem;
interface IRenderFont;
interface IFontRes;
class SkinRes;

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
	LPCTSTR  GetId() { return m_strId.c_str(); }
	void  SetId(LPCTSTR szValue) { SETSTRING(m_strId, szValue); }
	LOGFONT* GetLogFont(){ return &m_lf; }
	void  GetFaceName( TCHAR* szOut ) { _tcscpy( szOut, m_lf.lfFaceName ); }
	void  SetFaceName( const String& str ) { _tcsncpy( m_lf.lfFaceName, str.c_str(), 31 ); m_lf.lfFaceName[31] = 0; }
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
	IRenderFont* GetFont(SkinRes* pSkinRes, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI );

	bool   IsMyRenderFont(IRenderFont* pRenderFont);

private:
    IFontResItem*  m_pIFontResItem;

	String   m_strId;
	LOGFONT  m_lf;

	GDIRenderFont*  m_pGdiFont;
};

//
//	font 列表
//
class FontRes
{
public:
    FontRes(SkinRes*  m_pSkinRes);
	~FontRes();

    IFontRes&  GetIFontRes();
	 
	long  GetFontCount();
	bool  GetFontResItem(long lIndex, IFontResItem** ppResItem);
	bool  GetFont(LPCTSTR szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut);
	bool  GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont);
	LPCTSTR  GetDefaultFontId();

public:
	FontResItem* GetFontItem( int nIndex );
	FontResItem* GetFontItem( const String& strID );
	bool InsertFont(LPCTSTR szId, LOGFONT* pLogFont );
    FontResItem* InsertFont(LPCTSTR szId, LOGFONT* pLogFont, WPARAM wParam, LPARAM lParam);
	bool ModifyFont(const String& strID, LOGFONT* pLogFont );
	bool RemoveFont(const String& strID );
	void Clear();

	LPCTSTR GetRenderFontId(IRenderFont* pFont);

private:
    SkinRes*  m_pSkinRes;
    IFontRes*  m_pIFontRes;
    vector<FontResItem*>  m_vFonts;
};
}