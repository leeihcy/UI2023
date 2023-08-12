#include "include/inc.h"
#include "textrender.h"

#include "src/render/renderbase.h"

#include "src/object/object.h"
#include "include/interface/imapattr.h"
// #include "src/UIObject\Window\windowbase.h"
#include "include/interface/iuires.h"
// #include "src/Atl\image.h"
#include "src/attribute/attribute.h"
#include "src/attribute/flags_attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/string_attribute.h"
#include "src/resource/res_bundle.h"
// #include "src/Layer\windowrender.h"
#include "src/application/uiapplication.h"
#include "src/resource/colorres.h"

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                                  TextRender                                          //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

namespace ui {
#define DEFAULT_DRAWTEXT_FLAG (DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_NOPREFIX)
TextRenderBase::TextRenderBase(ITextRenderBase* p) : Message(p)
{
    m_pITextRenderBase = p;
	m_pObject = nullptr;
	m_nTextRenderType = TEXTRENDER_TYPE_NULL;
	m_nDrawTextFlag = DEFAULT_DRAWTEXT_FLAG;
    m_eDrawTextEffect = TEXT_EFFECT_NONE;
    m_pColorTextBkgnd = nullptr;
    m_wparamDrawText = m_lparamDrawText = 0;
	m_lRef = 0;
}
TextRenderBase::~TextRenderBase()
{
    SAFE_RELEASE(m_pColorTextBkgnd);
}

long  TextRenderBase::AddRef()
{
	m_lRef++;
	return m_lRef;
}
long  TextRenderBase::Release()
{
	--m_lRef;
	if (0 >= m_lRef)
	{
		m_pITextRenderBase->IMessage::Release();
		return 0;
	}

	return m_lRef;
}

void TextRenderBase::SetTextAlignment(int nDrawFlag)
{
	m_nDrawTextFlag = nDrawFlag;
}



void TextRenderBase::Serialize(AttributeSerializer* ps)
{
	ps->AddFlags(XML_TEXTRENDER_ALIGN, m_nDrawTextFlag)
// 		->AddFlag(DT_TOP, XML_TEXTRENDER_ALIGN_TOP)   // 0，不放属性里了
// 		->AddFlag(DT_LEFT, XML_TEXTRENDER_ALIGN_LEFT)
		->AddFlag(DT_RIGHT, XML_TEXTRENDER_ALIGN_RIGHT)
		->AddFlag(DT_CENTER, XML_TEXTRENDER_ALIGN_CENTER)
		->AddFlag(DT_BOTTOM, XML_TEXTRENDER_ALIGN_BOTTOM)
		->AddFlag(DT_VCENTER, XML_TEXTRENDER_ALIGN_VCENTER)
		->AddFlag(DT_SINGLELINE, XML_TEXTRENDER_ALIGN_SINGLELINE)
		->AddFlag(DT_WORDBREAK|DT_EDITCONTROL, XML_TEXTRENDER_ALIGN_MULTILINE)
		->AddFlag(DT_END_ELLIPSIS, XML_TEXTRENDER_ALIGN_END_ELLIPSIS)
		->AddFlag(DT_NOPREFIX, XML_TEXTRENDER_ALIGN_NO_PREFIX)
        ->SetDefault(DEFAULT_DRAWTEXT_FLAG);

	ps->AddEnum(XML_TEXTRENDER_EFFECT, *(long*)&m_eDrawTextEffect)
		->AddOption(TEXT_EFFECT_NONE, XML_TEXTRENDER_EFFECT_NONE)
		->AddOption(TEXT_EFFECT_HALO, XML_TEXTRENDER_EFFECT_HALO)
        ->AddOption(TEXT_EFFECT_ENDALPHAMASK, XML_TEXTRENDER_EFFECT_ENDALPHAMASK);

	ps->AddString(XML_TEXTRENDER_EFFECT_HALO_SHADOW_COLOR,
		Slot(&TextRenderBase::LoadHaloColor, this),
		Slot(&TextRenderBase::GetHaloColorId, this));
}

void  TextRenderBase::LoadHaloColor(const wchar_t* szColorId)
{
    _LoadColor(szColorId, m_pColorTextBkgnd); 
    m_wparamDrawText = 3; // 默认模糊半径
}
const wchar_t*  TextRenderBase::GetHaloColorId()
{
    return _SaveColor(m_pColorTextBkgnd);
}

void  TextRenderBase::_LoadFont(const wchar_t* szFontId, IRenderFont*& pRenderFont)
{
#if 0
    SAFE_RELEASE(pRenderFont);
    IFontRes*  pFontRes = GetSkinFontRes();
    if (!szFontId || !pFontRes)
        return;

	GRAPHICS_RENDER_LIBRARY_TYPE eType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
	WindowBase* pWnd = m_pObject->GetWindowObject();
	if (pWnd)
	{
		WindowRender* pRender = pWnd->GetWindowRender();
		if (pRender)
			eType = pRender->GetGraphicsRenderType();
	}

	pFontRes->GetFont(szFontId, eType, &pRenderFont);
#else
    UIASSERT(false);
#endif
}

const wchar_t*  TextRenderBase::_SaveFont(IRenderFont*& pRenderFont)
{
    if (!pRenderFont)
        return nullptr;

    IFontRes* pFontRes = GetSkinFontRes();
    if (pFontRes)
    {
        const wchar_t* szId = pFontRes->GetRenderFontId(pRenderFont);
        if (szId)
            return szId;
    }
    return nullptr;
}

void  TextRenderBase::_LoadDefalutFont(IRenderFont** ppRenderFont)
{
#if 0
	WindowBase* pWnd = m_pObject->GetWindowObject();
	if (!pWnd)
		return;

	IRenderFont* pFont = pWnd->GetWindowDefaultRenderFont();
    if (pFont)
    {
		if (ppRenderFont)
		{
			*ppRenderFont = pFont;
			(*ppRenderFont)->AddRef();
		}
    }
    else
    {
        IFontRes* pFontRes = GetSkinFontRes();

        // 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
		if (pFontRes && m_pObject)
		{
			GRAPHICS_RENDER_LIBRARY_TYPE eType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
			WindowBase* pWnd = m_pObject->GetWindowObject();
			if (pWnd)
			{
				WindowRender* pRender = pWnd->GetWindowRender();
				if (pRender)
					eType = pRender->GetGraphicsRenderType();
			}

			pFontRes->GetDefaultFont(eType, ppRenderFont);
		}
    }
#else
    UIASSERT(0);
#endif
}

void  TextRenderBase::_LoadColor(const wchar_t* szColorId, Color*& pColor)
{
    SAFE_RELEASE(pColor);
    if (!szColorId)
        return;

    IColorRes* pColorRes = GetSkinColorRes();
    if (!pColorRes)
        return;

    pColorRes->GetColor(szColorId, &pColor);
}
const wchar_t*  TextRenderBase::_SaveColor(Color*& pColor)
{
    if (!pColor)
        return nullptr;

    IColorRes* pColorRes = GetSkinColorRes();
    if (pColorRes)
    {
        const wchar_t* szId = pColorRes->GetColorId(pColor);
        if (szId)
            return szId;
    }

    wchar_t* szBuffer = GetTempBuffer();
    pColor->ToWebString(szBuffer);
    return szBuffer;
}

void  TextRenderBase::OnGetDesiredSize(Size* pSize, GETDESIREDSIZEINFO* pInfo)
{
    *pSize = GetDesiredSize(pInfo->szText, pInfo->nLimitWidth);
}

Size TextRenderBase::GetDesiredSize(const wchar_t* szText, int nLimitWidth)
{
    Size s = {0,0};
    IRenderFont* pFont = m_pITextRenderBase->GetRenderFont();
    if (pFont)
        s = pFont->MeasureString(szText, nLimitWidth);

    return s;
}

IColorRes*  TextRenderBase::GetSkinColorRes()
{
    ResBundle* pSkinRes = GetSkinRes();
    if (!pSkinRes)
        return nullptr;

    return &pSkinRes->GetColorRes().GetIColorRes();
}

ResBundle*  TextRenderBase::GetSkinRes()
{
    if (m_pObject)
    {
        return m_pObject->GetSkinRes();
    }

    return nullptr;
}

IFontRes*  TextRenderBase::GetSkinFontRes()
{
    ResBundle* pSkinRes = GetSkinRes();
    if (!pSkinRes)
        return nullptr;

    return &pSkinRes->GetFontRes().GetIFontRes();
}

bool  TextRenderBase::IsThemeRender() 
{ 
    return  m_nTextRenderType > TEXTRENDER_TYPE_THEME_FIRST && 
            m_nTextRenderType < TEXTRENDER_TYPE_THEME_LAST; 
}

void  TextRenderBase::CheckSkinTextureChanged()
{
    if (m_nTextRenderType > TEXTRENDER_TYPE_CONTRAST_FIRST && m_nTextRenderType < TEXTRENDER_TYPE_CONTRAST_LAST)
    {
        static_cast<IMessage*>(m_pITextRenderBase)->SendMessage(UI_MSG_SKINTEXTURECHANGED);
    }
}


SimpleTextRender::SimpleTextRender(ISimpleTextRender* p):TextRenderBase(p)
{
    m_pISimpleTextRender = p;

	m_pColorText = nullptr;
	m_pRenderFont = nullptr;
}
SimpleTextRender::~SimpleTextRender()
{
	SAFE_RELEASE(m_pColorText);
	SAFE_RELEASE(m_pRenderFont);
}

// 如果字体指针为空，则取object对象的配置字体。
void SimpleTextRender::OnSerialize(SERIALIZEDATA* pData)
{
	{
		AttributeSerializer s(pData, TEXT("SimpleTextRender"));
        TextRenderBase::Serialize(&s);

		s.AddString(XML_TEXTRENDER_FONT,
			Slot(&SimpleTextRender::LoadFont, this),
			Slot(&SimpleTextRender::GetFontId, this));
        s.AddColor(XML_TEXTRENDER_COLOR, m_pColorText);
	}

	if (!m_pRenderFont && pData->IsLoad())
	{
		_LoadDefalutFont(&m_pRenderFont);
	}
}

void  SimpleTextRender::LoadFont(const wchar_t* szFontId)
{
#ifdef EDITOR_MODE
	if (m_pObject && m_pObject->GetUIApplication()->IsEditorMode())
	{
		if (szFontId)
			m_strFontId = szFontId;
		else
			m_strFontId.clear();
	}
#endif

    ResBundle* pSkinRes = GetSkinRes();
    if (!pSkinRes)
        return;

    szFontId = pSkinRes->GetI18nRes().MapConfigValue(szFontId);
    _LoadFont(szFontId, m_pRenderFont);
}
const wchar_t*  SimpleTextRender::GetFontId()
{
#ifdef EDITOR_MODE
    return m_strFontId.c_str();
#endif
    return nullptr;
}


void SimpleTextRender::SetRenderFont(IRenderFont* pFont)
{
	SAFE_RELEASE(m_pRenderFont);

	m_pRenderFont = pFont;

	if (pFont)
		pFont->AddRef();
}

void SimpleTextRender::SetColor(Color*  pColText)
{
	SAFE_RELEASE(m_pColorText);
    if (pColText)
        m_pColorText = Color::CreateInstance(pColText->m_col);
}

bool  SimpleTextRender::GetColor(COLORREF& color)
{
	if (!m_pColorText)
		return false;

	color = m_pColorText->GetGDIValue();
	return true;
}


void SimpleTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->ds_renderbase.pRenderTarget;
	if (nullptr == pRenderTarget || nullptr == pDrawStruct->szText)
		return ;

	if (m_pRenderFont && wcslen(pDrawStruct->szText) > 0)
	{
        DRAWTEXTPARAM  param;

        if (m_pColorText)
			param.color = m_pColorText->m_col;
        param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
        param.prc = &pDrawStruct->ds_renderbase.rc;
        param.szText = pDrawStruct->szText;

        param.nEffectFlag = m_eDrawTextEffect;
        if (m_pColorTextBkgnd)
            param.bkcolor = *m_pColorTextBkgnd;
        param.wParam = m_wparamDrawText;
        param.lParam = m_lparamDrawText;
		pRenderTarget->DrawString(m_pRenderFont, &param);
	}
}


//////////////////////////////////////////////////////////////////////////


ContrastColorTextRender::ContrastColorTextRender(IContrastColorTextRender* p):TextRenderBase(p)
{
    m_pIContrastColorTextRender = p;

    m_pColorText = nullptr;
    m_pRenderFont = nullptr;
    m_bNeedCalcColor = false;
}
ContrastColorTextRender::~ContrastColorTextRender()
{
    SAFE_RELEASE(m_pRenderFont);
    SAFE_RELEASE(m_pColorText);
}

// 如果字体指针为空，则取object对象的配置字体。
void ContrastColorTextRender::OnSerialize(SERIALIZEDATA* pData)
{
    {
        AttributeSerializer s(pData, TEXT("ContrastColorTextRender"));
        TextRenderBase::Serialize(&s);

        s.AddString(XML_TEXTRENDER_FONT,
            Slot(&ContrastColorTextRender::LoadFont, this),
            Slot(&ContrastColorTextRender::GetFontId, this));
        s.AddString(XML_TEXTRENDER_COLOR,
            Slot(&ContrastColorTextRender::LoadColor, this),
            Slot(&ContrastColorTextRender::GetColorId, this));
    }

    if (pData->IsLoad())
    {
        if (!m_pRenderFont)
        {
            _LoadDefalutFont(&m_pRenderFont);
        }

        // 如果没有配置背景色，则默认设置一个
        if (m_eDrawTextEffect == TEXT_EFFECT_HALO)
        {
            if (m_pColorText && !m_pColorTextBkgnd)
            {
                SAFE_RELEASE(m_pColorTextBkgnd);
                if (m_pColorText->m_col == 0xFFFFFFFF)
                {
                    m_pColorTextBkgnd = Color::CreateInstance(0xFF000000);
                }
                else
                {
                    m_pColorTextBkgnd = Color::CreateInstance(0xFFFFFFFF);
                }
            }
            m_wparamDrawText = 3; // 默认模糊半径
        }
    }
}

void ContrastColorTextRender::SetRenderFont(IRenderFont* pFont)
{
    SAFE_RELEASE(m_pRenderFont);

    m_pRenderFont = pFont;

    if (pFont)
        pFont->AddRef();
}

void ContrastColorTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
	UIASSERT(0);
#if 0
    if (nullptr == m_pObject)
        return;

    if (nullptr == m_pColorText)
    {
        unsigned int dwColor = m_pObject->CalcContrastTextColor();
        m_pColorText = Color::CreateInstance(dwColor);

        if (m_eDrawTextEffect == TEXT_EFFECT_HALO)
        {
            SAFE_RELEASE(m_pColorTextBkgnd);
            if (dwColor == 0xFFFFFFFF)
            {
                m_pColorTextBkgnd = Color::CreateInstance(0xFF000000);
            }
            else
            {
                m_pColorTextBkgnd = Color::CreateInstance(0xFFFFFFFF);
            }
        }
    }

    IRenderTarget* pRenderTarget = pDrawStruct->ds_renderbase.pRenderTarget;
    if (nullptr == pRenderTarget || nullptr == pDrawStruct->szText)
        return ;

    if (m_pRenderFont && _tcslen(pDrawStruct->szText) > 0)
    {
        DRAWTEXTPARAM  param;
        param.szText = pDrawStruct->szText;
        param.prc = &pDrawStruct->ds_renderbase.rc;
        param.color = m_pColorText->m_col;
        param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;

        param.nEffectFlag = m_eDrawTextEffect;
        if (m_pColorTextBkgnd)
            param.bkcolor = *m_pColorTextBkgnd;
        param.wParam = m_wparamDrawText;
        param.lParam = m_lparamDrawText;
        pRenderTarget->DrawString(m_pRenderFont, &param);
    }
#endif
}

long  ContrastColorTextRender::OnSkinTextureChanged(unsigned int, long, long)
{
    SAFE_RELEASE(m_pColorText);
    return 0;
}

//////////////////////////////////////////////////////////////////////////

ContrastColorListTextRender::ContrastColorListTextRender(
		IContrastColorListTextRender* p):TextRenderBase(p)
{
    m_nCount = 0;
    m_pRenderFont = nullptr;
    m_pIContrastColorListTextRender = p;
}
ContrastColorListTextRender::~ContrastColorListTextRender()
{
    this->Clear();
    SAFE_RELEASE(m_pRenderFont);
}
void  ContrastColorListTextRender::Clear()
{
    for (int i = 0; i < m_nCount; i++)
    {
        SAFE_RELEASE(m_vTextColor[i]);
    }

    m_vTextColor.clear();
    m_nCount = 0;
}


long  ContrastColorListTextRender::OnSkinTextureChanged(unsigned int, long, long)
{
    for (int i = 0; i < m_nCount; i++)
    {
        SAFE_RELEASE(m_vTextColor[i]);
        m_vTextColor[i] = nullptr;
    }

    return 0;
}
void ContrastColorListTextRender::OnSerialize(SERIALIZEDATA* pData)
{
    {
        AttributeSerializer s(pData, TEXT("ContrastColorListTextRender"));
        TextRenderBase::Serialize(&s);

        s.AddLong(XML_TEXTRENDER_COLORLIST_COUNT,
            Slot(&ContrastColorListTextRender::SetCount, this),
            Slot(&ContrastColorListTextRender::GetCount, this));

        s.AddString(XML_TEXTRENDER_FONT,
            Slot(&ContrastColorListTextRender::LoadFont, this),
            Slot(&ContrastColorListTextRender::GetFontId, this));
    }

    if (pData->IsLoad() && !m_pRenderFont)
    {
        _LoadDefalutFont(&m_pRenderFont);
    }
}

void ContrastColorListTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
	UIASSERT(0);
#if 0
    if (0 == m_nCount || nullptr == m_pObject)
        return;

    int nRealState = LOWORD(pDrawStruct->ds_renderbase.nState);
    if (nRealState >= m_nCount)
        nRealState = 0;


    if (nullptr  == m_vTextColor[nRealState])
        m_vTextColor[nRealState] = Color::CreateInstance(m_pObject->CalcContrastTextColor());

    DRAWTEXTPARAM  param;

    param.prc = &pDrawStruct->ds_renderbase.rc;
    param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
    if (m_vTextColor[nRealState])
        param.color = m_vTextColor[nRealState]->m_col;
    param.szText = pDrawStruct->szText;
    pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_pRenderFont, &param);
#endif
}

IRenderFont* ContrastColorListTextRender::GetRenderFont()
{
    return m_pRenderFont;
}

void ContrastColorListTextRender::SetRenderFont(IRenderFont* pFont)
{
    SAFE_RELEASE(m_pRenderFont);
    m_pRenderFont = pFont;

    if (pFont)
        pFont->AddRef();
}

void ContrastColorListTextRender::SetCount(long nCount)
{
    this->Clear();
    m_nCount = nCount;

    for (int i = 0; i < m_nCount; i++)
    {
        m_vTextColor.push_back(nullptr);
    }
}
long  ContrastColorListTextRender::GetCount()
{
    return (long)m_vTextColor.size();
}

//////////////////////////////////////////////////////////////////////////

ColorListTextRender::ColorListTextRender(IColorListTextRender* p):TextRenderBase(p)
{
	m_nCount = 0;
	m_pRenderFont = nullptr;
    m_pIColorListTextRender = p;
}
ColorListTextRender::~ColorListTextRender()
{
	this->Clear();
	SAFE_RELEASE(m_pRenderFont);
}
void ColorListTextRender::Clear()
{
	for (int i = 0; i < m_nCount; i++)
	{
		SAFE_RELEASE(m_vTextColor[i]);
	}
	m_vTextColor.clear();
	m_nCount = 0;
}


void ColorListTextRender::OnSerialize(SERIALIZEDATA* pData)
{
    {
        AttributeSerializer s(pData, TEXT("ColorListTextRender"));
        TextRenderBase::Serialize(&s);

        s.AddLong(XML_TEXTRENDER_COLORLIST_COUNT,
            Slot(&ColorListTextRender::SetCount, this),
            Slot(&ColorListTextRender::GetCount, this));

        s.AddString(XML_TEXTRENDER_COLOR,
            Slot(&ColorListTextRender::LoadColor, this),
            Slot(&ColorListTextRender::GetColor, this));

        s.AddString(XML_TEXTRENDER_FONT,
            Slot(&ColorListTextRender::LoadFont, this),
            Slot(&ColorListTextRender::GetFontId, this));
    }

	if (pData->IsLoad() && !m_pRenderFont)
	{
		_LoadDefalutFont(&m_pRenderFont);
	}
}

void  ColorListTextRender::LoadFont(const wchar_t* szFontId)
{
#ifdef EDITOR_MODE
    if (szFontId)
        m_strFontId = szFontId;
    else
        m_strFontId.clear();
#endif

    ResBundle* pSkinRes = GetSkinRes();
    if (!pSkinRes)
        return;

    szFontId = pSkinRes->GetI18nRes().MapConfigValue(szFontId);
    _LoadFont(szFontId, m_pRenderFont);
}
const wchar_t*  ColorListTextRender::GetFontId()
{
    //return _SaveFont(m_pRenderFont);
#ifdef EDITOR_MODE
    return m_strFontId.c_str();
#endif
    return nullptr;
}

void  ColorListTextRender::LoadColor(const wchar_t* szText)
{
    if (!szText)
        return ;
    
    IColorRes* pColorRes = GetSkinColorRes();

    std::vector<String> vColors;
    UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
    int nCount = (int)vColors.size();

    if (0 == m_nCount)
    {
        this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小
    }

    for(int i = 0; i < m_nCount && i < nCount; i++ )
    {
        if (!vColors[i].empty())
        {
            SAFE_RELEASE(m_vTextColor[i]);
            pColorRes->GetColor(vColors[i].c_str(), &m_vTextColor[i]);
        }
    }
}
const wchar_t*  ColorListTextRender::GetColor()
{
    String&  strBuffer = GetTempBufferString();
    for (int i = 0; i < m_nCount; i++)
    {
        if (i > 0)
            strBuffer.push_back(XML_MULTI_SEPARATOR);

        const wchar_t* szTemp = _SaveColor(m_vTextColor[i]);
        if (szTemp)
            strBuffer.append(szTemp);
    }

    return strBuffer.c_str();
}

void ColorListTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (0 == m_nCount)
        return;

	int nRealState = (pDrawStruct->ds_renderbase.nState) & 0xFFFF;
	if (nRealState >= m_nCount)
		nRealState = 0;

    DRAWTEXTPARAM  param;
	if (m_vTextColor[nRealState])
		param.color = m_vTextColor[nRealState]->m_col;
	param.prc = &pDrawStruct->ds_renderbase.rc;
	param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
    param.szText = pDrawStruct->szText;
	pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_pRenderFont, &param);
}
IRenderFont* ColorListTextRender::GetRenderFont()
{
	return m_pRenderFont;
}

void ColorListTextRender::SetRenderFont(IRenderFont* pFont)
{
	SAFE_RELEASE(m_pRenderFont);
	m_pRenderFont = pFont;

	if (pFont)
		pFont->AddRef();
}

void ColorListTextRender::SetCount(long nCount)
{
	this->Clear();
	m_nCount = nCount;

	for (int i = 0; i < m_nCount; i++)
	{
		m_vTextColor.push_back(nullptr);
	}
}
long  ColorListTextRender::GetCount()
{
    return (long)m_vTextColor.size();
}

void ColorListTextRender::SetColor(int nIndex, COLORREF col)
{
	nIndex = nIndex & 0xFFFF;
	if (nIndex >= m_nCount)
		return;

	if (m_vTextColor[nIndex])
	{
		SAFE_RELEASE(m_vTextColor[nIndex]);
	}
    m_vTextColor[nIndex] = Color::CreateInstance(col);
}

//////////////////////////////////////////////////////////////////////////
FontColorListTextRender::FontColorListTextRender(
		IFontColorListTextRender* p):TextRenderBase(p)
{
	m_nCount = 0;
    m_pIFontColorListTextRender = p;
}
FontColorListTextRender::~FontColorListTextRender()
{
	this->Clear();
}
void FontColorListTextRender::Clear()
{
	for (int i = 0; i < m_nCount; i++)
	{
		SAFE_RELEASE(m_vTextColor[i]);
		SAFE_RELEASE(m_vTextFont[i]);
	}
	m_vTextFont.clear();
	m_vTextColor.clear();
	m_nCount = 0;
}

void FontColorListTextRender::OnSerialize(SERIALIZEDATA* pData)
{
    {
        AttributeSerializer s(pData, TEXT("FontColorListTextRender"));
        TextRenderBase::Serialize(&s);

        s.AddLong(XML_TEXTRENDER_FONTCOLORLIST_COUNT,
            Slot(&FontColorListTextRender::SetCount, this),
            Slot(&FontColorListTextRender::GetCount, this));

        s.AddString(XML_TEXTRENDER_COLOR,
            Slot(&FontColorListTextRender::LoadColor, this),
            Slot(&FontColorListTextRender::GetColor, this));

        s.AddString(XML_TEXTRENDER_FONT,
            Slot(&FontColorListTextRender::LoadFont, this),
            Slot(&FontColorListTextRender::GetFont, this));
    }
}

void  FontColorListTextRender::LoadColor(const wchar_t* szText)
{
    if (!szText)
        return;

    std::vector<String> vColors;
    UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
    int nCount = (int)vColors.size();

    if (0 == m_nCount)
        this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

    IColorRes* pColorRes = GetSkinColorRes();
    if (!pColorRes)
        return;

    for(int i = 0; i < m_nCount && i < nCount; i++ )
    {
        if(! vColors[i].empty() )
        {
            pColorRes->GetColor(vColors[i].c_str(), &m_vTextColor[i]);
        }
    }
}
const wchar_t*  FontColorListTextRender::GetColor()
{
    String&  strBuffer = GetTempBufferString();
    for (int i = 0; i < m_nCount; i++)
    {
        if (i > 0)
            strBuffer.push_back(XML_MULTI_SEPARATOR);

        const wchar_t* szTemp = _SaveColor(m_vTextColor[i]);
        if (szTemp)
            strBuffer.append(szTemp);
    }

    return strBuffer.c_str();
}

void  FontColorListTextRender::LoadFont(const wchar_t* szText)
{
    IFontRes* pFontRes = GetSkinFontRes();

    if (szText && m_pObject)
    {
        std::vector<String> vFont;
        UI_Split(szText, XML_MULTI_SEPARATOR, vFont);
        int nCount = (int)vFont.size();

        if (0 == m_nCount)
            this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

#if 0
		GRAPHICS_RENDER_LIBRARY_TYPE eType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		WindowBase* pWnd = m_pObject->GetWindowObject();
		if (pWnd)
		{
			WindowRender* pRender = pWnd->GetWindowRender();
			if (pRender)
				eType = pRender->GetGraphicsRenderType();
		}

        for(int i = 0; i < m_nCount && i < nCount; i++ )
        {
            if(! vFont[i].empty() )
            {
				pFontRes->GetFont(vFont[i].c_str(), eType, &m_vTextFont[i]);
            }
        }
#endif
    }

#if 0
	WindowBase* pWnd = m_pObject->GetWindowObject();

    for(int i = 0; i < m_nCount; i++ )
    {
        if(nullptr == m_vTextFont[i])
        {
			if (pWnd)
				m_vTextFont[i] = pWnd->GetWindowDefaultRenderFont();
            if (m_vTextFont[i])
                m_vTextFont[i]->AddRef();
        }
    }
#else
    UIASSERT(false);
#endif
}

const wchar_t*  FontColorListTextRender::GetFont()
{
    String&  strBuffer = GetTempBufferString();
    for (int i = 0; i < m_nCount; i++)
    {
        if (i > 0)
            strBuffer.push_back(XML_MULTI_SEPARATOR);

        const wchar_t* szTemp = _SaveFont(m_vTextFont[i]);
        if (szTemp)
            strBuffer.append(szTemp);
    }

    return strBuffer.c_str();
}

void FontColorListTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (0 == m_nCount)
        return;

	int nRealState = (pDrawStruct->ds_renderbase.nState) & 0xFFFF;
	if (nRealState >= m_nCount)
		nRealState = 0;

    DRAWTEXTPARAM  param;
	if (m_vTextColor[nRealState])
		param.color = *m_vTextColor[nRealState];

	param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
    param.szText = pDrawStruct->szText;
    param.prc = &(pDrawStruct->ds_renderbase.rc);
	pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_vTextFont[nRealState], &param);
}
IRenderFont* FontColorListTextRender::GetRenderFont()
{
	if (m_vTextFont.size() > 0)
		return m_vTextFont[0];
	
	return nullptr;
}

void FontColorListTextRender::SetRenderFont(IRenderFont* pRenderFont)
{
	if (m_vTextFont.size() > 0)
	{
		SAFE_RELEASE(m_vTextFont[0]);
		m_vTextFont[0] = pRenderFont;

		if (pRenderFont)
			pRenderFont->AddRef();
	}
}

void FontColorListTextRender::SetCount(long nCount)
{
	this->Clear();
	m_nCount = nCount;

	for (int i = 0; i < m_nCount; i++)
	{
		m_vTextColor.push_back(nullptr);
		m_vTextFont.push_back(nullptr);
	}
}

long  FontColorListTextRender::GetCount()
{
    return m_nCount;
}

void  FontColorListTextRender::SetColor(int nIndex, unsigned int col)
{
	nIndex = nIndex & 0xFFFF;
	if (nIndex >= m_nCount)
		return;

	if (m_vTextColor[nIndex])
	{
		SAFE_RELEASE(m_vTextColor[nIndex]);
	}
    m_vTextColor[nIndex] = Color::CreateInstance(col);
}
void FontColorListTextRender::SetFont(int nIndex, IRenderFont* pFont)
{
	if (nIndex >= m_nCount)
		return;

	SAFE_RELEASE(m_vTextFont[nIndex]);

	m_vTextFont[nIndex] = pFont;

	if (pFont)
		pFont->AddRef();
}
//////////////////////////////////////////////////////////////////////////
#if 0
ThemeTextRenderBase::ThemeTextRenderBase(IThemeTextRenderBase* p):TextRenderBase(p)
{
    m_pIThemeTextRenderBase = p;
	m_hTheme = nullptr;
    m_bNoTheme = false;
}
ThemeTextRenderBase::~ThemeTextRenderBase()
{
	if (m_hTheme)
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = nullptr;
	}
}
void ThemeTextRenderBase::OnInit()
{
	this->CreateTheme();
}
void ThemeTextRenderBase::OnThemeChanged()
{
	this->CreateTheme();
}
void ThemeTextRenderBase::CreateTheme()
{
    if (m_hTheme)
    {
        ::CloseThemeData(m_hTheme);
        m_hTheme = nullptr;
    }
    if (!m_bNoTheme)
    {
        const wchar_t* pThemeName = m_pIThemeTextRenderBase->GetThemeName();
        if (pThemeName)
        {
            m_hTheme = ::OpenThemeData(m_pObject->GetHWND(), pThemeName);
        }
    }
}

void  ThemeTextRenderBase::Serialize(AttributeSerializerWrap* ps)
{
	AttributeSerializer* psImpl = ps->GetImpl();
	TextRenderBase::Serialize(psImpl);
	
	psImpl->AddBool(XML_RENDER_THEME_DISABLE, this,
		memfun_cast<pfnBoolSetter>(&ThemeTextRenderBase::SetNoTheme),
		memfun_cast<pfnBoolGetter>(&ThemeTextRenderBase::GetNoTheme));
}

#endif

}