#pragma once

namespace ui
{
struct IGifImage;
class GifImageBase;
class ResBundle;
struct IGifResItem;
struct IGifRes;
class Application;
class ImageData;

class GifResItem
{
public:
	GifResItem(Application* pUIApp);
	~GifResItem();

	String&  GetIdRef();
	void  SetId(const wchar_t* text);
	const wchar_t*  GetId() { return m_strId.c_str(); }

	void  SetPath(const wchar_t* path);

private:
	String      m_strId;      // image id
	String      m_strPath;    // image path
	bool        m_bUseSkinHLS;         // 该图片是否参与皮肤色调改变 
	ImageData*  m_pOriginImageData;    // 该图片的原始数据（改变图片色调时使用）

public:
	bool  ModifyGif(const String& strPath) { UIASSERT(0); return false; } // TODO:
	bool  ModifyHLS(short h, short l, short s, int nFlag){ UIASSERT(0); return false; } // TODO:
	void  SetAttribute(IMapAttribute* pMapAttrib);
	GifImageBase*  GetGifImage(ResBundle* pSkinRes);
	IGifResItem*  GetIGifResItem();

	DECLARE_bool_SETGET(UseSkinHLS);

public:
	IMapAttribute*  m_pMapAttrib;
	GifImageBase*   m_pGifImage;
	Application* m_pUIApp;
	IGifResItem*    m_pIGifResItem;
};

class GifRes
{
public:
	GifRes(ResBundle* p);
	~GifRes();

	IGifRes*  GetIGifRes();

	IGifImage* GetGifImage(const wchar_t* szId);

public:
	GifResItem*  LoadItem(IMapAttribute* pMapAttrib, const String& strFullPath);
	void SetUIApplication(Application* p)
	{ m_pUIApp = p; }

	int  GetGifCount(); 
	GifResItem* GetGifItem(int nIndex);
	GifResItem* GetGifItem(const wchar_t*  szId);

	bool InsertGif(const String& strID, const String& strPath, GifResItem** pRet);
	bool ModifyGif(const String& strID, const String& strPath);
	bool RemoveGif(const String& strID);
	bool Clear();
	bool ChangeSkinHLS(short h, short l, short s, int nFlag){UIASSERT(0); return false;};  // TODO:

private:
	IGifRes*  m_pIGifRes;
	std::vector<GifResItem*>   m_vGifs;
	Application*       m_pUIApp;
	ResBundle*  m_pSkinRes;
};


}