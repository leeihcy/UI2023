#pragma once

//
//	鼠标样式
//
namespace  UI
{
class UICursor;
interface IUICursor;
interface ICursorResItem;
interface ICursorRes;

class CursorResItem
{
public:
	CursorResItem();
	~CursorResItem();

	void  SetId(LPCTSTR szId){ SETSTRING( m_strID, szId); }
	LPCTSTR  GetId(){ return m_strID.c_str(); }
	void  SetCurFilePath(LPCTSTR sz){ SETSTRING( m_strCurFilePath, sz); }
	LPCTSTR  GetCurFilePath(){ return m_strCurFilePath.c_str(); }

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
	void  GetCursor(LPCTSTR szCursorId, IUICursor** pp);
	LPCTSTR  GetCursorId(IUICursor* p);

public:
	CursorResItem* GetCursorItem( int nIndex );
	CursorResItem* GetCursorItem(UICursor* p);
	CursorResItem* GetCursorItem(LPCTSTR szId);
	int   GetCursorCount();
	CursorResItem*  LoadItem(IMapAttribute* pMapAttrib, LPCTSTR szFullPath);
	bool  InsertCursor(LPCTSTR strID, LPCTSTR strCurFilePath, CursorResItem** ppItem);
	bool  ModifyCursor(LPCTSTR strID, LPCTSTR strCursor );
	bool  RemoveCursor(LPCTSTR strID );
	void  Clear();

private:
	ICursorRes*  m_pICursorRes;
	vector<CursorResItem*>   m_vCursors;
};

}