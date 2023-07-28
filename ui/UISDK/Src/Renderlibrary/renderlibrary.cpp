#include "stdafx.h"

#include "include/interface/iwindow.h"
#include "src/Renderlibrary\gdi\gdibitmap.h"
#include "src/Renderlibrary\gdi\gdirender.h"
#include "src/Base\Object\object.h"

void RenderBitmapFactory::CreateInstance(IApplication* pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut)
{
	if (nullptr == ppOut)
		return;

	switch (eGraphicsRenderType)
	{	
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		{
			if (eType == IMAGE_ITEM_TYPE_ICON)
			{
				GDIIconRenderBitmap::CreateInstance(ppOut);
			}
			else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST)
			{
				GDIImageListRenderBitmap::CreateInstance(ppOut);
			}
// 			else if (eType == IMAGE_ITEM_TYPE_GIF)
// 			{
// 				GDIGifRenderBitmap::CreateInstance(ppOut);
// 			}
// 			else if (eType == IMAGE_ITEM_TYPE_PNGLISTGIF)
// 			{
// 				GDIGifRenderBitmap::CreateInstance(ppOut);
// 			}
			else 
			{
				GDIRenderBitmap::CreateInstance(ppOut);
			}
		}
		break;
#if 0
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
		{
			if (eType == IMAGE_ITEM_TYPE_ICON)
			{
				GdiplusIconRenderBitmap::CreateInstance(ppOut);
			}
			else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST)
			{
				GdiplusImageListRenderBitmap::CreateInstance(ppOut);
			}
			else 
			{
				GdiplusRenderBitmap::CreateInstance(ppOut);
			}
		}
		break;
	case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
		{
            if (!pUIApp)
                return;

            HMODULE  hModule = pUIApp->GetUID2DModule();
            if (!hModule)
                return;

            funcUI3D_CreateD2DRenderBitmap func = (funcUI3D_CreateD2DRenderBitmap)GetProcAddress(hModule, NAME_UI3D_CreateD2DRenderBitmap);
            if (!func)
            {
                UIASSERT(0);
                return;
            }

            func(eType, ppOut);
		}
		break;

    case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D:
        {
            if (!pUIApp)
                return;

            HMODULE  hModule = pUIApp->GetUID3DModule();
            if (!hModule)
                return;

            funcUI3D_CreateD3DRenderBitmap func = (funcUI3D_CreateD3DRenderBitmap)GetProcAddress(hModule, NAME_UI3D_CreateD3DRenderBitmap);
            if (!func)
            {
                UIASSERT(0);
                return;
            }

            func(eType, ppOut);
        }
        break;
#endif
    default:
        UIASSERT(0);
        break;
	}
}
 

//////////////////////////////////////////////////////////////////////////
namespace ui
{

// IRenderTarget* 没有引用计数机制
IRenderTarget*  UICreateRenderTarget(IApplication* pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eType, bool bNeedAlphaChannel)
{
    IRenderTarget*  pRenderTarget = nullptr;

    switch (eType)
    {
    case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
        {
            pRenderTarget = new GdiRenderTarget(bNeedAlphaChannel);
        }
        break;

#if 0
    case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
        {
            pRenderTarget = new GdiplusRenderTarget();
        }
        break;

    case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
        {
            if (!pUIApp)
            {
                UI_LOG_WARN(_T("%s CreateD2D RenderTarget Failed. pUIApp is nullptr"), FUNC_NAME);
                return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, bNeedAlphaChannel);
            }
            HMODULE  hModule = pUIApp->GetUID2DModule();
            if (!hModule)
            {
                UI_LOG_WARN(_T("%s CreateD2D RenderTarget Failed. GetUI3DModule Failed."), FUNC_NAME);
                return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, bNeedAlphaChannel);
            }
         
            funcUI3D_CreateD2DRenderTarget func = (funcUI3D_CreateD2DRenderTarget)GetProcAddress(hModule, NAME_UI3D_CreateD2DRenderTarget);
            if (!func)
            {
                UIASSERT(0);
                return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, bNeedAlphaChannel);
            }
            func(&pRenderTarget);
        }
        break;
	case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D:
		{
			if (!pUIApp)
			{
				UI_LOG_WARN(_T("%s CreateD3D RenderTarget Failed. pUIApp is nullptr"), FUNC_NAME);
				return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, bNeedAlphaChannel);
			}
			HMODULE  hModule = pUIApp->GetUID3DModule();
			if (!hModule)
			{
				UI_LOG_WARN(_T("%s CreateD3D RenderTarget Failed. GetUI3DModule Failed."), FUNC_NAME);
				return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, bNeedAlphaChannel);
			}

			funcUI3D_CreateD3DRenderTarget func = (funcUI3D_CreateD3DRenderTarget)GetProcAddress(hModule, NAME_UI3D_CreateD3DRenderTarget);
			if (!func)
			{
				UIASSERT(0);
				return UICreateRenderTarget(pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, bNeedAlphaChannel);
			}
			func(&pRenderTarget);
		}
		break;
#endif

    default:
        UIASSERT(0);
        break;
    }
    return pRenderTarget;
}

void  UICreateRenderBitmap(
        IApplication*  pUIApp, 
        GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, 
        IMAGE_ITEM_TYPE eType, 
        IRenderBitmap** ppOut)
{
    RenderBitmapFactory::CreateInstance(pUIApp, eGraphicsRenderType, eType, ppOut);
}

GRAPHICS_RENDER_LIBRARY_TYPE  UIParseGraphicsRenderLibraryType(const TCHAR* szText)
{
	if (szText)
	{
		if (0 == _tcscmp(szText, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDIPLUS))
		{
			return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
		}
		else if (0 == _tcscmp(szText, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D2D) ||
			0 == _tcscmp(szText, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT2D))
		{
			return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D;
		}
		else if (0 == _tcscmp(szText, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D3D) ||
			0 == _tcscmp(szText, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT3D))
		{
			return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D;
		}
		else
		{
			return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		}
	}

	return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
}

// void  EnumGraphicsRenderLibraryType(
// 		pfnEnumGraphicsRenderLibraryTypeCallback callback, 
// 		WPARAM w, LPARAM l)
// {
// 	callback(GRAPHICS_RENDER_LIBRARY_TYPE_GDI,      XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDI, w, l);
// 	callback(GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS,  XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDIPLUS, w, l);
// 	callback(GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D2D, w, l);
// 	callback(GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT2D, w, l);
// 	callback(GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D3D, w, l);
// 	callback(GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT3D, XML_WINDOW_GRAPHICS_RENDER_LIBRARY_DIRECT3D, w, l);
// }


}