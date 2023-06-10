#include "StdAfx.h"
#include "imagemanager.h"
#include "skinres.h"
#include "Inc\Interface\ixmlwrap.h"
#include "Inc\Interface\iuires.h"
#include "..\Base\Application\uiapplication.h"
#include "Inc\Interface\imapattr.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             图片                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


ImageManager::ImageManager(SkinRes* pSkinRes) : m_resImage(pSkinRes), m_resGif(pSkinRes)
{
    m_pIImageManager = nullptr;
	m_pSkinRes = pSkinRes;
	m_resGif.SetUIApplication(pSkinRes->GetUIApplication());
}

ImageManager::~ImageManager(void)
{
	this->Clear();
    SAFE_DELETE(m_pIImageManager);

	_MyListItem* pItem = m_listUIElement.GetFirst();
	while (pItem)
	{
		(**pItem).pXmlElement->Release();
		pItem = pItem->GetNext();
	}
	m_listUIElement.Clear();
}

IImageManager&  ImageManager::GetIImageManager()
{
    if (!m_pIImageManager)
        m_pIImageManager = new IImageManager(this);

    return *m_pIImageManager;
}


bool ImageManager::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	return m_resImage.ChangeSkinHLS(h,l,s,nFlag);
}

void ImageManager::Clear()
{
	m_resImage.Clear();
}

IImageResItem* ImageManager::InsertImageItem(IMAGE_ITEM_TYPE eType, const TCHAR* szId, const TCHAR* szPath)
{
	if (nullptr == szId || nullptr == szPath)
		return nullptr;

	ImageResItem* p = m_resImage.InsertImage(eType, szId, szPath);
    if (!p)
	{
		UI_LOG_ERROR(_T("m_resImage.InsertImage Id=%s, Path=%s Failed. "), szId, szPath);
		return nullptr;
	}

	return p->GetIImageResItem();
}

bool ImageManager::ModifyImageItem(const TCHAR* szId, const TCHAR* szPath)
{
	if (nullptr == szId || nullptr == szPath)
		return false;

	if (false == m_resImage.ModifyImage(szId, szPath))
	{
		UI_LOG_ERROR(_T("m_resImage.ModifyImage strID=%s, strPath=%s Failed."), szId, szPath);
		return false;
	}

	return true;
}
bool ImageManager::ModifyImageItemInRunTime(const TCHAR* szId, const TCHAR* szPath)
{
	if (nullptr == szId || nullptr == szPath)
		return false;

	if (false == m_resImage.ModifyImage(szId, szPath))
	{
		UI_LOG_ERROR(_T("m_resImage.ModifyImage strID=%s,strPath=%s Failed. "), szId, szPath);
		return false;
	}

	// TODO: 保存到用户配置文件中
	UI_LOG_DEBUG(_T("TODO: 保存到用户配置文件中"));
	
	return true;
}

bool ImageManager::ModifyImageItemAlpha(const TCHAR* szId, byte nAlphaPercent)
{
	if (nullptr == szId || nAlphaPercent < 0 || nAlphaPercent > 100)
		return false;

	if (false == m_resImage.ModifyImageItemAlpha(szId, nAlphaPercent))
	{
		UI_LOG_ERROR(_T("m_resImage.ModifyImageItemAlpha strID=%s,nAlphaPercent=%d Failed. "), szId, nAlphaPercent);
		return false;
	}

	// TODO: 保存到用户配置文件中
	UI_LOG_DEBUG(_T("TODO: 保存到用户配置文件中"));

	return true;
}

bool ImageManager::RemoveImageItem(const TCHAR* szId)
{
	if (nullptr == szId)
		return false;

    UIApplication* pUIApp = m_pSkinRes->GetUIApplication();
    if (!pUIApp->IsEditorMode())
    {
	    if (false == m_resImage.RemoveImage(szId))
	    {
		    UI_LOG_ERROR(_T("m_resImage.RemoveImage strID=%s Failed. "), szId);
		    return false;
	    }
    }
    else
    {
        IImageResItem* pItem = m_resImage.GetImageResItem(szId);
        if (!pItem)
            return false;

        IUIEditor* pEditor = pUIApp->GetUIEditorPtr();
        if (pEditor)
        {
            pEditor->OnImageItemDeleteInd(pItem);
        }
        return m_resImage.RemoveImage(pItem);
    }

	return true;
}

ImageRes&  ImageManager::GetImageRes()
{
	return m_resImage;
}
CursorRes&  ImageManager::GetCursorRes()
{
	return m_resCursor;
}
GifRes&  ImageManager::GetGifRes()
{
	return m_resGif;
}


//////////////////////////////////////////////////////////////////////////

HRESULT  ImageManager::UIParseImageTagCallback(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IImageManager&  pImageMgr = pSkinRes->GetImageManager();
    return pImageMgr.GetImpl()->ParseNewElement(pElem->GetImpl());
}

HRESULT  ImageManager::ParseNewElement(UIElement* pElem)
{
	if (!pElem)
		return E_INVALIDARG;

#if 0 // 单一版本
	SAFE_RELEASE(m_pUIElement);
    m_pUIElement = pElem;
    m_pUIElement->AddRef();
    
#elif 1 // 多版本
	ImageTagElementInfo info;
	info.pXmlElement = pElem;
	pElem->AddRef();

	CComBSTR bstrId;
	if (pElem->GetAttrib(XML_ID, &bstrId))
	{
		info.strId = bstrId;
	}
	else
	{
		// TODO: 取当前xml名称作为id
	}

	m_listUIElement.Add(info);
#endif

	// 遍历其子元素
	UIElementProxy childElement = pElem->FirstChild();
	while (childElement)
	{
		this->OnNewChild(childElement.get());
		childElement = childElement->NextElement();
	}
    return S_OK;
}

void  ImageManager::OnNewChild(UIElement* pElem)
{
    CComBSTR bstrTagName = pElem->GetTagName();

    //	加载所有属性
    IMapAttribute* pMapAttrib = nullptr;
    pElem->GetAttribList(&pMapAttrib);

    // 获取路径
    CComBSTR  bstrPath = pElem->GetData();
    
    if (0 == bstrPath.Length())
        bstrPath = pMapAttrib->GetAttr(XML_PATH, true);  // 如果没有配置在内容中，就检查一下是否是配置成属性了
    
    if (0 == bstrPath.Length())
    {
        SAFE_RELEASE(pMapAttrib);
        return;
    }

    String strPath;
    if (bstrPath)
        strPath = CW2T(bstrPath);

    // REMOVED 2013.5.29 交给datasource去判断
//     String  strFullPath;
//     if (Util::IsFullPath((BSTR)bstrPath))
//     {
//         strFullPath = bstrPath;
//     }
//     else
//     {
//         TCHAR szFullPath[MAX_PATH] = _T("");
//         Util::CalcFullPathByRelative(m_pSkinRes->GetPath(), (BSTR)bstrPath, szFullPath);
// 
//         strFullPath = szFullPath;
//     }

    if (bstrTagName == XML_IMAGE_ITEM_CURSOR)
    {
        CursorResItem* p = m_resCursor.LoadItem(pMapAttrib, strPath.c_str());
        if (p)
        {
            IUIEditor* pEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
            if (pEditor)
            {
                pEditor->OnCursorItemLoad(
                        p->GetICursorResItem(), 
                        pElem->GetIUIElement());
            }
        }
        else
        {
            UI_LOG_WARN(_T("insert cursor failed. path=%s"), strPath.c_str());
        }
    }
    else if (bstrTagName == XML_IMAGE_ITEM_GIF)
    {
        GifResItem* p = m_resGif.LoadItem(pMapAttrib, strPath);
        if (p)
        {
            IUIEditor* pEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
            if (pEditor)
            {
                pEditor->OnGifItemLoad(
                        p->GetIGifResItem(), 
                        pElem->GetIUIElement());
            }
        }
        else
        {
            UI_LOG_WARN(_T("insert gif failed. path=%s"), strPath.c_str());
        }
    }
    else
    {
        ImageResItem* p = m_resImage.LoadItem((LPCTSTR)bstrTagName, pMapAttrib, strPath.c_str());
        if (p)
        {
            IUIEditor* pEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
            if (pEditor)
            {
                pEditor->OnImageItemLoad(
                    p->GetIImageResItem(), 
                    pElem->GetIUIElement());
            }
        }
        else
        {
            UI_LOG_WARN(_T("insert image failed. path=%s"), strPath.c_str());
        }
    }

    SAFE_RELEASE(pMapAttrib);
}

UIElement*  ImageManager::GetImageXmlElem(LPCTSTR szId)
{
	if (0 == m_listUIElement.GetCount())
		return nullptr;

	if (!szId)
	{
		return m_listUIElement.GetFirst()->m_data.pXmlElement;
	}

	_MyListItem* pItem = m_listUIElement.GetFirst();
	while (pItem)
	{
		if ((**pItem).strId == szId)
		{
			return (**pItem).pXmlElement;
		}
		pItem = pItem->GetNext();
	}
	return nullptr;
}