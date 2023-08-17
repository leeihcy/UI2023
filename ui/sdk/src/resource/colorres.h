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
//	color�е�һ����Ϣ����
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
	std::wstring    m_strColor;     // ��ɫ�ַ���
    ColorPtr  m_pColor;

	COLORREF*  m_pOriginColorValue;  // ��COLORδ�ı�ɫ��ǰ����ɫֵ�����ڻ�ԭ

	// TODO: �����ٽ�����չ���������ֵĺڰ���ɫ�任���Ǹ������ֺ���ı���ɫ�������ģ�������������
	bool      m_bUseSkinHLS;  // ��COLOR�Ƿ����Ƥ��ɫ���ı� 

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
//	color�б�
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