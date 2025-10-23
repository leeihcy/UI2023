#include "imagemanager.h"
#include "include/interface/imapattr.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "res_bundle.h"
#include "src/application/uiapplication.h"

namespace ui {
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             图片                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

ImageManager::ImageManager(ResourceBundle *resource_bundle)
    : m_resImage(resource_bundle)
#if 0 // defined(OS_WIN)
, m_resGif(resource_bundle)
#endif
{
  m_pIImageManager = nullptr;
  m_resource_bundle = resource_bundle;
#if 0 // defined(OS_WIN)    
	m_resGif.SetUIApplication(resource_bundle->GetUIApplication());
#endif
}

ImageManager::~ImageManager(void) {
  this->Clear();
  SAFE_DELETE(m_pIImageManager);

  _MyListItem *pItem = m_listUIElement.GetFirst();
  while (pItem) {
    (**pItem).pXmlElement->Release();
    pItem = pItem->GetNext();
  }
  m_listUIElement.Clear();
}

IImageManager &ImageManager::GetIImageManager() {
  if (!m_pIImageManager)
    m_pIImageManager = new IImageManager(this);

  return *m_pIImageManager;
}

bool ImageManager::ChangeSkinHLS(short h, short l, short s, int nFlag) {
  return m_resImage.ChangeSkinHLS(h, l, s, nFlag);
}

void ImageManager::Clear() { m_resImage.Clear(); }

IImageResItem *ImageManager::InsertImageItem(IMAGE_ITEM_TYPE eType,
                                             const char *szId,
                                             const char *szPath) {
  if (nullptr == szId || nullptr == szPath)
    return nullptr;

  ImageResItem *p = m_resImage.InsertImage(eType, szId, szPath);
  if (!p) {
    UI_LOG_ERROR(L"m_resImage.InsertImage Id=%s, Path=%s Failed. ", szId,
                 szPath);
    return nullptr;
  }

  return p->GetIImageResItem();
}

bool ImageManager::ModifyImageItem(const char *szId, const char *szPath) {
  if (nullptr == szId || nullptr == szPath)
    return false;

  if (false == m_resImage.ModifyImage(szId, szPath)) {
    UI_LOG_ERROR("m_resImage.ModifyImage strID=%s, strPath=%s Failed.", szId,
                 szPath);
    return false;
  }

  return true;
}
bool ImageManager::ModifyImageItemInRunTime(const char *szId,
                                            const char *szPath) {
  if (nullptr == szId || nullptr == szPath)
    return false;

  if (false == m_resImage.ModifyImage(szId, szPath)) {
    UI_LOG_ERROR("m_resImage.ModifyImage strID=%s,strPath=%s Failed. ", szId,
                 szPath);
    return false;
  }

  // TODO: 保存到用户配置文件中
  UI_LOG_DEBUG(L"TODO: 保存到用户配置文件中");

  return true;
}

bool ImageManager::ModifyImageItemAlpha(const char *szId, byte nAlphaPercent) {
  if (nullptr == szId || nAlphaPercent < 0 || nAlphaPercent > 100)
    return false;

  if (false == m_resImage.ModifyImageItemAlpha(szId, nAlphaPercent)) {
    UI_LOG_ERROR(
        "m_resImage.ModifyImageItemAlpha strID=%s,nAlphaPercent=%d Failed. ",
        szId, nAlphaPercent);
    return false;
  }

  // TODO: 保存到用户配置文件中
  UI_LOG_DEBUG("TODO: 保存到用户配置文件中");

  return true;
}

bool ImageManager::RemoveImageItem(const char *szId) {
  if (nullptr == szId)
    return false;

  Application *pUIApp = m_resource_bundle->GetUIApplication();
  if (!pUIApp->IsEditorMode()) {
    if (false == m_resImage.RemoveImage(szId)) {
      UI_LOG_ERROR("m_resImage.RemoveImage strID=%s Failed. ", szId);
      return false;
    }
  } else {
    IImageResItem *pItem = m_resImage.GetImageResItem(szId);
    if (!pItem)
      return false;

    IUIEditor *pEditor = pUIApp->GetUIEditorPtr();
    if (pEditor) {
      pEditor->OnImageItemDeleteInd(pItem);
    }
    return m_resImage.RemoveImage(pItem);
  }

  return true;
}

ImageRes &ImageManager::GetImageRes() { return m_resImage; }
#if 0 // defined(OS_WIN)
CursorRes&  ImageManager::GetCursorRes()
{
	return m_resCursor;
}
GifRes&  ImageManager::GetGifRes()
{
	return m_resGif;
}
#endif

//////////////////////////////////////////////////////////////////////////

int ImageManager::UIParseImageTagCallback(IUIElement *pElem,
                                          IResourceBundle *resource_bundle) {
  IImageManager &pImageMgr = resource_bundle->GetImageManager();
  return pImageMgr.GetImpl()->ParseNewElement(pElem->GetImpl());
}

int ImageManager::ParseNewElement(UIElement *pElem) {
  if (!pElem)
    return 1; // E_INVALIDARG;

#if 0 // 单一版本
	SAFE_RELEASE(m_pUIElement);
    m_pUIElement = pElem;
    m_pUIElement->AddRef();

#elif 1 // 多版本
  ImageTagElementInfo info;
  info.pXmlElement = pElem;
  pElem->AddRef();

  std::string bstrId = pElem->GetAttrib(XML_ID);
  if (!bstrId.empty()) {
    info.strId = bstrId;
  } else {
    // TODO: 取当前xml名称作为id
  }

  m_listUIElement.Add(info);
#endif

  // 遍历其子元素
  UIElementProxy childElement = pElem->FirstChild();
  while (childElement) {
    this->OnNewChild(childElement.get());
    childElement = childElement->NextElement();
  }
  return 0;
}

void ImageManager::OnNewChild(UIElement *pElem) {
  std::string bstrTagName = pElem->GetTagName();

  //	加载所有属性
  std::shared_ptr<IAttributeMap> attribute_map = UICreateIMapAttribute();
  pElem->GetAttribList(attribute_map.get());

  // 获取路径
  std::string bstrPath = pElem->GetData();

  if (bstrPath.empty()) {
    // 如果没有配置在内容中，就检查一下是否是配置成属性了
    const char* path = attribute_map->GetAttr(XML_PATH, true);
    if (path) {
      bstrPath = path;
    }
  }

  if (bstrPath.empty()) {
    return;
  }

  std::string strPath = bstrPath;

  // REMOVED 2013.5.29 交给datasource去判断
  //     std::string  strFullPath;
  //     if (util::IsFullPath((BSTR)bstrPath))
  //     {
  //         strFullPath = bstrPath;
  //     }
  //     else
  //     {
  //         char szFullPath[MAX_PATH] = _T("");
  //         util::CalcFullPathByRelative(m_resource_bundle->GetPath(), (BSTR)bstrPath,
  //         szFullPath);
  //
  //         strFullPath = szFullPath;
  //     }

  if (bstrTagName == XML_IMAGE_ITEM_CURSOR) {
#if 0 // defined(OS_WIN)
        CursorResItem* p = m_resCursor.LoadItem(attribute_map, strPath.c_str());
        if (p)
        {
            IUIEditor* pEditor = m_resource_bundle->GetUIApplication()->GetUIEditorPtr();
            if (pEditor)
            {
                pEditor->OnCursorItemLoad(
                        p->GetICursorResItem(), 
                        pElem->GetIUIElement());
            }
        }
        else
        {
            UI_LOG_WARN("insert cursor failed. path=%s", strPath.c_str());
        }
#endif
  } else if (bstrTagName == XML_IMAGE_ITEM_GIF) {
#if 0 // defined(OS_WIN)
        GifResItem* p = m_resGif.LoadItem(attribute_map, strPath);
        if (p)
        {
            IUIEditor* pEditor = m_resource_bundle->GetUIApplication()->GetUIEditorPtr();
            if (pEditor)
            {
                pEditor->OnGifItemLoad(
                        p->GetIGifResItem(), 
                        pElem->GetIUIElement());
            }
        }
        else
        {
            UI_LOG_WARN("insert gif failed. path=%s", strPath.c_str());
        }
#endif
  } else {
    ImageResItem *p =
        m_resImage.LoadItem(bstrTagName.c_str(), attribute_map.get(), strPath.c_str());
    if (p) {
      IUIEditor *pEditor = m_resource_bundle->GetUIApplication()->GetUIEditorPtr();
      if (pEditor) {
        pEditor->OnImageItemLoad(p->GetIImageResItem(), pElem->GetIUIElement());
      }
    } else {
      UI_LOG_WARN("insert image failed. path=%s", strPath.c_str());
    }
  }
}

UIElement *ImageManager::GetImageXmlElem(const char *szId) {
  if (0 == m_listUIElement.GetCount())
    return nullptr;

  if (!szId) {
    return m_listUIElement.GetFirst()->m_data.pXmlElement;
  }

  _MyListItem *pItem = m_listUIElement.GetFirst();
  while (pItem) {
    if ((**pItem).strId == szId) {
      return (**pItem).pXmlElement;
    }
    pItem = pItem->GetNext();
  }
  return nullptr;
}

} // namespace ui