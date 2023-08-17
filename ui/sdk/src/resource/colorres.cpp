#include "colorres.h"
#include "include/interface/iuires.h"
#include "include/interface/imapattr.h"
// #include "Src/Atl/image.h"
#include "uiresource.h"
#include "res_bundle.h"

namespace ui
{

//
//	根据参数中提供的原始数据pSaveBits，将自己偏移wNewH色调
//
bool ChangeColorHue(byte& R, byte& G, byte& B, short h, bool bOffsetOrReplace)
{
    if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
        return false;

    Color c(R,G,B,255);

    HSL hsl;
    c.GetHSL(hsl);

    if (bOffsetOrReplace)
        hsl.hue += h;
    else
        hsl.hue = h;

    while(hsl.hue < MIN_HUE_VALUE)
        hsl.hue += MAX_HUE_VALUE;
    while (hsl.hue >= MAX_HUE_VALUE)
        hsl.hue -= MAX_HUE_VALUE;
    
    c.SetHSL(hsl);

    R = c.r;
    G = c.g;
    B = c.b;

    return true;
}

void ChangeColorLuminance(byte& R, byte& G, byte& B, short l, float dL)  // dL = l/100;
{
    if (l > 0)  // 相当于是在背景图上面盖了一张全白的图片（alpha为dL)
    {  
        R = R + (byte)((255 - R) * dL);  
        G = G + (byte)((255 - G) * dL);  
        B = B + (byte)((255 - B) * dL);  
    }  
    else if (l < 0)  // 相当于是在背景图上面盖了一张全黑的图片(alpha为dL)
    {  
        R = R + (byte)(R * dL);  
        G = G + (byte)(G * dL);   
        B = B + (byte)(B * dL);  
    }  
#define CHECK_RGB_RANGE(x)  \
    if (x>255) x = 255; \
    if (x<0)   x = 0;

    CHECK_RGB_RANGE(R);
    CHECK_RGB_RANGE(G);
    CHECK_RGB_RANGE(B);
}

bool ChangeColorHueAndSaturation(byte& R, byte& G, byte& B, short h, bool bOffsetOrReplace, short s, float dS)
{
    if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
        return false;

    Color c(R,G,B,255);

    HSL hsl;
    c.GetHSL(hsl);

    // hue
    if (bOffsetOrReplace)
        hsl.hue += h;
    else
        hsl.hue = h;

    while(hsl.hue < MIN_HUE_VALUE)
        hsl.hue += MAX_HUE_VALUE;
    while (hsl.hue >= MAX_HUE_VALUE)
        hsl.hue -= MAX_HUE_VALUE;

    // saturation
    if (s > 0)
        hsl.saturation = (hsl.saturation / (1-dS));
    else
        hsl.saturation = (hsl.saturation * (1+dS));

    if(hsl.saturation <= MIN_SATURATION_VALUE)
        hsl.saturation = 0, hsl.hue = 0;  // 灰色系了

    if (hsl.saturation > MAX_SATURATION_VALUE)
        hsl.saturation = MAX_SATURATION_VALUE;


    c.SetHSL(hsl);
    R = c.r;
    G = c.g;
    B = c.b;

    return true;
}
bool ChangeColorSaturation(byte& R, byte& G, byte& B, short s, float dS)
{
    if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
        return false;

    Color c(R,G,B,255);

    HSL hsl;
    c.GetHSL(hsl);

    // saturation
    if (s > 0)
        hsl.saturation = (hsl.saturation / (1-dS));
    else
        hsl.saturation = (hsl.saturation * (1+dS));

    if(hsl.saturation <= MIN_SATURATION_VALUE)
        hsl.saturation = 0, hsl.hue = 0;  // 灰色系了

    if (hsl.saturation > MAX_SATURATION_VALUE)
        hsl.saturation = MAX_SATURATION_VALUE;

    c.SetHSL(hsl);
    R = c.r;
    G = c.g;
    B = c.b;

    return true;
}

bool ChangeColorHLS(byte& R, byte& G, byte& B, short h, short l , short s, int nFlag )
{
    bool bChangeH = nFlag & CHANGE_SKIN_HLS_FLAG_H ? true:false;
    bool bChangeL = nFlag & CHANGE_SKIN_HLS_FLAG_L ? true:false;
    bool bChangeS = nFlag & CHANGE_SKIN_HLS_FLAG_S ? true:false;
    bool bSetHueMode = nFlag & CHANGE_SKIN_HLS_FALG_REPLACE_MODE ? false:true;
    if (l == 0)
        bChangeL = false;
    if (s == 0)
        bChangeS = false;

    if(false == bChangeH && false == bChangeL && false == bChangeS)
        return false;

    float dL = 0, ds = 0;
    if (bChangeL)
        dL = (float)(l/100.0); 
    if (bChangeS)
        ds = (float)(s/100.0);

    if (bChangeL)
        ChangeColorLuminance(R,G,B,l,dL);

    if (bChangeH && bChangeS)
    {
        ChangeColorHueAndSaturation(R,G,B,h,bSetHueMode,s,ds);
    }
    else
    {
        if (bChangeH)
            ChangeColorHue(R,G,B,h,bSetHueMode);
        if (bChangeS)
            ChangeColorSaturation(R,G,B,s,ds);
    }
    return true;
}

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
const wchar_t* ColorResItem::GetColorString()
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

void  ColorResItem::SetId(const wchar_t* szId)
{
	if (szId)
		m_strId = szId;
	else
		m_strId.clear();
}
const wchar_t*  ColorResItem::GetId()
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
        m_pColor = Color::CreateInstance(util::TranslateRGB(m_strColor.c_str(), XML_SEPARATOR));
        
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
		m_pColor->ReplaceRGB(util::TranslateRGB(m_strColor.c_str(), XML_SEPARATOR));
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

		unsigned char R = GetRValue(*m_pOriginColorValue);
		unsigned char G = GetGValue(*m_pOriginColorValue);
		unsigned char B = GetBValue(*m_pOriginColorValue);

		if (ChangeColorHLS(R,G,B,h,l,s,nFlag))
			m_pColor->ReplaceRGB(RGB(R,G,B));
	}
	return true;
}

ColorRes::ColorRes(Resource* p)
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
	std::vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	std::vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

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

void ColorRes::GetColor(const wchar_t* szColorId, Color** pp)
{
	if (!szColorId || !szColorId[0] || !pp)
		return;

     // 直接翻译，不根据ID去映射
    if (szColorId[0] == _T('#'))
    {
        COLORREF color = util::TranslateHexColor(szColorId+1); 
        *pp = Color::CreateInstance(color);
        return;
    }
    else if (szColorId[0] == _T('0') && szColorId[1] == _T('x'))
    {
        COLORREF color = util::TranslateHexColor(szColorId+2);
        *pp = Color::CreateInstance(color);
        return;
    }
    else if (wcsstr(szColorId, TEXT(",")))
    {
        COLORREF color = util::TranslateColor(szColorId);
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
bool ColorRes::LoadItem(IMapAttribute* pMapAttrib, const wchar_t* szValue)
{
	String strID;

    const wchar_t* szText = pMapAttrib->GetAttr(XML_ID, true);
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
bool ColorRes::InsertColor(const String& strID, const wchar_t* szColor, ColorResItem** pItem )
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
	std::vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	std::vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

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
	std::vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	std::vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

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
	std::vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	std::vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		ColorResItem* p = *iter;
		p->ModifyHLS(h,l,s,nFlag);
	}

	return true;
}

const wchar_t*  ColorRes::GetColorId(Color* p)
{
	std::vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	for( ; iter != m_vColors.end(); ++iter)
	{
		if ((*iter)->IsMyColor(p))
			return (*iter)->GetId();
	}

	return nullptr;
}

}