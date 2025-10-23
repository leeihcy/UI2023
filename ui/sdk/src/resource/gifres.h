#pragma once

namespace ui
{
struct IGifImage;
class GifImageBase;
class ResourceBundle;
struct IGifResItem;
struct IGifRes;
class Application;
class ImageData;

class GifResItem
{
public:
	GifResItem(Application* pUIApp);
	~GifResItem();

	std::string&  GetIdRef();
	void  SetId(const char* text);
	const char*  GetId() { return m_strId.c_str(); }

	void  SetPath(const char* path);

private:
	std::string      m_strId;      // image id 
	std::string      m_strPath;    // image path
	bool        m_bUseSkinHLS;         // 该图片是否参与皮肤色调改变 
	ImageData*  m_pOriginImageData;    // 该图片的原始数据（改变图片色调时使用）

public:
	bool  ModifyGif(const std::string& strPath) { UIASSERT(0); return false; } // TODO:
	bool  ModifyHLS(short h, short l, short s, int nFlag){ UIASSERT(0); return false; } // TODO:
	void  SetAttribute(IAttributeMap* attribute_map);
	GifImageBase*  GetGifImage(ResourceBundle* resource_bundle);
	IGifResItem*  GetIGifResItem();

	DECLARE_bool_SETGET(UseSkinHLS);

public:
	IAttributeMap*  m_pMapAttrib;
	GifImageBase*   m_pGifImage;
	Application* m_pUIApp;
	IGifResItem*    m_pIGifResItem;
};

class GifRes
{
public:
	GifRes(ResourceBundle* p);
	~GifRes();

	IGifRes*  GetIGifRes();

	IGifImage* GetGifImage(const wchar_t* szId);

public:
	GifResItem*  LoadItem(IAttributeMap* attribute_map, const std::string& strFullPath);
	void SetUIApplication(Application* p)
	{ m_pUIApp = p; }

	int  GetGifCount(); 
	GifResItem* GetGifItem(int nIndex);
	GifResItem* GetGifItem(const wchar_t*  szId);

	bool InsertGif(const std::string& strID, const std::string& strPath, GifResItem** pRet);
	bool ModifyGif(const std::string& strID, const std::string& strPath);
	bool RemoveGif(const std::string& strID);
	bool Clear();
	bool ChangeSkinHLS(short h, short l, short s, int nFlag){UIASSERT(0); return false;};  // TODO:

private:
	IGifRes*  m_pIGifRes;
	std::vector<GifResItem*>   m_vGifs;
	Application*       m_pUIApp;
	ResourceBundle*  m_resource_bundle;
};


}