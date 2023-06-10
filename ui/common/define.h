#ifndef _UI_COMMON_DEFINE_H
#define _UI_COMMON_DEFINE_H
#include <string>


namespace UI
{
    
//
//	release版本下断言输出，可以利用debugview.exe看到断言错误
//  remark: __FILE__ ...这些都是char*类型的
//
#ifdef ASSERT
#undef ASSERT  // 避免使用ASSERT，请使用UIASSERT
#endif

#ifdef _DEBUG
#define UIASSERT(x)   \
	if(!(x))                          \
	{                                 \
		char szInfo[2048] = "";       \
		sprintf_s(szInfo, 2047, "UIASSERT expr(%s) %s(L%d) ## %s\r\n", #x, __FILE__, __LINE__, __FUNCTION__ );  \
		::OutputDebugStringA(szInfo); \
		DebugBreak(); \
	} 
#else
#define UIASSERT(x)    
#endif


	
// 常用缩写定义
#define SAFE_DELETE(p) \
	if (p) \
{ \
	delete (p); \
	(p) = NULL; \
}

#define SAFE_ARRAY_DELETE(p) \
	if (p) \
{ \
	delete[] (p); \
	(p) = NULL; \
}

#define SAFE_RELEASE(p) \
	if (p) \
{ \
	(p)->Release(); \
	(p) = NULL; \
}

#define SAFE_RELEASE2(p) \
	if (p) \
{ \
	(p).Release(); \
	(p) = NULL; \
}

#define SAFE_DELETE_GDIOBJECT(h) \
{\
	if (h)\
	{\
		/*BOOL b = */::DeleteObject(h);\
		/*if (0 == b) UIASSERT(b);*/ \
		(h) = NULL;\
	}\
}

#ifdef _UNICODE
	typedef std::wstring String;
#else
    typedef std::string  String;
#endif


inline int _round(const float& f)
{
	if (f < 0.0f)
		return int(f - 0.5f);
	else
		return int(f + 0.5f);
}
inline int _round(const double& f)
{
	if (f < 0.0f)
		return int(f - 0.5);
	else
		return int(f + 0.5);
}

}

#endif