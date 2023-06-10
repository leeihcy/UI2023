#include "stdafx.h"
#include "roundcorner.h"

namespace UI
{

// hBitmap目前只做了 topdown 类型
void  RoundCorner_WindowBitmap(HBITMAP hBitmap, RECT* prc, REGION4* pRadius)
{
    if (!hBitmap || !pRadius)
        return;

    DIBSECTION  dib = {0};
    if (sizeof(dib) != GetObject(hBitmap, sizeof(dib), &dib))
        return;
   
    BITMAP& bm = dib.dsBm;
    byte* pBits = (byte*)bm.bmBits;
    int   nPitch = bm.bmWidthBytes;

    RECT  rect;
    if (prc)
    {
        RECT  rcBitmap = {0, 0, bm.bmWidth, bm.bmHeight};
        IntersectRect(&rect, &rcBitmap, prc);
    }
    else
    {
        ::SetRect(&rect, 0, 0, bm.bmWidth, bm.bmHeight);
    }

    int l = rect.left;
    int t = rect.top;
    int r = rect.right-1;
    int b = rect.bottom-1;

#define GETPIXEL(x, y) \
    (LPDWORD)(pBits + y*nPitch + (x<<2))
#define THROWPIXEL(x, y) \
    *GETPIXEL(x, y) = 0
    

    // 左上角
    switch (pRadius->left) 
    {
    case 0:
        break;
    case 1:
        {
            // {0, 0}
            THROWPIXEL(l, t);
        }
        break;

    case 2:
        {
            // {0,0}, {1,0}, 
            // {0,1}

            POINT pt[] = 
            { 
                {l,t  },  {l+1,t}, 
                {l,t+1} 
            };
            int count = sizeof(pt)/sizeof(POINT);

            for (int i = 0; i < count; i++)
                THROWPIXEL(pt[i].x, pt[i].y);
        }
        break;

    case 3:
    default:
        {
            // {0,0}, {1,0}, {2,0}, {3,0}
            // {0,1}, {1,1},
            // {0,2},
            // {0,3}
            POINT pt[] = 
            {
                {l,t  }, {l+1,t  }, {l+2,t}, {l+3,t},
                {l,t+1}, {l+1,t+1},
                {l,t+2},
                {l,t+3}
            };
            int count = sizeof(pt)/sizeof(POINT);

            for (int i = 0; i < count; i++)
                THROWPIXEL(pt[i].x, pt[i].y);
        };
        break;
    }
    
    // 右上角
    switch (pRadius->top)
    {
    case 0:
        break;
    case 1:
        {
            THROWPIXEL(r, t);
        }
        break;

    case 2:
        {
            POINT pt[] = 
            {
                {r-1,t}, {r,t}, 
                       {r,t+1}
            };
            int count = sizeof(pt)/sizeof(POINT);

            for (int i = 0; i < count; i++)
                THROWPIXEL(pt[i].x, pt[i].y);
        }
        break;

    case 3:
    default:
        {
            POINT pt[] = 
            {
                {r-3,t}, {r-2,t}, {r-1,t}, {r,t},
                              {r-1,t+1}, {r,t+1},
                                         {r,t+2},
                                         {r,t+3}
            };
            int count = sizeof(pt)/sizeof(POINT);

            for (int i = 0; i < count; i++)
                THROWPIXEL(pt[i].x, pt[i].y);
        };
        break;
    }

    // 右下角
    switch (pRadius->right)
    {
    case 0:
        break;
    case 1:
        {
            THROWPIXEL(r, b);
        }
        break;

    case 2:
        {
            POINT pt[] = 
            {
                       {r,b-1},
                {r-1,b}, {r,b}
            };
            int count = sizeof(pt)/sizeof(POINT);

            for (int i = 0; i < count; i++)
                THROWPIXEL(pt[i].x, pt[i].y);
        }
        break;

    case 3:
    default:
        {
            POINT pt[] = 
            {
                                         {r,b-3},
                                         {r,b-2},
                              {r-1,b-1}, {r,b-1},
                {r-3,b}, {r-2,b}, {r-1,b}, {r,b}
            };
            int count = sizeof(pt)/sizeof(POINT);

            for (int i = 0; i < count; i++)
                THROWPIXEL(pt[i].x, pt[i].y);
        };
        break;
    }

    // 左下角
    switch (pRadius->bottom)
    {
    case 0:
        break;

    case 1:
        {
            THROWPIXEL(l, b);
        }
        break;

    case 2:
        {
            POINT pt[] = 
            {
                {l,b-1},
                {l,b}, {l+1,b}
            };
            int count = sizeof(pt)/sizeof(POINT);

            for (int i = 0; i < count; i++)
                THROWPIXEL(pt[i].x, pt[i].y);
        }
        break;

    case 3:
    default:
        {
            POINT pt[] = 
            {
                {l,b-3},
                {l,b-2},
                {l,b-1}, {l+1,b-1},
                {l,b},   {l+1,b}, {l+2,b}, {l+3,b}
            };
            int count = sizeof(pt)/sizeof(POINT);

            for (int i = 0; i < count; i++)
                THROWPIXEL(pt[i].x, pt[i].y);
        };
        break;
    }
}

void  RoundCorner_Portrait(HBITMAP hBitmap, RECT* prc)
{
    DIBSECTION  dib = {0};
    if (sizeof(dib) != GetObject(hBitmap, sizeof(dib), &dib))
        return;

    BITMAP& bm = dib.dsBm;
    if (bm.bmWidth < 8 || bm.bmHeight < 8)
        return;

    byte* pBits = (byte*)bm.bmBits;
    int   nPitch = bm.bmWidthBytes;

    RECT  rect;
    if (prc)
    {
        RECT  rcBitmap = {0, 0, bm.bmWidth, bm.bmHeight};
        IntersectRect(&rect, &rcBitmap, prc);
    }
    else
    {
        ::SetRect(&rect, 0, 0, bm.bmWidth, bm.bmHeight);
    }

    // 默认缓存在左上角，取(0,0)
    int w = prc->right;
    int h = prc->left;

    // 25%透
    POINT ptAlpha0[4] = { {0, 0}, {w-1, 0}, {0, h-1}, {w-1, h-1} };
    for (int i = 0; i < 4; i++)
    {
        byte* pAlpha = pBits + (ptAlpha0[i].y*nPitch + (ptAlpha0[i].x<<2));
        byte a = ((int)*(pAlpha+3)) >> 4;
        *pAlpha = (*pAlpha) * a >> 8;   // r
        pAlpha++;
        *pAlpha = (*pAlpha) * a >> 8;   // g
        pAlpha++;
        *pAlpha = (*pAlpha) * a >> 8;   // b
        pAlpha++;
        *pAlpha = a;                    // a
    }

    // 50%透
    POINT ptAlpha1[8] = { {1, 0}, {w-2, 0}, {0, 1}, {w-1,1},  {0, h-2}, {w-1, h-2}, {1, h-1}, {w-2, h-1} };
    for (int i = 0; i < 8; i++)
    {
        byte* pAlpha = pBits + (ptAlpha1[i].y*nPitch + (ptAlpha1[i].x<<2));
        byte a = ((int)*(pAlpha+3)) >> 2;
        *pAlpha = (*pAlpha) * a >> 8;
        pAlpha++;
        *pAlpha = (*pAlpha) * a >> 8;
        pAlpha++;
        *pAlpha = (*pAlpha) * a >> 8;
        pAlpha++;
        *pAlpha = a;
    }

    // 75%
    POINT ptAlpha2[8] = { {2, 0}, {w-3, 0}, {0, 2}, {w-1,2},  {0, h-3}, {w-1, h-3}, {2, h-1}, {w-3, h-1} };
    for (int i = 0; i < 8; i++)
    {
        byte* pAlpha = pBits + (ptAlpha2[i].y*nPitch + (ptAlpha2[i].x<<2));
        byte a = ((int)*(pAlpha+3)) * 3 >> 2;
        *pAlpha = (*pAlpha) * a >> 8;
        pAlpha++;
        *pAlpha = (*pAlpha) * a >> 8;
        pAlpha++;
        *pAlpha = (*pAlpha) * a >> 8;
        pAlpha++;
        *pAlpha = a;
    }
}

}