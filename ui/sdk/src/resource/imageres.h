#pragma once
#include "src/private_inc.h"

namespace ui
{
	struct IImageResItem;
	struct IRenderBitmap;
	struct IImageRes;
class ImageData;
class GDIRenderBitmap;
class GdiplusRenderBitmap;
class Resource;

//
//	image�е�һ����Ϣ����
//
class ImageResItem
{
public:
	ImageResItem();
	~ImageResItem();

	bool  ModifyImage(const wchar_t*  szPath);
    IMAGE_ITEM_TYPE  GetImageItemType();
    void  SetImageItemType(IMAGE_ITEM_TYPE e);

	const wchar_t*  GetId();
	void  SetId(const wchar_t* text);
	void  SetPath(const wchar_t* text);
	const wchar_t*  GetPath();

	DECLARE_bool_SETGET(UseSkinHLS);
	DECLARE_bool_SETGET(NeedAntiAliasing)

    bool  NeedDpiAdapt();
	void  SetFileDpiScale(int n){
			m_nFileDpiScale = (byte)n;
	}

public:
	bool  ModifyHLS(short h, short l, short s, int nFlag);
	bool  ModifyHLS(IRenderBitmap* pBitmap, short h, short l, short s, int nFlag);
	bool  ModifyAlpha(byte nAlphaPercent);
	IRenderBitmap*  GetImage(Resource* pSkinRes, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI, bool* pbFirstTimeCreate=nullptr);
    IRenderBitmap*  GetImage_gdi(Resource* pSkinRes, bool* pbFirstTimeCreate=nullptr);

	bool  IsMyRenderBitmap(IRenderBitmap* pRenderBitmap);

    // ����Ixxx�ӿ�
    virtual IImageResItem*  GetIImageResItem(); 
    // ��ȡxml���ԣ���imagelist��layout��count
	virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    // �ڴ�����RenderBitmap֮���ɸ�������ȥΪ��������չ����
    virtual void  SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap);

protected:
	IImageResItem*  m_pIImageResItem;

    String    m_strId;      // image id
    String    m_strPath;    // image path
	byte   m_nFileDpiScale;  // ͼƬdpi�Ŵ�ϵ����@2x @3x ...

    bool  m_bUseSkinHLS;                   // ��ͼƬ�Ƿ����Ƥ��ɫ���ı� 
    bool  m_bNeedAntiAliasing;             // ��Ҫ֧�ֿ����(gdi��alphablend���Ų�֧��SetStretchBltMode��HALFTONE)
    bool  m_bMustHasAlphaChannel;          // ��ͼƬ�Ƿ���Ҫ����alpha channel��1. ��alpha channel��ͼƬһ�ʲ���gdi����������alphablend���ơ�2. û��alpha channel��gdiͼƬ���޷��ڷֲ㴰���������
    bool  m_bDpiAdapt;                     // �Ƿ�����ӦDPI����

    IMapAttribute*        m_pMapAttrib;    // Ϊ��ͼƬ���õ����ԣ�����imagelist��count��icon��width height
    IMAGE_ITEM_TYPE       m_eType;         // ͼƬ����

    ImageData*            m_pOriginImageData;    // ��ͼƬ��ԭʼ���ݣ��ı�ͼƬɫ��ʱʹ�ã�
#if defined(OS_WIN)
    GDIRenderBitmap*      m_pGdiBitmap;          // �ⲿ���ã��ó�Ա���������ü���
#endif
    //  ע��Ϊ����߻�ͼЧ�ʣ�ͬʱҲ����һ��ͼƬ���ض�����ʽ������ʹ��GDI(AlphaBlend)����ͼ��
    //	    ��Ҫע����ǣ������Ҫʹ��alphaͨ��ʱ������ǿ�ƴ���һ��32λ��GDI BITMAP
    // GdiplusRenderBitmap*  m_pGdiplusBitmap;      // �ⲿ����
};

class ImageListResItem : public ImageResItem
{
public:
    ImageListResItem();

    virtual IImageResItem*  GetIImageResItem(); 
    virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    virtual void  SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap);

    IMAGELIST_LAYOUT_TYPE  GetLayoutType();
    int  GetItemCount();
    void  SetLayoutType(IMAGELIST_LAYOUT_TYPE);
    void  SetItemCount(int);

private:
    IMAGELIST_LAYOUT_TYPE   m_eLayoutType;
    int  m_nCount;
};

class ImageIconResItem : public ImageResItem
{
public:
    ImageIconResItem();

    virtual IImageResItem*  GetIImageResItem(); 
    virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    virtual void  SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap);

private:
    Size  m_sizeDraw;
};

//
//	image�б�
//
class ImageRes
{
public:
	ImageRes(Resource*  pSkinRes);
	~ImageRes();

	IImageRes&  GetIImageRes();

	long  GetImageCount();
	IImageResItem*  GetImageResItem(long lIndex);
	IImageResItem*  GetImageResItem(const wchar_t* szID);
	bool ModifyImage(const wchar_t* szId, const wchar_t* szPath);

	HBITMAP  LoadBitmap(const wchar_t* szId);
	bool  GetBitmap(const wchar_t* szImageID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, 
        /*__out*/ IRenderBitmap** pRenderBitmap);

	const wchar_t*  GetRenderBitmapId(IRenderBitmap* pBitmap);

public:
	ImageResItem*  LoadItem(const wchar_t* szType, IMapAttribute* pMapAttrib, const wchar_t* szFullPath);

	ImageResItem*  GetImageItem2(int nIndex);
	ImageResItem*  GetImageItem2(const wchar_t* szId);

	ImageResItem*  InsertImage(IMAGE_ITEM_TYPE eType, const wchar_t* szId, const wchar_t* szPath);
	bool RemoveImage(const wchar_t* szId);
    bool RemoveImage(IImageResItem* pItem);
	bool Clear();
	bool ChangeSkinHLS(short h, short l, short s, int nFlag);
	bool ModifyImageItemAlpha(const String& strID, byte nAlphaPercent);

private: 
	IImageRes*  m_pIImageRes;
	Resource*  m_pSkinRes;

    typedef  std::map<String,  ImageResItem*>::iterator  _MyIter;
    std::map<std::wstring,  ImageResItem*>   m_mapImages;
    //vector<ImageResItem*>   m_vImages;   // vector�Ĳ���Ч��̫���ˣ���˻���map
};

}