#ifndef _UI_SDK_SRC_PRIVATE_INC_H_
#define _UI_SDK_SRC_PRIVATE_INC_H_

// 模块内部公共头文件

#include "include/util/color.h"
#include "include/util/util.h"
#include "include/macro/xmldefine.h"
#include "sdk/include/interface/iuiinterface.h"
#include "include/interface/iuieditor.h"
#include "include/interface/imeta.h"
#include "include/util/log.h"
#include "src/util/util.h"

#include <map>
#include <vector>
#include <string>

#define  ENABLE_HARDCOMPOSITION 1

#define DECLARE_bool_SETGET(xxx) \
	bool    Get##xxx() { return m_b##xxx; } \
	void    Set##xxx( bool b ) { m_b##xxx = b; }

#define DECLARE_byte_SETGET(xxx) \
	unsigned char  Get##xxx() { return m_byte##xxx; } \
	void    Set##xxx( byte b ) { m_byte##xxx = b; }

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

typedef std::map<std::string, std::string>  ATTRMAP;


// 内部api
class Message;
void UI_SplitW(const std::wstring& str, wchar_t szSep, std::vector<std::wstring>& vRet);
void UI_Split(const std::string &str, char szSep, std::vector<std::string> &vRet);
struct IRenderFont;
// void  UI_AttachFont(IRenderFont** pOut, HFONT hFont, eGraphicsLibraryType eRenderType);
char*  GetTempBuffer(int nMaxSize = 255);
wchar_t*  GetTempBufferW(int nMaxSize = 255);
std::string&  GetTempBufferString();
std::wstring&  GetTempBufferStringW();

class Object;
struct IMKMgr : public IRootInterface
{
	virtual void  OnObjectVisibleChangeInd(Object* pObj, bool bVisible) = 0;
	virtual void  OnObjectRemoveInd(Object* pObj) = 0;
	virtual void  SetFocusObject(Object* pObj) = 0;
};

}

#endif