#pragma once

#include "Src\Atl\image.h"
namespace UI
{
// 2013.7.30
// 用于解决一些特效计算过程中需要临时的内存图片，避免不断的创建内存位图
// 在这里使用全局缓存图片。
class CacheBitmap
{
public:
    CacheBitmap();
    ~CacheBitmap();

    static CacheBitmap*  GetInstance()
    {
        static CacheBitmap s;
        return &s;
    }

    HBITMAP  Create(int nWidth, int nHeight);
    Image*  CreateEx(int nWidth, int nHeight);
    static  bool  IsTopdowmBitmap();
    void  Destroy();
    void  Clear(DWORD dwColor, RECT* prc);
   // void  Transparent(DWORD dwTransparent, RECT* prc);

    HDC  GetMemDC();
    void  ReleaseMemDC(HDC);
    void  SetPixel(int x, int y, long color);

protected:
    void  AdjustWH(int nWidth, int nHeight, int& nLastWidth, int& nLastHeight);

    int  CalcMin2(int n);

public:
    Image  m_buffer;
};
}

