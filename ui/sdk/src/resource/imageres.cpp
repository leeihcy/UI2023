#include "imageres.h"
#include "include/inc.h"

#include "image/image.h"
// #include "src\Renderlibrary\gdi\gdibitmap.h"
// #ina clude "src/Renderlibrary\gdiplus\gdiplusbitmap.h"
#include "src/resource/res_bundle.h"
// #include "Inc\Util\iimage.h"
#include "include/macro/xmldefine.h"
#include "include/interface/iuires.h"
#include "src/skin_parse/datasource/skindatasource.h"
#include "src/util/DPI/dpihelper.h"
#include "include/interface/imapattr.h"
#include "uiresource.h"
#include "src/application/uiapplication.h"
#include <math.h>

namespace ui 
{
ImageResItem::ImageResItem()
{
	m_pMapAttrib = nullptr;
#if defined(OS_WIN)
	m_pGdiBitmap = nullptr;
#endif
	//m_pGdiplusBitmap = nullptr;
	m_pOriginImageData = nullptr;
	m_pIImageResItem = nullptr;

    m_bDpiAdapt = true;
	m_bUseSkinHLS = false;
	m_bNeedAntiAliasing = false;
	m_bMustHasAlphaChannel = true;  // 默认都创建32位alpha channel，现在大部分gdi render也支持fix alpha channel了
	m_eType = IMAGE_ITEM_TYPE_IMAGE;
	m_nFileDpiScale = 1;
}

ImageResItem::~ImageResItem()
{
#if defined(OS_WIN)
    // 不直接删除，还有控件在引用这个图片（如在UIEditor中删除一张图片），由引用者负责释放其引用的图片指针
    if (m_pGdiBitmap)
    {
        m_pGdiBitmap->Destroy();
		m_pGdiBitmap->SetOutRef(nullptr); // 注：nnd，搞了我一天才发现没的置空导致的莫名其妙的崩溃，自己都析构了，还通知个毛，不崩溃才怪
    }
#endif
//     if (m_pGdiplusBitmap)
//     {
//         m_pGdiplusBitmap->Destroy();
// 		m_pGdiplusBitmap->SetOutRef(nullptr);
//     }
	SAFE_DELETE(m_pOriginImageData);
	SAFE_RELEASE(m_pMapAttrib);
	SAFE_DELETE(m_pIImageResItem);
}

IImageResItem*  ImageResItem::GetIImageResItem()
{
	if (nullptr == m_pIImageResItem)
		m_pIImageResItem = new IImageResItem(this);

	return m_pIImageResItem;
}

IMAGE_ITEM_TYPE  ImageResItem::GetImageItemType()
{
    return m_eType;
}
void  ImageResItem::SetImageItemType(IMAGE_ITEM_TYPE e)
{
    if (e == IMAGE_ITEM_TYPE_IMAGE ||
        e == IMAGE_ITEM_TYPE_ICON || 
        e == IMAGE_ITEM_TYPE_IMAGE_LIST)
        m_eType = e;
}

bool  ImageResItem::IsMyRenderBitmap(IRenderBitmap* pRenderBitmap)
{
#if defined(OS_WIN)
	if (m_pGdiBitmap == pRenderBitmap) 
	{
		return true;
	}  
#endif
	return false;
}

IRenderBitmap* ImageResItem::GetImage(ResBundle* pSkinRes, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, bool* pbFirstTimeCreate)
{
	if (nullptr == pSkinRes)
		return nullptr;

	switch (eRenderType)
	{
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		 return GetImage_gdi(pSkinRes, pbFirstTimeCreate);	

	case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
//         return GetImage_gdiplus(pSkinRes, pbFirstTimeCreate);	
// 
    case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
//         return GetImage_d2d(pSkinRes, pbFirstTimeCreate);
// 
    case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D:
//         return GetImage_d3d(pSkinRes, pbFirstTimeCreate);

    default:
    return nullptr;
	}

	return nullptr;
}

IRenderBitmap*  ImageResItem::GetImage_gdi(ResBundle* pSkinRes, bool* pbFirstTimeCreate)
{
#if defined(OS_WIN)
    SkinDataSource* pDataSource = pSkinRes->GetDataSource();
    if (nullptr == pDataSource)
        return nullptr;

	if (m_bNeedAntiAliasing)
	{
		// TODO:
		UIASSERT(0);
		//return GetImage_gdiplus(pSkinRes, pbFirstTimeCreate);
	}

    if (m_pGdiBitmap )
    {
        m_pGdiBitmap->AddRef();
        return (IRenderBitmap*)m_pGdiBitmap;
    }    

    RenderBitmapFactory::CreateInstance(
        pSkinRes->GetUIApplication()->GetIUIApplication(),
        GRAPHICS_RENDER_LIBRARY_TYPE_GDI,
        m_eType, 
        (IRenderBitmap**)&m_pGdiBitmap);

    if (!m_pGdiBitmap)
        return nullptr;
    
    m_pGdiBitmap->SetOutRef((IRenderResource **)&m_pGdiBitmap);
    SetRenderBitmapAttribute(m_pGdiBitmap);

    /*if (false == m_pGdiBitmap->LoadFromFile(m_strPath.c_str(), m_bMustHasAlphaChannel))*/
	// 2015.2.11 gdi也可以用于透明窗口了。干脆将所有图片都加载为32位的，省的gdi的jpg图片无法在透明窗口中显示
    long flags = RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL;
	if (NeedDpiAdapt())
	{
		flags |= RENDER_BITMAP_LOAD_DPI_ADAPT;
		flags |= (m_nFileDpiScale) << 24;
	}

    if (false == pDataSource->Load_RenderBitmap(m_pGdiBitmap, 
                    m_strPath.c_str(), 
                    (RENDER_BITMAP_LOAD_FLAG)flags))  
    {
        SAFE_RELEASE(m_pGdiBitmap);
        UI_LOG_ERROR(_T("Load gdi bitmap from file failed. path=%s"), 
            m_strPath.c_str());
        return nullptr;
    }

    if (pbFirstTimeCreate)
        *pbFirstTimeCreate = true;

    return (IRenderBitmap*)m_pGdiBitmap;
#else
    return nullptr;
#endif
}


bool  ImageResItem::ModifyHLS(short h, short l, short s, int nFlag)
{
	if (false == m_bUseSkinHLS)
		return true;
#if defined(OS_WIN)
	ModifyHLS(m_pGdiBitmap, h,l,s,nFlag);

	// 	if (m_pGdiBitmap)
	// 		m_pGdiBitmap->GetBitmap()->Save(_T("c:\\aaa.png"),Gdiplus::ImageFormatPNG);
    return true;
#else
	return false;
#endif
}
bool ImageResItem::ModifyHLS(IRenderBitmap* pBitmap, short h, short l, short s, int nFlag)
{
	if (false == m_bUseSkinHLS)
		return true;

	if (pBitmap)
	{
		if (nullptr == m_pOriginImageData)
		{
			m_pOriginImageData = new ImageData;
			if (false == pBitmap->SaveBits(m_pOriginImageData))
			{
				UI_LOG_WARN(_T("not support this image to change hue. id=%s"), m_strId.c_str() );
				m_bUseSkinHLS = false;
				SAFE_DELETE(m_pOriginImageData);
			}
		}
		return pBitmap->ChangeHSL(m_pOriginImageData, h,s,l,nFlag);
	}
	return false;
}

bool ImageResItem::ModifyAlpha(byte nAlphaPercent)
{
#if defined(OS_WIN)
	if (m_pGdiBitmap)
	{
		if (nullptr == m_pOriginImageData)
		{
			m_pOriginImageData = new ImageData;
			if (false == m_pGdiBitmap->SaveBits(m_pOriginImageData))
			{
				UI_LOG_WARN(_T("not support this image to change alpha. id=%s"), m_strId.c_str() );
				SAFE_DELETE(m_pOriginImageData);
			}
		}
		return m_pGdiBitmap->GetBitmap()->ModifyAlpha(m_pOriginImageData, nAlphaPercent);
	}
	return false;
#else
    return false;
#endif
}

//
// 修改图片
//
// 注意：1. 外界根本就不会知道image已经被重新创建的事实，因为没有指针被销毁
//       2. 但是有一个问题，如果第一次加载的是32位带alpha的图片，返回出去的将是GdiBitmap，
//          这时候如果modify为一个非32位的位图，将导致GdiBitmap没有alpha通道，在分层窗口上
//          显示失败
//
bool ImageResItem::ModifyImage(const wchar_t* szPath)
{
	if (nullptr == szPath)
		return false;

	m_strPath = szPath;

	SAFE_DELETE(m_pOriginImageData);
#if defined(OS_WIN)
	if (m_pGdiBitmap)
	{
        long flags = 0;
        if (m_bMustHasAlphaChannel)
            flags |= RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL;
        if (NeedDpiAdapt())
            flags |= RENDER_BITMAP_LOAD_DPI_ADAPT;
		if (!m_pGdiBitmap->LoadFromFile(m_strPath.c_str(), (RENDER_BITMAP_LOAD_FLAG)flags))
		{
			UI_LOG_ERROR(_T("load from file failed 1:%s"), m_strPath.c_str());
		}
	}

//     long flags = RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL;
//     if (NeedDpiAdapt())
//         flags |= RENDER_BITMAP_LOAD_DPI_ADAPT;
// 	if (m_pGdiplusBitmap)
// 	{
// 		if (!m_pGdiplusBitmap->LoadFromFile(m_strPath.c_str(), (RENDER_BITMAP_LOAD_FLAG)flags))
// 		{
// 			UI_LOG_ERROR(_T("load from file failed 2:%s"), m_strPath.c_str());
// 		}
// 	}

	return true;
#else
    return false;
#endif
}

bool  ImageResItem::NeedDpiAdapt()
{
    if (!m_bDpiAdapt)
        return false;

    if (GetDpi() == DEFAULT_SCREEN_DPI)
        return false;

    return true;
}
void ImageResItem::SetAttribute(IMapAttribute* pMapAttrib) 
{ 
	m_pMapAttrib = pMapAttrib;
	m_pMapAttrib->AddRef();

	bool  bUseSkinHLS = false;
	bool  bNeedAntiAliasing = false;
    bool  bDpiAdapt = true;

	pMapAttrib->GetAttr_bool(XML_IMAGE_THEMEHLS, true, &bUseSkinHLS);
	this->SetUseSkinHLS(bUseSkinHLS);

	pMapAttrib->GetAttr_bool(XML_IMAGE_NEED_ANTIALIASING, true, &bNeedAntiAliasing);
	this->SetNeedAntiAliasing(bNeedAntiAliasing);

    pMapAttrib->GetAttr_bool(XML_IMAGE_DPI_ADAPT, true, &bDpiAdapt);
    m_bDpiAdapt = bDpiAdapt;
}

// TODO: 该函数以后将被干掉，还差一个icon没有重构
void  ImageResItem::SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap)
{
}

const wchar_t* ui::ImageResItem::GetId()
{
	return m_strId.c_str();
}

void ui::ImageResItem::SetId(const wchar_t* text)
{
	if (text)
		m_strId = text; 
	else
		m_strId.clear();
}

void ui::ImageResItem::SetPath(const wchar_t* text)
{
	if (text)
		m_strPath = text;
	else
		m_strPath.clear();
}

const wchar_t* ui::ImageResItem::GetPath()
{
	return m_strPath.c_str();
}

//////////////////////////////////////////////////////////////////////////
ImageListResItem::ImageListResItem()
{
    m_eType = IMAGE_ITEM_TYPE_IMAGE_LIST;
    m_eLayoutType = IMAGELIST_LAYOUT_TYPE_H;
    m_nCount = 1;
}

IImageResItem*  ImageListResItem::GetIImageResItem()
{
    if (nullptr == m_pIImageResItem)
        m_pIImageResItem = new IImageListResItem(this);

    return m_pIImageResItem;   
}
IMAGELIST_LAYOUT_TYPE  ImageListResItem::GetLayoutType()
{
    return m_eLayoutType;
}

int  ImageListResItem::GetItemCount()
{
    return m_nCount;
}
void  ImageListResItem::SetLayoutType(IMAGELIST_LAYOUT_TYPE e)
{
    m_eLayoutType = e;
}
void  ImageListResItem::SetItemCount(int n)
{   
    m_nCount = n;
}
void  ImageListResItem::SetAttribute(IMapAttribute* pMapAttrib)
{
    ImageResItem::SetAttribute(pMapAttrib);

    const wchar_t* szText = pMapAttrib->GetAttr(XML_IMAGE_IMAGELIST_LAYOUT, true);
    if (szText)
    {
        if (0 == wcscmp(szText, XML_IMAGE_IMAGELIST_LAYOUT_V))
            m_eLayoutType = IMAGELIST_LAYOUT_TYPE_V;
        else
            m_eLayoutType = IMAGELIST_LAYOUT_TYPE_H;
    }

    pMapAttrib->GetAttr_int(XML_IMAGE_IMAGELIST_COUNT, true, &m_nCount);
}

void  ImageListResItem::SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap)
{
    if (!pRenderBitmap)
        return;

    if (pRenderBitmap->GetImageType() != IMAGE_ITEM_TYPE_IMAGE_LIST)
        return;

    IImageListRenderBitmap* pImageListBmp = static_cast<IImageListRenderBitmap*>(pRenderBitmap);
    pImageListBmp->SetItemCount(m_nCount);
    pImageListBmp->SetLayoutType(m_eLayoutType);
}


//////////////////////////////////////////////////////////////////////////

ImageIconResItem::ImageIconResItem()
{
    m_eType = IMAGE_ITEM_TYPE_ICON;
    m_sizeDraw.width = 16;
    m_sizeDraw.height = 16;
}

IImageResItem*  ImageIconResItem::GetIImageResItem()
{
    if (nullptr == m_pIImageResItem)
        m_pIImageResItem = new IImageIconResItem(this);

    return m_pIImageResItem;   
}
void  ImageIconResItem::SetAttribute(IMapAttribute* pMapAttrib)
{
    ImageResItem::SetAttribute(pMapAttrib);

    pMapAttrib->GetAttr_int(XML_IMAGE_ICON_WIDTH,  true, (int*)&m_sizeDraw.width);
    pMapAttrib->GetAttr_int(XML_IMAGE_ICON_HEIGHT, true, (int*)&m_sizeDraw.height);
}

void  ImageIconResItem::SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap)
{
    if (!pRenderBitmap)
        return;

    if (pRenderBitmap->GetImageType() != IMAGE_ITEM_TYPE_ICON)
        return;

    IImageIconRenderBitmap* pIcon = static_cast<IImageIconRenderBitmap*>(pRenderBitmap);
    pIcon->SetDrawSize(&m_sizeDraw);
}

//////////////////////////////////////////////////////////////////////////
//
//	ImageRes
//

ImageRes::ImageRes(ResBundle*  pSkinRes)
{
	m_pSkinRes = pSkinRes; 
	m_pIImageRes = nullptr;
}

ImageRes::~ImageRes()
{	
	this->Clear(); 
	SAFE_DELETE(m_pIImageRes);
}

IImageRes&  ImageRes::GetIImageRes()
{
	if (nullptr == m_pIImageRes)
		m_pIImageRes = new IImageRes(this);

	return *m_pIImageRes;
}

//
// 从文件中加载一项(由CXmlImageParse::load_from_file中调用)
//
ImageResItem* ImageRes::LoadItem(const wchar_t* szType, IMapAttribute* pMapAttrib, const wchar_t* szFullPath)
{
    if (!szType || !pMapAttrib || !szFullPath)
        return nullptr;

    const wchar_t* szId = pMapAttrib->GetAttr(XML_ID, true);
    if (!szId)
        return nullptr;

    IMAGE_ITEM_TYPE eType = IMAGE_ITEM_TYPE_IMAGE;
    if (0 == wcscmp(szType, XML_IMAGE_ITEM_TYPE_ICON))
    {
        eType = IMAGE_ITEM_TYPE_ICON;
    }
    else if (0 == wcscmp(szType, XML_IMAGE_ITEM_TYPE_IMAGELIST))
    {
        eType = IMAGE_ITEM_TYPE_IMAGE_LIST;
    }
    else 
    {
        int lLength = (int)wcslen(szFullPath);
        if (lLength > 4 && 0 == wcsicmp((szFullPath+lLength-4), _T(".ico")))
        {
            eType = IMAGE_ITEM_TYPE_ICON;
        }
        else
        {
            eType = IMAGE_ITEM_TYPE_IMAGE;
        }
    }

	ImageResItem* pItem = this->InsertImage(eType, szId, szFullPath);
	if (!pItem)
	{
        UI_LOG_WARN( _T("insert image m_strID=%s, path=%s failed."), szId, szFullPath);
        return nullptr;
	}
	
    pItem->SetAttribute(pMapAttrib);
    return pItem;
}

long ImageRes::GetImageCount() 
{
    return (long)m_mapImages.size();
}

IImageResItem* ImageRes::GetImageResItem(long lIndex)
{
	ImageResItem* pItem = this->GetImageItem2(lIndex);
	if (nullptr == pItem)
		return nullptr;

	return pItem->GetIImageResItem();
}
IImageResItem*  ImageRes::GetImageResItem(const wchar_t* szId)
{
	if (nullptr == szId)
		return nullptr;

    ImageResItem* p = GetImageItem2(szId);
    if (p)
        return p->GetIImageResItem();
    return nullptr;
}

ImageResItem* ImageRes::GetImageItem2(int nIndex)
{
	if (nIndex < 0)
		return nullptr;

    if (nIndex >= (int)m_mapImages.size())
        return nullptr;

    _MyIter iter = m_mapImages.begin();
    for (int i = 0; i < nIndex; i++)
    {
        iter++;
    }

    return iter->second;
}

ImageResItem* ImageRes::GetImageItem2(const wchar_t* szId)
{
    String strId(szId);
    _MyIter iter = m_mapImages.find(strId);
    if (iter == m_mapImages.end())
        return nullptr;

    return  iter->second;
}

ImageResItem*  ImageRes::InsertImage(IMAGE_ITEM_TYPE eType, const wchar_t* szId, const wchar_t* szPath)
{
    if (!szId || !szPath)
        return nullptr;

    ImageResItem* pItem = this->GetImageItem2(szId);
    if (pItem)
    {
        UI_LOG_WARN(_T("failed, insert item=%s, path=%s"), szId, szPath);
        return nullptr;
    }

    ImageResItem*  pImageItem = nullptr;
    switch (eType)
    {
    case IMAGE_ITEM_TYPE_IMAGE_LIST:
        pImageItem = new ImageListResItem;
        break;

    case IMAGE_ITEM_TYPE_ICON:
        pImageItem = new ImageIconResItem;
        break;

    default:
        pImageItem = new ImageResItem;
        break;
    }

    if (!pImageItem)
        return nullptr;


	pImageItem->SetId(szId);
	pImageItem->SetPath(szPath);

	// DPI适配，查找文件是否在@2x,@3x图片
	do 
	{
		if (ui::GetDpi() == DEFAULT_SCREEN_DPI)
			break;

		SkinDataSource* pDataSource = m_pSkinRes->GetDataSource();
		if (!pDataSource)
			break;

		float fScale = GetDpiScale();
		int nScale = (int)ceil(fScale);

		// 拆文件路径，用于重新组装
		const wchar_t* p = wcsrchr(szPath, TEXT('.'));
		if (!p)
		{
			UI_LOG_WARN(_T("invalid path: %s"), szPath);
			break;
		}

		String str(szPath);
		String::size_type pos = str.find_last_of(TEXT('.'));
		if (pos == String::npos)
		{
			UI_LOG_WARN(_T("invalid path: %s"), szPath);
			break;
		}

			
		wchar szInsert[16] = { 0 };
		for (int i = nScale; i > 1; i--)
		{
			wprintf(szInsert, TEXT("@%dx"), i);
			String strTest(str);
			strTest.insert(pos, szInsert);

			if (pDataSource->FileExist(strTest.c_str()))
			{
				pImageItem->SetPath(strTest.c_str());
				pImageItem->SetFileDpiScale(i);

				break;
			}
		}
	} while (0);
	

	String  strId(szId);
    m_mapImages[strId] = pImageItem;
	return pImageItem;
}

bool ImageRes::ModifyImage(const wchar_t* szId, const wchar_t* szPath)
{
	if (nullptr == szId || nullptr == szPath)
		return false;

	ImageResItem* p = this->GetImageItem2(szId);
	if (p)
	{
		bool bRet = p->ModifyImage(szPath);
		if (p->GetUseSkinHLS())
		{
			// 检查当前皮肤的HLS
			if (m_pSkinRes && m_pSkinRes->GetHLSInfo())
			{
				SKIN_HLS_INFO* pHLSInfo = m_pSkinRes->GetHLSInfo();
				p->ModifyHLS(pHLSInfo->h, pHLSInfo->l, pHLSInfo->s, pHLSInfo->nFlag);
			}
		}

		return bRet;
	}

	UI_LOG_WARN(_T("failed. modify image id=%s, path=%s"), szId, szPath);
	return false;
}
bool ImageRes::RemoveImage(const wchar_t* szId)
{
    if (!szId)
        return false;

    String strId(szId);
    _MyIter iter = m_mapImages.find(strId);
    if (iter == m_mapImages.end())
        return false;

    ImageResItem* p = iter->second;
    delete p;
    m_mapImages.erase(iter);
    return true;
}
bool ImageRes::RemoveImage(IImageResItem* pItem)
{
    if (!pItem)
        return false;

    ImageResItem* pItemImpl = pItem->GetImpl();
    
    _MyIter iter = m_mapImages.begin();
    for (; iter != m_mapImages.end(); iter++)
    {
        if (iter->second == pItemImpl)
        {
            delete pItemImpl;
            m_mapImages.erase(iter);
            return true;
        }
    }

    return false;
}
bool ImageRes::Clear()
{
    _MyIter iter = m_mapImages.begin();
    for (; iter != m_mapImages.end(); iter++)
    {
        ImageResItem* p = iter->second;
        delete p;
    }
    m_mapImages.clear();
	return true;
}

bool ImageRes::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
    _MyIter iter = m_mapImages.begin();
    for (; iter != m_mapImages.end(); iter++)
    {
        ImageResItem* p = iter->second;
        p->ModifyHLS(h,l,s,nFlag);
    }

	return true;
}

bool ImageRes::ModifyImageItemAlpha(const String& strID, byte nAlphaPercent)
{
    _MyIter iter = m_mapImages.find(strID);
    if (iter == m_mapImages.end())
        return false;

    ImageResItem* p = iter->second;
    return p->ModifyAlpha(nAlphaPercent);

	return false;
}

HBITMAP  ImageRes::LoadBitmap(const wchar_t* szId)
{
#if defined(OS_WIN)
	if (nullptr == szId)
		return nullptr;

	ImageResItem* pItem = this->GetImageItem2(szId);
	if (nullptr == pItem)
	{
		UI_LOG_ERROR( _T("GetImageItem：%s failed .1"), szId);
		return nullptr;
	}

	SkinDataSource* pDataSource = m_pSkinRes->GetDataSource();
	if (nullptr == pDataSource)
		return nullptr;

	ImageWrap image;
	pDataSource->Load_Image(pItem->GetPath(), &image);    

	if (image.IsNull())
		return nullptr;

	return image.Detach();
#else
    return 0;
#endif
}

bool  ImageRes::GetBitmap(const wchar_t* szImageID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, IRenderBitmap** pRenderBitmap)
{
	if (!szImageID || !szImageID[0] || !pRenderBitmap)
		return false;

	ImageResItem* pItem = this->GetImageItem2(szImageID);
	if (!pItem)
	{
		// 获取失败，尝试向上一级资源获取
		if (m_pSkinRes->GetParentSkinRes())
		{
			return m_pSkinRes->GetParentSkinRes()->
				GetImageRes().GetBitmap(szImageID, eRenderType, pRenderBitmap);
		}

		UI_LOG_ERROR( _T("GetImageItem：%s failed .1"), szImageID);
		return false;
	}

	bool bFirstTimeCreate = false;
	IRenderBitmap* pBitmap = pItem->GetImage(m_pSkinRes, eRenderType, &bFirstTimeCreate);
	if (!pBitmap)
	{
		UI_LOG_ERROR( _T("GetImage：%s failed .2"), szImageID);
		return false;
	}

	if (bFirstTimeCreate && pItem->GetUseSkinHLS())
	{
		// 检查当前皮肤的HLS
		if (m_pSkinRes && m_pSkinRes->GetHLSInfo())
		{
			SKIN_HLS_INFO* pHLSInfo = m_pSkinRes->GetHLSInfo();
			pItem->ModifyHLS(pBitmap, pHLSInfo->h, pHLSInfo->l, pHLSInfo->s, pHLSInfo->nFlag);
		}
	}

	*pRenderBitmap = pBitmap;
	return true;
}

const wchar_t*  ImageRes::GetRenderBitmapId(IRenderBitmap* pBitmap)
{
	if (!pBitmap)
		return nullptr;

	_MyIter iter = m_mapImages.begin();
	for (; iter != m_mapImages.end(); iter++)
	{
		if (iter->second->IsMyRenderBitmap(pBitmap))
		{
			return iter->second->GetId();
		}
	}

    ResBundle* pParent = m_pSkinRes->GetParentSkinRes();
    if (pParent)
    {
        return pParent->GetImageRes().GetRenderBitmapId(pBitmap);
    }
	return nullptr;
}

}