#pragma once

//
//	鼠标样式
//
namespace  ui
{
class UICursor;
struct IUICursor;
struct ICursorResItem;
struct ICursorRes;

class CursorResItem
{
public:
	CursorResItem();
	~CursorResItem();

	void  SetId(const wchar_t* szId){ SETSTRING( m_strID, szId); }
	const wchar_t*  GetId(){ return m_strID.c_str(); }
	void  SetCurFilePath(const wchar_t* sz){ SETSTRING( m_strCurFilePath, sz); }
	const wchar_t*  GetCurFilePath(){ return m_strCurFilePath.c_str(); }

	IUICursor*  GetCursor();
	bool  ModifyCursor( const String& strCurFilePath );
	void  SetAttribute(IMapAttribute* p){}

	ICursorResItem*  GetICursorResItem();
public:
	String     m_strID;
	String     m_strCurFilePath;     // .cur文件路径
	UICursor*  m_pCursor;  
	ICursorResItem*  m_pICursorResItem;
};


class CursorRes
{
public:
	CursorRes();
	~CursorRes();

	ICursorRes*  GetICursorRes();
	void  GetCursor(const wchar_t* szCursorId, IUICursor** pp);
	const wchar_t*  GetCursorId(IUICursor* p);

public:
	CursorResItem* GetCursorItem( int nIndex );
	CursorResItem* GetCursorItem(UICursor* p);
	CursorResItem* GetCursorItem(const wchar_t* szId);
	int   GetCursorCount();
	CursorResItem*  LoadItem(IMapAttribute* pMapAttrib, const wchar_t* szFullPath);
	bool  InsertCursor(const wchar_t* strID, const wchar_t* strCurFilePath, CursorResItem** ppItem);
	bool  ModifyCursor(const wchar_t* strID, const wchar_t* strCursor );
	bool  RemoveCursor(const wchar_t* strID );
	void  Clear();

private:
	ICursorRes*  m_pICursorRes;
	std::vector<CursorResItem*>   m_vCursors;
};

}