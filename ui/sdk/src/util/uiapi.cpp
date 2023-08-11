#include "include/inc.h"
#include "include/uiapi.h"
#include "src/object/object.h"
#include "src/application/uiapplication.h"
// #include "src/Renderlibrary\gdi\gdifont.h"
// #include "src/ATL\image.h"
#include "include/util/util.h"
#include "include/interface/iuiapplication.h"
// #include "..\Layer\layer.h"

// 内部全局方法
namespace ui
{
 
bool  CreateUIApplication(IApplication** pp)
{
    if (!pp)
        return false;

    IApplication* p = new IApplication;
    p->GetImpl()->x_Init();

    *pp = p;
    return true;
}

/*
void  UI_AttachFont(IRenderFont** pOut, HFONT hFont, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType )
{
    if( nullptr == hFont || nullptr == pOut)
        return;

    switch (eRenderType)
    {
    case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
        {
            GDIRenderFont::CreateInstance(pOut);
            IRenderFont* pRenderFont = (IRenderFont*)*pOut;
            pRenderFont->Attach(hFont);
        }
        break;

    case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
        {
            GdiplusRenderFont::CreateInstance(pOut);
            IRenderFont* pRenderFont = (IRenderFont*)*pOut;
            pRenderFont->Attach(hFont);
        }
        break;

#ifdef UI_D2D_RENDER
    case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
        {
            Direct2DRenderFont::CreateInstance(pOut);
            IRenderFont* pRenderFont = (IRenderFont*)*pOut;
            pRenderFont->Attach(hFont);
        }
        break;
#endif

    default: 
        return ;
    }

    return ;
}
*/
void  UI_ExtendPath(String& strPath)
{
#if defined(OS_WIN)
    if (!util::IsFullPath(strPath.c_str()))
    {
        wchar szModulePath[MAX_PATH] = _T("");

        GetModuleFileName(/*Application::GetModuleInstance()*/g_hInstance, szModulePath, MAX_PATH);
        wchar* szTemp = _tcsrchr(szModulePath, _T('\\'));
        if (szTemp)
            *(szTemp+1) = 0;

        String strTemp = szModulePath;
        strPath = strTemp;
        strPath.append(strPath);
    }
#else
    UIASSERT(false);
#endif
}


void  UI_AttachFont(IRenderFont** pOut, HFONT hFont, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType)
{
#if defined(OS_WIN)
	if (nullptr == hFont || nullptr == pOut)
		return;

	switch (eRenderType)
	{
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
	{
		GDIRenderFont::CreateInstance(pOut);
		IRenderFont* pRenderFont = (IRenderFont*)*pOut;
		pRenderFont->Attach(hFont);
	}
	break;

	default:
		return;
	}

	return;
#endif
}

//
// 字符串拆分
//
//	Remark:
//		在向DLL之间使用stl作为参数进行传递，会导致内存释放时的崩溃，因此这里没有直接去调用UI_Split方法
//		而是直接在ULDLL中重写这么一个函数
//
void  UI_Split(const String& str, wchar_t szSep, std::vector<String>& vRet)
{
	if (str.empty())
		return;

    int nIndex = 0;
    while (true)
    {
        int nResult = (int)str.find( szSep, nIndex );
        if (-1 == nResult)
        {
            vRet.push_back( str.substr( nIndex, str.length()-nIndex ) );
            break;
        }
        else
        {
            vRet.push_back( str.substr( nIndex, nResult-nIndex ) );
            nIndex = ++nResult;
        }
    }
}

// 用于支持Get时返回一个LPCTSTR临时变量
#define GlobalTempBufferSize  256
wchar   g_szGlobalTempBuffer[GlobalTempBufferSize];
String  g_strGlobalTempBuffer;

wchar*  GetTempBuffer(int nMaxSize)
{
	UIASSERT(nMaxSize < GlobalTempBufferSize);
	memset(g_szGlobalTempBuffer, 0, sizeof(g_szGlobalTempBuffer));
	return g_szGlobalTempBuffer;
}
String&  GetTempBufferString()
{
	g_strGlobalTempBuffer.clear();
	return g_strGlobalTempBuffer;
}


HBITMAP CreateMemBitmap(int nWidth, int nHeight, int* pnPitch, byte** ppBits)
{
#if defined(OS_WIN)
    Image image;
    image.Create(nWidth, nHeight, 32, Image::createAlphaChannel);

    if (ppBits)
        *ppBits = (BYTE*)image.GetBits();

    if (pnPitch)
        *pnPitch = image.GetPitch();
    
    return image.Detach();
#else
    UIASSERT(0);
    return 0;
#endif
}

} // namespace
