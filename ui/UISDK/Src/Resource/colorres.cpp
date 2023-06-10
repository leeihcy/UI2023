#include "stdafx.h"
#include "colorres.h"
#include "Inc/Interface/iuires.h"
#include "Inc/Interface/imapattr.h"
#include "Src/Atl/image.h"
#include "uiresource.h"
#include "skinres.h"


ColorResItem::ColorResItem()
{
    m_pColor = nullptr;
	m_bUseSkinHLS = true;
	m_pOriginColorValue = nullptr;
    m_pIColorResItem = nullptr;
}
ColorResItem::~ColorResItem()
{
	SAFE_DELETE(m_pOriginColorValue);
    SAFE_RELEASE(m_pColor);
    SAFE_DELETE(m_pIColorResItem);
}
IColorResItem*  ColorResItem::GetIColorResItem()
{
    if (nullptr == m_pIColorResItem)
        m_pIColorResItem = new IColorResItem(this);

    return m_pIColorResItem;
}
LPCTSTR ColorResItem::GetColorString()
{
	return m_strColor.c_str();
}
bool  ColorResItem::IsMyColor(Color* p)
{
	if (m_pColor && p == m_pColor)
		return true;

	return false;
}
const String& ColorResItem::GetColorStringRef()
{ 
	return m_strColor; 
}

void  ColorResItem::SetId(LPCTSTR szId)
{
	if (szId)
		m_strId = szId;
	else
		m_strId.clear();
}
LPCTSTR  ColorResItem::GetId()
{
	return m_strId.c_str();
}
const String&  ColorResItem::GetIdRef()
{
	return m_strId;
}
void  ColorResItem::SetUseSkinHLS(bool b)
{ 
	m_bUseSkinHLS = b;
}
bool  ColorResItem::GetUseSkinHLS()
{
	return m_bUseSkinHLS;
}

bool ColorResItem::GetColor(Color** ppColor, bool* pbFirstTimeCreate) 
{
	if (nullptr == ppColor)
		return false;

    if (nullptr == m_pColor)
    {
        m_pColor = Color::CreateInstance(Util::TranslateRGB(m_strColor.c_str(), XML_SEPARATOR));
        
        if (pbFirstTimeCreate)
            *pbFirstTimeCreate = true;
        
    }

    m_pColor->AddRef();
	*ppColor = m_pColor;
	return  true;
}

void ColorResItem::SetColor(const String& strColor) 
{
	m_strColor = strColor;

	if (m_pColor)
		m_pColor->ReplaceRGB(Util::TranslateRGB(m_strColor.c_str(), XML_SEPARATOR));
}

void ColorResItem::SetAttribute(IMapAttribute* pMapAttrib)
{
	bool bUseSkinHLS = true;
    pMapAttrib->GetAttr_bool(XML_COLOR_USESKINHLS, true, &bUseSkinHLS);
	this->SetUseSkinHLS(bUseSkinHLS);
}

bool ColorResItem::ModifyHLS( short h, short l, short s, int nFlag)
{
	if (false == m_bUseSkinHLS)
		return true;

	if (m_pColor)
	{
		if (nullptr == m_pOriginColorValue )
		{
			m_pOriginColorValue = new COLORREF;
			*m_pOriginColorValue = m_pColor->m_col;
		}

		BYTE R = GetRValue(*m_pOriginColorValue);
		BYTE G = GetGValue(*m_pOriginColorValue);
		BYTE B = GetBValue(*m_pOriginColorValue);

		if (ChangeColorHLS(R,G,B,h,l,s,nFlag))
			m_pColor->ReplaceRGB(RGB(R,G,B));
	}
	return true;
}

ColorRes::ColorRes(SkinRes* p)
{
    m_pSkinRes = p; 
    m_pIColorRes = nullptr; 
}

ColorRes::~ColorRes()
{
	this->Clear();
    SAFE_DELETE(m_pIColorRes);
}

IColorRes&  ColorRes::GetIColorRes()
{
    if (!m_pIColorRes)
        m_pIColorRes = new IColorRes(this);

    return *m_pIColorRes;
}
long ColorRes::GetColorCount() 
{
	return (long)m_vColors.size();
}

IColorResItem*  ColorRes::GetColorResItem(long lIndex)
{
	ColorResItem* pItem = GetColorItem(lIndex);
	if (nullptr == pItem)
		return nullptr;

	return pItem->GetIColorResItem();
}
ColorResItem* ColorRes::GetColorItem(int nIndex)
{
	if (nIndex < 0)
		return nullptr;
	if (nIndex >= (int)m_vColors.size() )
		return nullptr;

	return m_vColors[nIndex];
}

ColorResItem* ColorRes::GetColorItem( const String& strID )
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for (; iter != iterEnd; iter++)
	{
		ColorResItem* p = *iter;
		if (p->GetIdRef() == strID )
		{
			return p;
		}
	}
	return nullptr;
}

void ColorRes::GetColor(LPCTSTR szColorId, Color** pp)
{
	if (!szColorId || !szColorId[0] || !pp)
		return;

     // 直接翻译，不根据ID去映射
    if (szColorId[0] == _T('#'))
    {
        COLORREF color = Util::TranslateHexColor(szColorId+1); 
        *pp = Color::CreateInstance(color);
        return;
    }
    else if (szColorId[0] == _T('0') && szColorId[1] == _T('x'))
    {
        COLORREF color = Util::TranslateHexColor(szColorId+2);
        *pp = Color::CreateInstance(color);
        return;
    }
    else if (_tcsstr(szColorId, TEXT(",")))
    {
        COLORREF color = Util::TranslateColor(szColorId);
        *pp = Color::CreateInstance(color);
        return;
    }

	ColorResItem* pItem = this->GetColorItem(szColorId);
	if (nullptr == pItem)
	{
		UI_LOG_WARN( _T("failed, id=%s"), szColorId);
		return;
	}

	bool bFirstTimeCreate = false;
    pItem->GetColor(pp, &bFirstTimeCreate);
	if (bFirstTimeCreate && pItem->GetUseSkinHLS())
	{
        // 检查当前皮肤的HLS
        if (m_pSkinRes && m_pSkinRes->GetHLSInfo())
        {
            SKIN_HLS_INFO* pHLSInfo = m_pSkinRes->GetHLSInfo();
            pItem->ModifyHLS(pHLSInfo->h, pHLSInfo->l, pHLSInfo->s, pHLSInfo->nFlag);
        }
	}
}


//
// 从文件中加载一项(由CXmlImageParse::load_from_file中调用)
//
bool ColorRes::LoadItem(IMapAttribute* pMapAttrib, LPCTSTR szValue)
{
	String strID;

    LPCTSTR szText = pMapAttrib->GetAttr(XML_ID, true);
	if (szText)
		strID = szText;

	ColorResItem* pItem = nullptr;
	if (this->InsertColor(strID, szValue, &pItem))
	{
		pItem->SetAttribute(pMapAttrib);
		return true;
	}
	else
	{
		UI_LOG_WARN( _T("insert image m_strId=%s, path=%s failed."), strID.c_str(), szValue);
		return false;
	}
}
bool ColorRes::InsertColor(const String& strID, LPCTSTR szColor, ColorResItem** pItem )
{
    if (nullptr == szColor)
        return false;

	ColorResItem* p = this->GetColorItem(strID);
	if (p)
	{
		UI_LOG_WARN(_T("ColorRes::InsertColor failed, insert item=%s, color=%s"), strID.c_str(), szColor);
		return false;
	}

	ColorResItem*  pColorItem = new ColorResItem;
	pColorItem->SetId(strID.c_str());
	pColorItem->SetColor(szColor);

	this->m_vColors.push_back(pColorItem); 

	if (pItem)
		*pItem = pColorItem;
	
	return true;
}

bool ColorRes::ModifyColor( const String& strID, const String& strColor )
{
	ColorResItem* p = this->GetColorItem(strID);
	if (p)
	{
		p->SetColor(strColor);
		return true;
	}

	return false;
}

bool ColorRes::RemoveColor(const String& strID )
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		ColorResItem* p = *iter;
		if (p->GetIdRef() == strID )
		{
			delete p;
			p = nullptr;
			m_vColors.erase(iter);
			return true;
		}
	}

	return false;
}
void ColorRes::Clear()
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		ColorResItem* p = *iter;
		delete p;
		p = nullptr;
	}

	m_vColors.clear();
}

bool ColorRes::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		ColorResItem* p = *iter;
		p->ModifyHLS(h,l,s,nFlag);
	}

	return true;
}

LPCTSTR  ColorRes::GetColorId(Color* p)
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	for( ; iter != m_vColors.end(); ++iter)
	{
		if ((*iter)->IsMyColor(p))
			return (*iter)->GetId();
	}

	return nullptr;
}