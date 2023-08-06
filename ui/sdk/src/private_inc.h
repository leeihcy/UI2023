#pragma once

// 模块内部公共头文件

#include "include/util/struct.h"
#include "include/util/color.h"
#include "include/util/util.h"
#include "include/macro/xmldefine.h"
// #include "include/util/iimage.h"
#include "include/interface/iuieditor.h"
#include "include/interface/iobjectdescription.h"
#include "include/util/log.h"
// #include "../common/define.h"

#include <map>
#include <vector>

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

namespace ui
{
    
//typedef unsigned char byte;

#ifndef uint
typedef unsigned int uint;
#endif

// typedef Rect Rect;

typedef std::map<std::wstring, std::wstring>  ATTRMAP;


// 内部api
class Message;
void  UI_Split(const std::wstring& str, wchar_t szSep, std::vector<std::wstring>& vRet);
long  UISendMessage(Message* pMsgTo, unsigned int message, long wParam = 0, long lParam = 0);
struct IRenderFont;
void  UI_AttachFont(IRenderFont** pOut, HFONT hFont, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType);
wchar_t*  GetTempBuffer(int nMaxSize = 255);
std::wstring&  GetTempBufferString();

class Object;
struct IMKMgr : public IRootInterface
{
	virtual void  OnObjectVisibleChangeInd(Object* pObj, bool bVisible) = 0;
	virtual void  OnObjectRemoveInd(Object* pObj) = 0;
	virtual void  SetFocusObject(Object* pObj) = 0;
};

}