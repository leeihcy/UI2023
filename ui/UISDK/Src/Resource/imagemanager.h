#pragma once
#include "imageres.h"
#include "cursorres.h"
#include "gifres.h"
#include "Src\Util\DataStruct\list.h"

namespace UI
{
	interface UIElement;
	interface IUIElement;
	interface IImageManager;

class  ImageTagElementInfo
{
public:
	String  strId;  // 模块名
	UIElement*  pXmlElement;  // xml结点
};

class ImageManager
{
public:
	ImageManager(SkinRes* pSkinRes);
	~ImageManager(void);

    IImageManager&  GetIImageManager();

	ImageRes&    GetImageRes();
	CursorRes&   GetCursorRes();
	GifRes&      GetGifRes();
    UIElement*  GetImageXmlElem(LPCTSTR szId);

	IImageResItem*  InsertImageItem(IMAGE_ITEM_TYPE eType, LPCTSTR szID, LPCTSTR szPath);
	bool  ModifyImageItem(LPCTSTR szID, LPCTSTR szPath);
	bool  RemoveImageItem(LPCTSTR szID);

	bool  ModifyImageItemInRunTime(LPCTSTR szID, LPCTSTR szPath);
	bool  ModifyImageItemAlpha(LPCTSTR szID, byte nAlphaPercent);

	void  Clear();
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

	static HRESULT  UIParseImageTagCallback(IUIElement*, ISkinRes* pSkinRes);

private:
    // ISkinTagParse
    HRESULT  ParseNewElement(UIElement* pElem);
    void  OnNewChild(UIElement* pElem);

private:
	// 数据持久层
    IImageManager*  m_pIImageManager;

	typedef  UIList<ImageTagElementInfo>  _MyList;
	typedef  UIListItem<ImageTagElementInfo>  _MyListItem;
	_MyList  m_listUIElement;

	// 对象属性
	ImageRes        m_resImage;    // 界面图片
	CursorRes       m_resCursor;   // 鼠标样式图片
	GifRes          m_resGif;      // 动画

	SkinRes*        m_pSkinRes;
};

} 