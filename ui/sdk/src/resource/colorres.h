#pragma once
#include <vector>
#include "include/inc.h"
#include "include/util/color.h"

namespace ui
{
struct IColorResItem;
struct IColorRes;
class Resource;

//
//	color中的一项信息数据
//
class  ColorResItem
{
public:
	ColorResItem();
	~ColorResItem();

    IColorResItem*  GetIColorResItem();
	const wchar_t* GetColorString();
	bool  IsMyColor(Color* p);

private:
	std::wstring    m_strId;
	std::wstring    m_strColor;     // 颜色字符串
    ColorPtr  m_pColor;

	COLORREF*  m_pOriginColorValue;  // 该COLOR未改变色调前的颜色值，用于还原

	// TODO: 可以再进行扩展。例如文字的黑白颜色变换，是根据文字后面的背景色来决定的，而不是配死的
	bool      m_bUseSkinHLS;  // 该COLOR是否参与皮肤色调改变 

public:
	void  SetId(const wchar_t* ID);
	const wchar_t*  GetId();
	const std::wstring&   GetIdRef();
	void  SetUseSkinHLS(bool b);
	bool  GetUseSkinHLS();
	const std::wstring&   GetColorStringRef();
	bool  GetColor(ColorPtr* pColor, bool *bFirstTimeCreate = nullptr);
	void  SetColor(const std::wstring& strColor); 

	void  SetAttribute(IMapAttribute* p);
	bool  ModifyHLS(short h, short l, short s, int nFlag);

private:
    IColorResItem*  m_pIColorResItem;
};

//
//	color列表
//
class ColorRes
{
public:
	ColorRes(Resource* p);
	~ColorRes();

    IColorRes&  GetIColorRes();

	long  GetColorCount();
	IColorResItem*  GetColorResItem(long lIndex);
	void  GetColor(const wchar_t* szColorId, Color** pp);
	const wchar_t*  GetColorId(Color* p);

public:
	ColorResItem* GetColorItem( int nIndex );
	ColorResItem* GetColorItem( const std::wstring& strID );
	bool  InsertColor( const std::wstring& strID, const wchar_t* szColor, ColorResItem** pItem );
	bool  ModifyColor( const std::wstring& strID, const std::wstring& strColor );
	bool  RemoveColor( const std::wstring& strID );
	void  Clear();

	bool  LoadItem(IMapAttribute* pMapAttrib, const wchar_t* szData);
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

private:
	Resource*  m_pSkinRes;
    IColorRes*  m_pIColorRes;
    std::vector<ColorResItem*> m_vColors;
};

}