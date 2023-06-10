#ifndef GAUSSBLUR_H_0D1B7B70_79BE_4197_BAEE_BADB5FEF9329
#define GAUSSBLUR_H_0D1B7B70_79BE_4197_BAEE_BADB5FEF9329
#include <math.h>

// 本代码摘自agg
// 主要用于文字阴影显示，将浮点数转成整形计算，效率上有所提升，但却带来了误差，例如纯白色的背景变成了(250,250,250);
//
namespace UI
{

template<class T=long> 
struct recursive_blur_calc_rgb2
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
        long l_r = b1*c1.r + b2*c2.r + b3*c3.r + b4*c4.r;
        if (l_r < 0)
            r = 0;
        else
            r = (l_r) >> 16;

        long l_g = (b1*c1.g + b2*c2.g + b3*c3.g + b4*c4.g);
        if (l_g < 0)
            g = 0;
        else
            g = l_g >> 16;

        long l_b = (b1*c1.b + b2*c2.b + b3*c3.b + b4*c4.b);
        if (l_b < 0)
            b = 0;
        else
            b = l_b >> 16;
    }

    DWORD to_pix() const
    {
//         return RGB(
//             unsigned(r + 0.5), 
//             unsigned(g + 0.5), 
//             unsigned(b + 0.5)
//             )|0xFF000000;

        return (r + (g << 8) + (b << 16)) | 0xFF000000;
//         byte br = (byte)(r );
//         byte bg = (byte)(g );
//         byte bb = (byte)(b );
//         return RGB(br,bg,bb)|0xFE000000;
    }
};

//===========================================================recursive_blur，递归高斯
class recursive_blur2
{
public:
    recursive_blur2()
    {
        b = b1 = b2 = b3 = 0;

        m_sum1 = nullptr;
        m_sum2 = nullptr;
        m_buf = nullptr;
    }
    ~recursive_blur2()
    {
    }

    void blur_x(HBITMAP hBitmap, double radius, RECT* prc)
    {
        DIBSECTION  dibsection = {0};
        ::GetObject(hBitmap, sizeof(dibsection), &dibsection);
        int w = prc->right - prc->left;
        int h = prc->bottom - prc->top;

        if(radius < 0.62) return;
        if(w < 3) return;

        int wm = w-1;   // width max index
        int x = 0, y = 0;

        int nBytePerPixel = 4/*bm.bmBitsPixel >> 3*/;  // 这里的代码不支持其它图片格式
        byte* pBits = (byte*)dibsection.dsBm.bmBits;;
        int  nPitch = dibsection.dsBm.bmWidthBytes;
        
        if (dibsection.dsBmih.biHeight > 0)  // DIBOR_BOTTOMUP
        {
            pBits = (byte*)(dibsection.dsBm.bmBits)+((dibsection.dsBmih.biHeight-1)*nPitch);
            nPitch = -nPitch;
        }
        pBits = pBits + (prc->top*nPitch) + (prc->left*nBytePerPixel);   // 开始的数据指针

        for (y = prc->top; y < prc->bottom; y++)
        {
            // 对输入信号进行前向滤波
            recursive_blur_calc_rgb<> c;
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

            // 对上一步结果进行后向滤波，得到输出结果
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

    void blur_y(HBITMAP hBitmap, double radius, RECT* prc)
    {
        DIBSECTION  dibsection = {0};
        ::GetObject(hBitmap, sizeof(dibsection), &dibsection);
        int w = prc->right - prc->left;
        int h = prc->bottom - prc->top;

        if (radius < 0.62) return;
        if (h < 3) return;

        int hm = h-1; // height max index
        int x = 0, y = 0;

        int nBytePerPixel = 4/*bm.bmBitsPixel >> 3*/;  // 这里的代码不支持其它图片格式
        byte* pBits = (byte*)dibsection.dsBm.bmBits;;
        int  nPitch = dibsection.dsBm.bmWidthBytes;
        int  nPitchInt = nPitch>>2;

        if (dibsection.dsBmih.biHeight > 0)  // DIBOR_BOTTOMUP
        {
            pBits = (byte*)(dibsection.dsBm.bmBits)+((dibsection.dsBmih.biHeight-1)*nPitch);
            nPitch = -nPitch;
            nPitchInt = -nPitchInt;
        }
        pBits = pBits + (prc->top*nPitch) + (prc->left*nBytePerPixel);   // 开始的数据指针
        for (x = 0; x < w; x++)
        {
            recursive_blur_calc_rgb<> c;
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

    void blur(HBITMAP hBitmap, double radius, RECT* prc)
    {
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

        double f_b1 = double( 2.44413 * q + 
            2.85619 * q2 + 
            1.26661 * q3);

        double f_b2 = double(-1.42810 * q2 + 
            -1.26661 * q3);

        double f_b3 = double(0.422205 * q3);

        double f_b  = double(1 - (f_b1 + f_b2 + f_b3) * b0);  // 确定归一化常数B

        b  = (long)(f_b * 65536);
        b1 = (long)((f_b1 * b0) * 65536);
        b2 = (long)((f_b2 * b0) * 65536);
        b3 = (long)((f_b3 * b0) * 65536);

        int nMaxWH = max(rcBlur.right-rcBlur.left, rcBlur.bottom-rcBlur.top);
        m_sum1 = new recursive_blur_calc_rgb2<>[nMaxWH];
        m_sum2 = new recursive_blur_calc_rgb2<>[nMaxWH];
        m_buf = new int[nMaxWH];

        blur_x(hBitmap, radius, &rcBlur);
        blur_y(hBitmap, radius, &rcBlur);

        SAFE_ARRAY_DELETE(m_sum1);
        SAFE_ARRAY_DELETE(m_sum2);
        SAFE_ARRAY_DELETE(m_buf);
    }

//     double b;
//     double b1;
//     double b2;
//     double b3;
    long b;
    long b1;
    long b2;
    long b3;
  
    recursive_blur_calc_rgb2<>* m_sum1;
    recursive_blur_calc_rgb2<>* m_sum2;
    int* m_buf;

};

}

#endif // GAUSSBLUR_H_0D1B7B70_79BE_4197_BAEE_BADB5FEF9329