#include "skinres.h"
#include "src/resource/skinmanager.h"
#include "src/skin_parse/skinparseengine.h"
#include "src/skin_parse/xml/xmlwrap.h"
#include "src/skin_parse/datasource/skindatasource.h"
// #include "src/util/Stopwatch/stopwatch.h"
#include "include/interface/iuires.h"
#include "uiresource.h"

namespace ui 
{
    
SkinRes::SkinRes(SkinManager& o):
    m_mgrSkinRef(o),
    m_mgrLayout(this),
    m_mgrStyle(this),
    m_mgrFont(this),
    m_mgrColor(this),
    m_mgrImage(this),
	m_mgrI18n(this)
    
{	
    m_pISkinRes = nullptr;
    m_eLoadState = SKIN_RES_LOAD_STATE_NONE;
    m_pDataSource = nullptr;
    m_pHLSInfo = nullptr;
	m_pParentSkinRes = nullptr;
}
SkinRes::~SkinRes()
{
    Unload();
    SAFE_DELETE(m_pISkinRes);
    SAFE_DELETE(m_pHLSInfo);
    SAFE_RELEASE(m_pDataSource);
}

ISkinRes*  SkinRes::GetISkinRes()
{
    if (nullptr == m_pISkinRes)
    {
        m_pISkinRes = new ISkinRes(this);
    }
    return m_pISkinRes; 
}

void  SkinRes::SetName(const wchar_t* szName)
{
	if (szName)
		m_strSkinResName = szName;
	else
		m_strSkinResName.clear();
}

void  SkinRes::SetPath(const wchar_t* szPath)
{
	UIASSERT(m_pDataSource);
	if (!m_pDataSource)
		return;
	m_pDataSource->SetPath(szPath);
}

SkinDataSource*  SkinRes::CreateDataSource(SKIN_PACKET_TYPE eType)
{
	SAFE_DELETE(m_pDataSource);
	CreateDataSourceInstance(eType, &m_pDataSource);

	return m_pDataSource;
}

const wchar_t*  SkinRes::GetName()
{ 
    return m_strSkinResName.c_str();
}

const wchar_t*  SkinRes::GetPath() 
{ 
    if (nullptr == m_pDataSource)
        return nullptr;

    return m_pDataSource->GetPath(); 
}

bool  SkinRes::Load()
{
    if (m_eLoadState == SKIN_RES_LOAD_STATE_LOADED)
        return true;

    // 加载速度测试
#if 0
	StopWatch watch;
	watch.Start();
#endif
	SkinParseEngine parse(this);
    if (parse.Parse(m_pDataSource, XML_SKIN_XML))
    {
		m_eLoadState = SKIN_RES_LOAD_STATE_LOADED;
		
#if 0        
		__int64 lCost = watch.Now();
		wchar szText[128] = {0};
		wprintf(szText, _T("Load Skin File Cost: %d ms\r\n"), (int)lCost);
		::OutputDebugString(szText);

		UI_LOG_DEBUG(_T("Load Skin File Cost: %d ms"), lCost);
#endif
		return true;
    }   
	else
	{
		Unload();
		return false;
	}
}

bool  SkinRes::Unload()
{
    _DocList::iterator iter = m_listDoc.begin();
    for (; iter != m_listDoc.end(); iter++)
    {
        UIDocument* po = *iter;
        SAFE_RELEASE(po);
    }
    m_listDoc.clear();

    m_eLoadState = SKIN_RES_LOAD_STATE_NONE;
    return true;
}

void  SkinRes::SetParentSkinRes(SkinRes* p)
{
    UIASSERT(p != this);
	m_pParentSkinRes = p;
} 

SkinRes*  SkinRes::GetParentSkinRes()
{
	return m_pParentSkinRes;
}

// parse.ProcessFile在解析配置文件过程中，如果发现一个新的文档，
// 则会回调该函数进行通知
void  SkinRes::OnNewUIDocument(UIDocument* pDoc)
{
	if (!pDoc)
		return;

	pDoc->AddRef();
    m_listDoc.push_back(pDoc);
}

bool SkinRes::Save()
{
    _DocList::iterator iter = m_listDoc.begin();
    for (; iter != m_listDoc.end(); iter++)
    {
        UIDocument* po = *iter;

        if (!po->IsDirty())
            continue;

        if (!po->Save())
        {
            UI_LOG_ERROR(_T("%s Save Xml Doc Failed. path=%s"), 
					po->GetSkinPath());
        }
    }

	return true;
}

bool SkinRes::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
    if (nullptr == m_pHLSInfo)
    {
        m_pHLSInfo = new SKIN_HLS_INFO;
    }
    m_pHLSInfo->h = h;
    m_pHLSInfo->l = l;
    m_pHLSInfo->s = s;
    m_pHLSInfo->nFlag = nFlag;

	if (false == m_mgrImage.ChangeSkinHLS(h,l,s,nFlag))
		return false;
	if (false == m_mgrColor.ChangeSkinHLS(h,l,s,nFlag))
		return false;

#if 0 // -- 架构改造	
	// 保存该 HLS 数据
	SkinConfigRes* pSkinConfigRes = m_pSkinConfigMgr->GetSkinConfigRes();
	if (nullptr == pSkinConfigRes)
		return false;

	pSkinConfigRes->SetHLS((char)h,(char)l,(char)s,nFlag);
#endif
	return true;
}


UIDocument*  SkinRes::GetXmlDoc(unsigned int nIndex)
{
    if (nIndex >= GetXmlDocCount())
        return nullptr;

    return m_listDoc[nIndex];
}

UIDocument* SkinRes::GetXmlDocByName(const wchar_t* szName)
{
	if (!szName)
		return nullptr;

	_DocList::iterator iter = m_listDoc.begin();
	for (; iter != m_listDoc.end(); iter++)
	{
		UIDocument* po = *iter;
		if (0 == wcscmp(szName, po->GetSkinPath()))
		{
			return po;
		}
	}
	return nullptr;
}

UIApplication*  SkinRes::GetUIApplication()  
{ 
	return m_mgrSkinRef.GetUIApplication();
}
ISkinManager&  SkinRes::GetISkinManager()   
{ 
	return m_mgrSkinRef.GetISkinManager(); 
}

ImageRes&  SkinRes::GetImageRes()
{
	return m_mgrImage.GetImageRes();
}
ColorRes&  SkinRes::GetColorRes()
{
	return m_mgrColor.GetColorRes();
}
FontRes&  SkinRes::GetFontRes()
{
	return m_mgrFont.GetFontRes();
}

StyleRes&  SkinRes::GetStyleRes()
{
    return m_mgrStyle.GetStyleRes();
}

I18nRes&  SkinRes::GetI18nRes()
{
	return m_mgrI18n.GetI18nRes();
}
}