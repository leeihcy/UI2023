#pragma once
#include "imageres.h"
#if defined(OS_WIN)
#include "cursorres.h"
#include "gifres.h"
#endif
#include "src/util/DataStruct/list.h"

namespace ui
{
	struct UIElement;
	struct IUIElement;
	struct IImageManager;

class  ImageTagElementInfo
{
public:
	String  strId;  // ģ����
	UIElement*  pXmlElement;  // xml���
};

class ImageManager
{
public:
	ImageManager(Resource* pSkinRes);
	~ImageManager(void);

    IImageManager&  GetIImageManager();

	ImageRes&    GetImageRes();
#if defined(OS_WIN)
	CursorRes&   GetCursorRes();
	GifRes&      GetGifRes();
#endif
    UIElement*  GetImageXmlElem(const wchar_t* szId);

	IImageResItem*  InsertImageItem(IMAGE_ITEM_TYPE eType, const wchar_t* szID, const wchar_t* szPath);
	bool  ModifyImageItem(const wchar_t* szID, const wchar_t* szPath);
	bool  RemoveImageItem(const wchar_t* szID);

	bool  ModifyImageItemInRunTime(const wchar_t* szID, const wchar_t* szPath);
	bool  ModifyImageItemAlpha(const wchar_t* szID, byte nAlphaPercent);

	void  Clear();
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

	static int  UIParseImageTagCallback(IUIElement*, IResource* pSkinRes);

private:
    // ISkinTagParse
    int  ParseNewElement(UIElement* pElem);
    void  OnNewChild(UIElement* pElem);

private:
	// ���ݳ־ò�
    IImageManager*  m_pIImageManager;

	typedef  UIList<ImageTagElementInfo>  _MyList;
	typedef  UIListItem<ImageTagElementInfo>  _MyListItem;
	_MyList  m_listUIElement;

	// ��������
	ImageRes        m_resImage;    // ����ͼƬ
#if defined(OS_WIN)
	CursorRes       m_resCursor;   // �����ʽͼƬ
	GifRes          m_resGif;      // ����
#endif
	Resource*        m_pSkinRes;
};

} 