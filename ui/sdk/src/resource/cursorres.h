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

	void  SetId(const char* szId){ SETSTRING( m_strID, szId); }
	const char*  GetId(){ return m_strID.c_str(); }
	void  SetCurFilePath(const char* sz){ SETSTRING( m_strCurFilePath, sz); }
	const char*  GetCurFilePath(){ return m_strCurFilePath.c_str(); }

	IUICursor*  GetCursor();
	bool  ModifyCursor( const std::string& strCurFilePath );
	void  SetAttribute(IAttributeMap* p){}

	ICursorResItem*  GetICursorResItem();
public:
	std::string     m_strID;
	std::string     m_strCurFilePath;     // .cur文件路径
	UICursor*  m_pCursor;  
	ICursorResItem*  m_pICursorResItem;
};


class CursorRes
{
public:
	CursorRes();
	~CursorRes();

	ICursorRes*  GetICursorRes();
	void  GetCursor(const char* szCursorId, IUICursor** pp);
	const char*  GetCursorId(IUICursor* p);

public:
	CursorResItem* GetCursorItem( int nIndex );
	CursorResItem* GetCursorItem(UICursor* p);
	CursorResItem* GetCursorItem(const char* szId);
	int   GetCursorCount();
	CursorResItem*  LoadItem(IAttributeMap* attribute_map, const char* szFullPath);
	bool  InsertCursor(const char* strID, const char* strCurFilePath, CursorResItem** ppItem);
	bool  ModifyCursor(const char* strID, const char* strCursor );
	bool  RemoveCursor(const char* strID );
	void  Clear();

private:
	ICursorRes*  m_pICursorRes;
	std::vector<CursorResItem*>   m_vCursors;
};

}