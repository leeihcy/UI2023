#pragma once

// 模块内部公共头文件

#include "Inc/Util/struct.h"
#include "Inc/Util/color.h"
#include "Inc/Util/util.h"
#include "Inc/Util/iimage.h"
#include "Inc/Interface/iuieditor.h"
#include "Inc/Interface/iobjectdescription.h"
#include "Inc/Util/log.h"
#include "../common/define.h"

#define  ENABLE_HARDCOMPOSITION 1

#define DECLARE_bool_SETGET(xxx) \
	bool    Get##xxx() { return m_b##xxx; } \
	void    Set##xxx( bool b ) { m_b##xxx = b; }

#define DECLARE_byte_SETGET(xxx) \
	unsigned char  Get##xxx() { return m_byte##xxx; } \
	void    Set##xxx( byte b ) { m_byte##xxx = b; }

#define DECLARE_INT_SETGET(xxx) \
	int     Get##xxx() { return m_n##xxx; } \
	void    Set##xxx( int n ) { m_n##xxx = n; }

// String安全赋值
#define SETSTRING(str, sz) \
	if (sz){str = sz;} else {str.clear();}

namespace UI
{
    
//typedef unsigned char byte;

#ifndef uint
typedef unsigned int uint;
#endif

typedef CRect CRegion4;

typedef map<String, String>  ATTRMAP;


// 内部api
void  UI_Split(const String& str, TCHAR szSep, vector<String>& vRet);
LONG_PTR  UISendMessage(Message* pMsgTo, UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
interface IRenderFont;
void  UI_AttachFont(IRenderFont** pOut, HFONT hFont, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType);
LPTSTR  GetTempBuffer(int nMaxSize = 255);
String&  GetTempBufferString();

class Object;
interface IMKMgr : public IRootInterface
{
	virtual void  OnObjectVisibleChangeInd(Object* pObj, bool bVisible) = 0;
	virtual void  OnObjectRemoveInd(Object* pObj) = 0;
	virtual void  SetFocusObject(Object* pObj) = 0;
};

}