#pragma once

namespace UI
{
interface IGifImage;
class GifImageBase;
class SkinRes;
interface IGifResItem;
interface IGifRes;
class UIApplication;
class ImageData;

class GifResItem
{
public:
	GifResItem(UIApplication* pUIApp);
	~GifResItem();

	String&  GetIdRef();
	void  SetId(LPCTSTR text);
	LPCTSTR  GetId() { return m_strId.c_str(); }

	void  SetPath(LPCTSTR path);

private:
	String      m_strId;      // image id
	String      m_strPath;    // image path
	bool        m_bUseSkinHLS;         // 该图片是否参与皮肤色调改变 
	ImageData*  m_pOriginImageData;    // 该图片的原始数据（改变图片色调时使用）

public:
	bool  ModifyGif(const String& strPath) { UIASSERT(0); return false; } // TODO:
	bool  ModifyHLS(short h, short l, short s, int nFlag){ UIASSERT(0); return false; } // TODO:
	void  SetAttribute(IMapAttribute* pMapAttrib);
	GifImageBase*  GetGifImage(SkinRes* pSkinRes);
	IGifResItem*  GetIGifResItem();

	DECLARE_bool_SETGET(UseSkinHLS);

public:
	IMapAttribute*  m_pMapAttrib;
	GifImageBase*   m_pGifImage;
	UIApplication* m_pUIApp;
	IGifResItem*    m_pIGifResItem;
};

class GifRes
{
public:
	GifRes(SkinRes* p);
	~GifRes();

	IGifRes*  GetIGifRes();

	IGifImage* GetGifImage(LPCTSTR szId);

public:
	GifResItem*  LoadItem(IMapAttribute* pMapAttrib, const String& strFullPath);
	void SetUIApplication(UIApplication* p)
	{ m_pUIApp = p; }

	int  GetGifCount(); 
	GifResItem* GetGifItem(int nIndex);
	GifResItem* GetGifItem(const TCHAR*  szId);

	bool InsertGif(const String& strID, const String& strPath, GifResItem** pRet);
	bool ModifyGif(const String& strID, const String& strPath);
	bool RemoveGif(const String& strID);
	bool Clear();
	bool ChangeSkinHLS(short h, short l, short s, int nFlag){UIASSERT(0); return false;};  // TODO:

private:
	IGifRes*  m_pIGifRes;
	vector<GifResItem*>   m_vGifs;
	UIApplication*       m_pUIApp;
	SkinRes*  m_pSkinRes;
};


}