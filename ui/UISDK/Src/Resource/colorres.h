#pragma once
#include "Inc/Util/color.h"

namespace UI
{
interface IColorResItem;
interface IColorRes;
class SkinRes;

//
//	color中的一项信息数据
//
class  ColorResItem
{
public:
	ColorResItem();
	~ColorResItem();

    IColorResItem*  GetIColorResItem();
	LPCTSTR GetColorString();
	bool  IsMyColor(Color* p);

private:
	String    m_strId;
	String    m_strColor;     // 颜色字符串
    ColorPtr  m_pColor;

	COLORREF*  m_pOriginColorValue;  // 该COLOR未改变色调前的颜色值，用于还原

	// TODO: 可以再进行扩展。例如文字的黑白颜色变换，是根据文字后面的背景色来决定的，而不是配死的
	bool      m_bUseSkinHLS;  // 该COLOR是否参与皮肤色调改变 

public:
	void  SetId(LPCTSTR ID);
	LPCTSTR  GetId();
	const String&   GetIdRef();
	void  SetUseSkinHLS(bool b);
	bool  GetUseSkinHLS();
	const String&   GetColorStringRef();
	bool  GetColor(ColorPtr* pColor, bool *bFirstTimeCreate = nullptr);
	void  SetColor(const String& strColor); 

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
	ColorRes(SkinRes* p);
	~ColorRes();

    IColorRes&  GetIColorRes();

	long  GetColorCount();
	IColorResItem*  GetColorResItem(long lIndex);
	void  GetColor(LPCTSTR szColorId, Color** pp);
	LPCTSTR  GetColorId(Color* p);

public:
	ColorResItem* GetColorItem( int nIndex );
	ColorResItem* GetColorItem( const String& strID );
	bool  InsertColor( const String& strID, LPCTSTR szColor, ColorResItem** pItem );
	bool  ModifyColor( const String& strID, const String& strColor );
	bool  RemoveColor( const String& strID );
	void  Clear();

	bool  LoadItem(IMapAttribute* pMapAttrib, LPCTSTR szData);
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

private:
	SkinRes*  m_pSkinRes;
    IColorRes*  m_pIColorRes;
    vector<ColorResItem*>   m_vColors;
};

}