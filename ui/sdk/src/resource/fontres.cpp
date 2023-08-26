#include "fontres.h"
// #include "src/Renderlibrary/gdi/gdifont.h"
#include "include/interface/iuires.h"
#include "src/resource/res_bundle.h"

namespace ui {

int FontHeight2Size(int nHeight);

FontResItem::FontResItem() {
  m_pIFontResItem = nullptr;
#if 0
	m_pGdiFont = nullptr;
#endif
}
FontResItem::~FontResItem() {
#if 0
	SAFE_DELETE(m_pGdiFont);
#endif
  SAFE_DELETE(m_pIFontResItem);
}

IFontResItem *FontResItem::GetIFontResItem() {
  if (nullptr == m_pIFontResItem)
    m_pIFontResItem = new IFontResItem(this);

  return m_pIFontResItem;
}
//
// 通过结构体来快速赋值
//
void FontResItem::SetLogFont(LOGFONT *pLogFont) {
  if (!pLogFont)
    return;

  memcpy(&m_lf, pLogFont, sizeof(LOGFONT));
  strcpy(m_lf.lfFaceName, pLogFont->lfFaceName);
  m_lf.lfEscapement =
      m_lf.lfOrientation; // TODO: 实现字体角度的话，这两个值好像要相等
}

void FontResItem::ModifyFont(LOGFONT *pLogFont) {
  this->SetLogFont(pLogFont);
#if 0
	if (m_pGdiFont)
	{
		m_pGdiFont->ModifyFont(pLogFont);
	}
#endif
}

int FontResItem::GetFontSize() { return FontHeight2Size(m_lf.lfHeight); }

IRenderFont *FontResItem::GetFont(Resource *pSkinRes,
                                  GRAPHICS_RENDER_LIBRARY_TYPE eRenderType) {
#if 0
	switch(eRenderType)
	{
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		{
			if (nullptr == m_pGdiFont)
			{
				GDIRenderFont::CreateInstance((IRenderFont**)&m_pGdiFont);
				if (m_pGdiFont)
				{
					m_pGdiFont->SetOutRef((IRenderResource**)&m_pGdiFont);
					m_pGdiFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("gdi font create: %s, facename=%s, size=%d, Ptr=0x%08X"), m_strId.c_str(), m_lf.lfFaceName, m_lf.lfHeight, m_pGdiFont );
				}
                return (IRenderFont*)m_pGdiFont;
			}
			if (m_pGdiFont )
			{
				m_pGdiFont->AddRef();
			}
			return (IRenderFont*)m_pGdiFont;
		}
		break;

	default:
		return nullptr;
	}
#endif
  return nullptr;
}

bool FontResItem::IsMyRenderFont(IRenderFont *pRenderFont) {
#if 0
    if (m_pGdiFont == pRenderFont) 
    {
        return true;
    }
#endif
  return false;
}

FontRes::FontRes(Resource *pSkinRes) {
  m_pSkinRes = pSkinRes;
  m_pIFontRes = nullptr;
}
FontRes::~FontRes() {
  this->Clear();
  SAFE_DELETE(m_pIFontRes);
}
IFontRes &FontRes::GetIFontRes() {
  if (!m_pIFontRes)
    m_pIFontRes = new IFontRes(this);

  return *m_pIFontRes;
}
long FontRes::GetFontCount() { return (long)m_vFonts.size(); }
bool FontRes::GetFontResItem(long lIndex, IFontResItem **ppResItem) {
  if (nullptr == ppResItem)
    return false;

  FontResItem *pItem = this->GetFontItem(lIndex);
  if (nullptr == pItem)
    return false;

  *ppResItem = pItem->GetIFontResItem();
  return true;
}

FontResItem *FontRes::GetFontItem(int nIndex) {
  if (nIndex < 0)
    return nullptr;
  if (nIndex >= (int)m_vFonts.size())
    return nullptr;

  return m_vFonts[nIndex];
}

FontResItem *FontRes::GetFontItem(const std::string &strId) {
  std::vector<FontResItem *>::iterator iter = m_vFonts.begin();
  std::vector<FontResItem *>::iterator iterEnd = m_vFonts.end();

  for (; iter != iterEnd; iter++) {
    FontResItem *p = *iter;
    const char *szId = p->GetId();
    if (strId == szId)
      return p;
  }
  return nullptr;
}

bool FontRes::GetFont(const char *szFontId,
                      GRAPHICS_RENDER_LIBRARY_TYPE eRenderType,
                      IRenderFont **ppOut) {
  if (!szFontId || !ppOut)
    return false;

  std::vector<FontResItem *>::iterator iter = m_vFonts.begin();
  for (; iter != m_vFonts.end(); ++iter) {
    if (0 == strcmp((*iter)->GetId(), szFontId)) {
      // if (p->GetWParam() == 0 && p->GetLParam() == 0 )
      {
        *ppOut = (*iter)->GetFont(m_pSkinRes, eRenderType);
        return true;
      }
    }
  }

  // 获取失败，尝试向上一级资源获取
  if (m_pSkinRes->GetParentSkinRes()) {
    return m_pSkinRes->GetParentSkinRes()->GetFontRes().GetFont(
        szFontId, eRenderType, ppOut);
  }

  return false;
}

bool FontRes::GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType,
                             IRenderFont **ppFont) {
  if (nullptr == ppFont)
    return false;

  if (!m_vFonts.empty()) {
    if (m_vFonts[0]) {
      *ppFont = m_vFonts[0]->GetFont(m_pSkinRes, eRenderType);
      return true;
    }
  }

  Resource *pParentRes = m_pSkinRes->GetParentSkinRes();
  if (pParentRes) {
    return pParentRes->GetFontRes().GetDefaultFont(eRenderType, ppFont);
  }

  return false;
}

const char *FontRes::GetDefaultFontId() {
  if (0 == (int)m_vFonts.size())
    return nullptr;

  if (nullptr == m_vFonts[0])
    return nullptr;

  return m_vFonts[0]->GetId();
}

const char *FontRes::GetRenderFontId(IRenderFont *pFont) {
  std::vector<FontResItem *>::iterator iter = m_vFonts.begin();
  std::vector<FontResItem *>::iterator iterEnd = m_vFonts.end();

  for (; iter != iterEnd; iter++) {
    FontResItem *p = *iter;
    if (p->IsMyRenderFont(pFont)) {
      return p->GetId();
    }
  }
  return nullptr;
}

bool FontRes::InsertFont(const char *szId, LOGFONT *pLogFont) {
  FontResItem *pItem = this->InsertFont(szId, pLogFont, 0, 0);
  if (!pItem)
    return false;

  return true;
}

FontResItem *FontRes::InsertFont(const char *szId, LOGFONT *pLogFont,
                                 long wParam, long lParam) {
  if (nullptr == pLogFont) {
    UI_LOG_ERROR(_T("FontRes::InsertFont failed."));
    return nullptr;
  }
  // 	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
  // 	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();
  //
  // 	for( ; iter != iterEnd; iter++ )
  // 	{
  // 		FontResItem* p = *iter;
  //
  // 		if (strID == p->GetId() /*&& p->GetWParam() == wParam &&
  // p->GetLParam() == lParam */)
  // 		{
  // 			UI_LOG_WARN(_T("FontRes::InsertFont failed, insert item=%s"),
  // strID.c_str() ); 			return nullptr;
  // 		}
  // 	}

  FontResItem *pFontItem = new FontResItem;
  pFontItem->SetId(szId);
  pFontItem->SetLogFont(pLogFont);

  this->m_vFonts.push_back(pFontItem);
  return pFontItem;
}

bool FontRes::ModifyFont(const std::string &strID, LOGFONT *pLogFont) {
  if (nullptr == pLogFont) {
    UI_LOG_ERROR(_T("FontRes::ModifyFont failed."));
    return false;
  }
  std::vector<FontResItem *>::iterator iter = m_vFonts.begin();
  std::vector<FontResItem *>::iterator iterEnd = m_vFonts.end();

  for (; iter != iterEnd; iter++) {
    FontResItem *p = *iter;

    if (strID == p->GetId()) {
      p->ModifyFont(pLogFont);
      return true;
    }
  }
  UI_LOG_WARN(_T("FontRes::InsertFont failed, insert item=%s"), strID.c_str());
  return false;
}

bool FontRes::RemoveFont(const std::string &strID) {
  std::vector<FontResItem *>::iterator iter = m_vFonts.begin();
  std::vector<FontResItem *>::iterator iterEnd = m_vFonts.end();

  for (; iter != iterEnd; iter++) {
    FontResItem *p = *iter;

    if (strID == p->GetId()) {
      delete p;
      p = nullptr;
      m_vFonts.erase(iter);
      return true;
    }
  }
  return false;
}

void FontRes::Clear() {
  std::vector<FontResItem *>::iterator iter = m_vFonts.begin();
  std::vector<FontResItem *>::iterator iterEnd = m_vFonts.end();

  for (; iter != iterEnd; iter++) {
    FontResItem *p = *iter;
    delete p;
    p = nullptr;
  }

  m_vFonts.clear();
}

} // namespace ui