#ifndef GAUSSBLUR_H_0D1B7B70_79BE_4197_BAEE_BADB5FEF9328
#define GAUSSBLUR_H_0D1B7B70_79BE_4197_BAEE_BADB5FEF9328
#include <math.h>

// 本代码摘自agg
// 主要用于文字阴影显示，但目前为止自己仍然无法理解这个算法实现的原理是什么

namespace ui
{

#define RECURSIVE_BLUR_FLAG_X_ONLY  0x0001
#define RECURSIVE_BLUR_FLAG_Y_ONLY  0x0002
#define RECURSIVE_BLUR_FLAG_BITMAP_BOTTOMUP  0x0004

    // 普通的图片模糊用的参数，不需要计算alpha通道
template<class T=double> 
struct recursive_blur_calc_rgb
{
    typedef T value_type;
    typedef recursive_blur_calc_rgb<T> self_type;

    value_type r,g,b;

    void from_pix(DWORD c)
    {
        r = GetRValue(c);
        g = GetGValue(c);
        b = GetBValue(c);
    }

    void calc(value_type b1, 
        value_type b2, 
        value_type b3, 
        value_type b4,
        const self_type& c1, 
        const self_type& c2, 
        const self_type& c3, 
        const self_type& c4)
    {
        r = b1*c1.r + b2*c2.r + b3*c3.r + b4*c4.r;
        g = b1*c1.g + b2*c2.g + b3*c3.g + b4*c4.g;
        b = b1*c1.b + b2*c2.b + b3*c3.b + b4*c4.b;
    }

    DWORD to_pix() const
    {
        return RGB(
            unsigned(r + 0.5), 
            unsigned(g + 0.5), 
            unsigned(b + 0.5)
            )|0xFF000000;
    }
};


// 文字阴影用的计算参数。文字的阴影也需要透明，因此也要携带alpha进行计算，然后贴到背景上面去
template<class T=double> 
struct recursive_blur_calc_rgba
{
    typedef T value_type;
    typedef recursive_blur_calc_rgba<T> self_type;

    value_type r,g,b,a;

    void from_pix(DWORD c)
    {
        r = GetRValue(c);
        g = GetGValue(c);
        b = GetBValue(c);
        a = (LOBYTE((c)>>24));
    }

    void calc(value_type b1, 
        value_type b2, 
        value_type b3, 
        value_type b4,
        const self_type& c1, 
        const self_type& c2, 
        const self_type& c3, 
        const self_type& c4)
    {
        r = b1*c1.r + b2*c2.r + b3*c3.r + b4*c4.r;
        g = b1*c1.g + b2*c2.g + b3*c3.g + b4*c4.g;
        b = b1*c1.b + b2*c2.b + b3*c3.b + b4*c4.b;
        a = b1*c1.a + b2*c2.a + b3*c3.a + b4*c4.a;
    }

    DWORD to_pix() const
    {
        return RGBA(
            unsigned(r + 0.5), 
            unsigned(g + 0.5), 
            unsigned(b + 0.5),
            unsigned(a + 0.5));
    }
};

//===========================================================recursive_blur
template<class rgbatype=recursive_blur_calc_rgb<> >
class recursive_blur
{
public:
    recursive_blur()
    {
        b = b1 = b2 = b3 = 0;
        m_sum1 = nullptr;
        m_sum2 = nullptr;
        m_buf = nullptr;
    }
    ~recursive_blur()
    {
        SAFE_ARRAY_DELETE(m_sum1);
        SAFE_ARRAY_DELETE(m_sum2);
        SAFE_ARRAY_DELETE(m_buf);
    }

    void blur_x(HBITMAP hBitmap, double radius, RECT* prc, bool bTopDownBitmap)
    {
        BITMAP  bm = {0};
        ::GetObject(hBitmap, sizeof(bm), &bm);
        int w = prc->right - prc->left;
        int h = prc->bottom - prc->top;

        if(radius < 0.62) return;
        if(w < 3) return;

        int wm = w-1;   // width max index
        int x = 0, y = 0;

        int nBytePerPixel = 4/*bm.bmBitsPixel >> 3*/;  // 这里的代码不支持其它图片格式
        byte* pBits = (byte*)bm.bmBits;;
        int  nPitch = bm.bmWidthBytes;
        
        if (!bTopDownBitmap)  // DIBOR_BOTTOMUP
        {
            pBits = (byte*)(pBits)+((bm.bmHeight-1)*nPitch);
            nPitch = -nPitch;
        }

        pBits = pBits + (prc->top*nPitch) + (prc->left*nBytePerPixel);   // 开始的数据指针

        for (y = prc->top; y < prc->bottom; y++)
        {
            rgbatype c;
            int* pnBits = (int*)pBits;
            c.from_pix(*pnBits);
            m_sum1[0].calc(b, b1, b2, b3, c, c, c, c);

            pnBits++;
            c.from_pix(*pnBits);
            m_sum1[1].calc(b, b1, b2, b3, c, m_sum1[0], m_sum1[0], m_sum1[0]);

            pnBits++;
            c.from_pix(*pnBits);
            m_sum1[2].calc(b, b1, b2, b3, c, m_sum1[1], m_sum1[0], m_sum1[0]);

            for (x = 3; x < w; ++x)
            {
                pnBits++;
                c.from_pix(*pnBits);
                m_sum1[x].calc(b, b1, b2, b3, c, m_sum1[x-1], m_sum1[x-2], m_sum1[x-3]);
            }

            m_sum2[wm  ].calc(b, b1, b2, b3, m_sum1[wm  ], m_sum1[wm  ], m_sum1[wm], m_sum1[wm]);
            m_sum2[wm-1].calc(b, b1, b2, b3, m_sum1[wm-1], m_sum2[wm  ], m_sum2[wm], m_sum2[wm]);
            m_sum2[wm-2].calc(b, b1, b2, b3, m_sum1[wm-2], m_sum2[wm-1], m_sum2[wm], m_sum2[wm]);
            m_buf[wm  ] = m_sum2[wm  ].to_pix();
            m_buf[wm-1] = m_sum2[wm-1].to_pix();
            m_buf[wm-2] = m_sum2[wm-2].to_pix();

            for (x = wm-3; x >= 0; --x)
            {
                m_sum2[x].calc(b, b1, b2, b3, m_sum1[x], m_sum2[x+1], m_sum2[x+2], m_sum2[x+3]);
                m_buf[x] = m_sum2[x].to_pix();
            }
            memcpy(pBits, m_buf, sizeof(int)*w);

            pBits += nPitch;
        }
    }

    void blur_y(HBITMAP hBitmap, double radius, RECT* prc, bool bTopDownBitmap)
    {
        BITMAP  bm = {0};
        ::GetObject(hBitmap, sizeof(bm), &bm);
        int w = prc->right - prc->left;
        int h = prc->bottom - prc->top;

        if (radius < 0.62) return;
        if (h < 3) return;

        int hm = h-1; // height max index
        int x = 0, y = 0;

        int nBytePerPixel = 4/*bm.bmBitsPixel >> 3*/;  // 这里的代码不支持其它图片格式
        byte* pBits = (byte*)bm.bmBits;
        int  nPitch = bm.bmWidthBytes;
        int  nPitchInt = nPitch>>2;

        if (!bTopDownBitmap)  // DIBOR_BOTTOMUP
        {
            pBits = (byte*)(pBits)+((bm.bmHeight-1)*nPitch);
            nPitch = -nPitch;
            nPitchInt = -nPitchInt;
        }

        pBits = pBits + (prc->top*nPitch) + (prc->left*nBytePerPixel);   // 开始的数据指针
        for (x = 0; x < w; x++)
        {
            rgbatype c;
            int* pnBits = (int*)pBits;

            c.from_pix(*pnBits);
            m_sum1[0].calc(b, b1, b2, b3, c, c, c, c);

            pnBits += nPitchInt;
            c.from_pix(*pnBits);
            m_sum1[1].calc(b, b1, b2, b3, c, m_sum1[0], m_sum1[0], m_sum1[0]);

            pnBits += nPitchInt;
            c.from_pix(*pnBits);
            m_sum1[2].calc(b, b1, b2, b3, c, m_sum1[1], m_sum1[0], m_sum1[0]);

            for (y = 3; y < h; ++y)
            {
                pnBits += nPitchInt;
                c.from_pix(*pnBits);
                m_sum1[y].calc(b, b1, b2, b3, c, m_sum1[y-1], m_sum1[y-2], m_sum1[y-3]);
            }

            m_sum2[hm  ].calc(b, b1, b2, b3, m_sum1[hm  ], m_sum1[hm  ], m_sum1[hm], m_sum1[hm]);
            m_sum2[hm-1].calc(b, b1, b2, b3, m_sum1[hm-1], m_sum2[hm  ], m_sum2[hm], m_sum2[hm]);
            m_sum2[hm-2].calc(b, b1, b2, b3, m_sum1[hm-2], m_sum2[hm-1], m_sum2[hm], m_sum2[hm]);
            m_buf[hm  ] = m_sum2[hm  ].to_pix();
            m_buf[hm-1] = m_sum2[hm-1].to_pix();
            m_buf[hm-2] = m_sum2[hm-2].to_pix();

            for (y = hm-3; y >= 0; --y)
            {
                m_sum2[y].calc(b, b1, b2, b3, m_sum1[y], m_sum2[y+1], m_sum2[y+2], m_sum2[y+3]);
                m_buf[y] = m_sum2[y].to_pix();
            }
            
            pnBits = (int*)pBits;            
            for (y = 0; y < h; ++y)
            {
                *pnBits = m_buf[y];
                pnBits += nPitchInt;
            }
            pBits += nBytePerPixel;
        }
    }

    void blur(HBITMAP hBitmap, double radius, RECT* prc, int nFlag)
    {
        if (radius < 0.62) 
            return;

        RECT rcBlur = {0};
        BITMAP  bm = {0};
        ::GetObject(hBitmap, sizeof(bm), &bm);
        if (bm.bmBitsPixel != 32)
            return;

        if (prc)
        {
            RECT rcBmp = {0, 0, bm.bmWidth, bm.bmHeight};
            if (FALSE == ::IntersectRect(&rcBlur, &rcBmp, prc))
                return;
        }
        else
        {
            ::SetRect(&rcBlur, 0, 0, bm.bmWidth, bm.bmHeight);
        }

        if(radius < 0.62) return;

        double s = double(radius * 0.5);
        double q = (s < 2.5) ?
            (3.97156 - 4.14554 * sqrt(1 - 0.26891 * s)) : (0.98711 * s - 0.96330);

        double q2 = double(q * q);
        double q3 = double(q2 * q);

        double b0 = double(1.0 / (1.578250 + 
            2.444130 * q + 
            1.428100 * q2 + 
            0.422205 * q3));

        b1 = double( 2.44413 * q + 
            2.85619 * q2 + 
            1.26661 * q3);

        b2 = double(-1.42810 * q2 + 
            -1.26661 * q3);

        b3 = double(0.422205 * q3);

        b  = double(1 - (b1 + b2 + b3) * b0);

        b1 *= b0;
        b2 *= b0;
        b3 *= b0;

        int nMaxWH = max(rcBlur.right-rcBlur.left, rcBlur.bottom-rcBlur.top);
        m_sum1 = new rgbatype[nMaxWH];
        m_sum2 = new rgbatype[nMaxWH];
        m_buf = new int[nMaxWH];

        bool bTopDown = true;
        if (nFlag & RECURSIVE_BLUR_FLAG_BITMAP_BOTTOMUP)
            bTopDown = false;

        if (nFlag & RECURSIVE_BLUR_FLAG_X_ONLY)
        {
            blur_x(hBitmap, radius, &rcBlur, bTopDown);
        }
        else if (nFlag & RECURSIVE_BLUR_FLAG_Y_ONLY)
        {
            blur_y(hBitmap, radius, &rcBlur, bTopDown);
        }
        else
        {
            blur_x(hBitmap, radius, &rcBlur, bTopDown);
            blur_y(hBitmap, radius, &rcBlur, bTopDown);
        }
        

        SAFE_ARRAY_DELETE(m_sum1);
        SAFE_ARRAY_DELETE(m_sum2);
        SAFE_ARRAY_DELETE(m_buf);
    }

    rgbatype* m_sum1;
    rgbatype* m_sum2;
    int* m_buf;

    double b;
    double b1;
    double b2;
    double b3;
};

}

#endif // GAUSSBLUR_H_0D1B7B70_79BE_4197_BAEE_BADB5FEF9328