#include "gifres.h"
#include "src/resource/res_bundle.h"
#include "src/util/gif/gifImage.h"
#include "include/interface/iuires.h"
#include "Inc/Interface/imapattr.h"
#include "src/skin_parse/datasource/skindatasource.h"

namespace ui
{

GifResItem::GifResItem(Application* pUIApp)
{
	m_pMapAttrib = nullptr;
	m_pGifImage = nullptr;
	m_pOriginImageData = nullptr;
	m_bUseSkinHLS = false;
	m_pUIApp = pUIApp;
	m_pIGifResItem = nullptr;
}
GifResItem::~GifResItem()
{
	SAFE_DELETE(m_pGifImage);
	SAFE_RELEASE(m_pMapAttrib);
	SAFE_DELETE(m_pIGifResItem);
}

std::string& GifResItem::GetIdRef()
{
	return m_strId;
}

void GifResItem::SetId(const char* text)
{
	if (text)
		m_strId = text;
	else
		m_strId.clear();
}

void GifResItem::SetPath(const char* path)
{
	if (path)
		m_strPath = path;
	else
		m_strPath.c_str();
}

IGifResItem*  GifResItem::GetIGifResItem()
{
	if (nullptr == m_pIGifResItem)
	{
		m_pIGifResItem = new IGifResItem(this);
	}
	return m_pIGifResItem;
}

GifImageBase*  GifResItem::GetGifImage(Resource* pSkinRes)
{
	if (nullptr == m_pUIApp)
		return nullptr;

	if (nullptr == m_pGifImage)
	{
		std::string strExt = m_strPath.substr(m_strPath.length()-4, 4);

		if (0 == strcasecmp(strExt.c_str(), ".gif"))
		{
			m_pGifImage = new GifImage();
		}
		else
		{
			m_pGifImage = new PngListGifImage();
		}
	}

	if (m_pGifImage && !m_pGifImage->IsLoaded())
	{
		SkinDataSource* pDataSource = pSkinRes->GetDataSource();
		if (!pDataSource)
			return nullptr;

		IStreamBufferReader* pBuffer = nullptr;
		if (!pDataSource->Load_StreamBuffer(m_strPath.c_str(), &pBuffer))
			return nullptr;

		m_pGifImage->Load(pBuffer, m_pMapAttrib);
		SAFE_RELEASE(pBuffer);
	}

	return m_pGifImage;
}

void GifResItem::SetAttribute(IMapAttribute* pMapAttrib) 
{ 
	m_pMapAttrib = pMapAttrib; 
	m_pMapAttrib->AddRef();
}

GifRes::GifRes(Resource* p)
{
	m_pUIApp = nullptr; 
	m_pIGifRes = nullptr;
	m_pSkinRes = p;
}
GifRes::~GifRes()
{
	SAFE_DELETE(m_pIGifRes);
	this->Clear(); 
}

IGifRes*  GifRes::GetIGifRes()
{
	if (nullptr == m_pIGifRes)
		m_pIGifRes = new IGifRes(this);

	return m_pIGifRes;
}

GifResItem* GifRes::LoadItem(IMapAttribute* pMapAttrib, const std::string& strFullPath)
{
	std::string strID;

	const wchar_t* szText = pMapAttrib->GetAttr(XML_ID, true);
	if (szText)
		strID = szText;

	GifResItem* pItem = nullptr;
	if (this->InsertGif(strID, strFullPath, &pItem))
	{
		pItem->SetAttribute(pMapAttrib);
		return pItem;
	}
	else
	{
		UI_LOG_WARN( _T("insert gif id=%s, path=%s failed."), strID.c_str(), strFullPath.c_str());
		return nullptr;
	}
}
int GifRes::GetGifCount() 
{
	return (int)m_vGifs.size();
}

GifResItem* GifRes::GetGifItem( int nIndex )
{
	if (nIndex < 0)
		return nullptr;
	if (nIndex >= (int)m_vGifs.size())
		return nullptr;

	return m_vGifs[nIndex];
}

GifResItem* GifRes::GetGifItem(const wchar_t*  szId)
{
	if (nullptr == szId)
		return nullptr;

	vector<GifResItem*>::iterator  iter = m_vGifs.begin();
	for  (; iter != m_vGifs.end(); iter++)
	{
		GifResItem* p = *iter;
		if (0 == strcmp(szId, p->GetId()))
			return p;
	}
	return nullptr;
}

IGifImage*  GifRes::GetGifImage(const wchar_t* szId)
{
	if (nullptr == szId)
		return false;

	GifResItem* pItem = this->GetGifItem(szId);
	if (!pItem)
	{
		// ��ȡʧ�ܣ���������һ����Դ��ȡ
		if (m_pSkinRes->GetParentSkinRes())
		{
			return m_pSkinRes->GetParentSkinRes()->
				GetImageManager().GetGifRes().GetGifImage(szId);
		}
		return nullptr;
	}

	GifImageBase* pGifImage = pItem->GetGifImage(m_pSkinRes);
	if (!pGifImage)
		return nullptr;

	return pGifImage->GetIGifImage();
}

bool GifRes::InsertGif(const std::string& strID, const std::string& strPath, GifResItem** pRet)
{
	GifResItem* pItem = this->GetGifItem(strID.c_str());
	if (pItem)
	{
		UI_LOG_WARN("failed, insert item=%s, path=%s", strID.c_str(), strPath.c_str() );
		return false;
	}

	GifResItem*  pGifItem = new GifResItem(m_pUIApp);
	pGifItem->SetId(strID.c_str());
	pGifItem->SetPath(strPath.c_str());

	this->m_vGifs.push_back(pGifItem);  
	if (pRet)
		*pRet = pGifItem;
	return true;
}
bool GifRes::ModifyGif(const std::string& strID, const std::string& strPath)
{
	GifResItem* p = this->GetGifItem(strID.c_str());
	if (p)
	{
		return p->ModifyGif(strPath);
	}

	UI_LOG_WARN("failed. modify gif id=%s, path=%s", strID.c_str(), strPath.c_str() );
	return false;
}
bool GifRes::RemoveGif(const std::string& strID)
{
	vector<GifResItem*>::iterator  iter = m_vGifs.begin();
	vector<GifResItem*>::iterator  iterEnd = m_vGifs.end();

	for (; iter != iterEnd; iter++)
	{
		GifResItem* p = *iter;
		if (p->GetIdRef() == strID)
		{
			delete p;
			p = nullptr;
			m_vGifs.erase(iter);
			return true;
		}
	}
	return false;
}
bool GifRes::Clear()
{
	vector<GifResItem*>::iterator  iter = m_vGifs.begin();
	vector<GifResItem*>::iterator  iterEnd = m_vGifs.end();

	for (; iter != iterEnd; iter++)
	{
		GifResItem* p = *iter;
		delete p;
		p = nullptr;
	}

	m_vGifs.clear();
	return true;
}

}