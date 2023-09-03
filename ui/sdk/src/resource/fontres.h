#pragma once
#include "include/inc.h"
#include "src/private_inc.h"
#include "include/util/windows.h"

namespace ui {
struct IFontResItem;
struct IRenderFont;
struct IFontRes;
class Resource;

//
//	一条字体信息
//
class GDIRenderFont;
class GdiplusRenderFont;

class FontResItem {
public:
  FontResItem();
  ~FontResItem();

  IFontResItem *GetIFontResItem();

public:
  const char *GetId() { return m_strId.c_str(); }
  void SetId(const char *szValue) { SETSTRING(m_strId, szValue); }
  LOGFONT *GetLogFont() { return &m_lf; }
#if 0
  void GetFaceName(char *szOut) { strcpy(szOut, m_lf.lfFaceName); }
  void SetFaceName(const std::string &str) {
    strncpy(m_lf.lfFaceName, str.c_str(), 31);
    m_lf.lfFaceName[31] = 0;
  }
#endif
  void SetFontHeight(int nHeight) { m_lf.lfHeight = nHeight; }
  int GetFontSize();
  void SetFontOrientation(int o) { m_lf.lfOrientation = o; }
  int GetFontOrientation() { return (int)m_lf.lfOrientation; }
  void SetFontBold(bool b) {
    b ? m_lf.lfWeight = FW_BOLD : m_lf.lfWeight = FW_NORMAL;
  }
  bool GetFontBold() { return m_lf.lfWeight == FW_BOLD; }
  void SetFontItalic(bool b) { b ? m_lf.lfItalic = 1 : m_lf.lfItalic = 0; }
  bool GetFontItalic() {
    if (m_lf.lfItalic == 0) {
      return false;
    } else {
      return true;
    }
  }
  void SetFontUnderline(bool b) {
    b ? m_lf.lfUnderline = 1 : m_lf.lfUnderline = 0;
  }
  bool GetFontUnderline() {
    if (m_lf.lfUnderline == 0) {
      return false;
    } else {
      return true;
    }
  }
  void SetFontStrikeout(bool b) {
    b ? m_lf.lfStrikeOut = 1 : m_lf.lfStrikeOut = 0;
  }
  bool GetFontStrikeout() {
    if (m_lf.lfStrikeOut == 0) {
      return false;
    } else {
      return true;
    }
  }

  void SetLogFont(LOGFONT *pLogFont);
  void ModifyFont(LOGFONT *pLogFont);
  IRenderFont *GetFont(Resource *pSkinRes,
                       GRAPHICS_RENDER_LIBRARY_TYPE eRenderType =
                           GRAPHICS_RENDER_LIBRARY_TYPE_GDI);

  bool IsMyRenderFont(IRenderFont *pRenderFont);

private:
  IFontResItem *m_pIFontResItem;

  std::string m_strId;
  LOGFONT m_lf;
#if 0
	GDIRenderFont*  m_pGdiFont;
#endif
};

//
//	font 列表
//
class FontRes {
public:
  FontRes(Resource *m_pSkinRes);
  ~FontRes();

  IFontRes &GetIFontRes();

  long GetFontCount();
  bool GetFontResItem(long lIndex, IFontResItem **ppResItem);
  bool GetFont(const char *szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType,
               /*__out*/ IRenderFont **ppOut);
  bool GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType,
                      /*__out*/ IRenderFont **ppFont);
  const char *GetDefaultFontId();

public:
  FontResItem *GetFontItem(int nIndex);
  FontResItem *GetFontItem(const std::string &strID);
  bool InsertFont(const char *szId, LOGFONT *pLogFont);
  FontResItem *InsertFont(const char *szId, LOGFONT *pLogFont, long wParam,
                          long lParam);
  bool ModifyFont(const std::string &strID, LOGFONT *pLogFont);
  bool RemoveFont(const std::string &strID);
  void Clear();

  const char *GetRenderFontId(IRenderFont *pFont);

private:
  Resource *m_pSkinRes;
  IFontRes *m_pIFontRes;
  std::vector<FontResItem *> m_vFonts;
};
} // namespace ui