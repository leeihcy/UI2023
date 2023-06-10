#ifndef _UI_COLOR_H_
#define _UI_COLOR_H_

namespace UI
{
// 换色
#if 0
#define MIN_HUE_VALUE           0              // 色调  HLS范围定义
#define MAX_HUE_VALUE           240
#define MIN_LUMINANCE_VALUE     0              // 亮度
#define MAX_LUMINANCE_VALUE     240            
#define MIN_SATURATION_VALUE    0              // 饱和度
#define MAX_SATURATION_VALUE    240            
#else
#define MIN_HUE_VALUE           0              // 色调  HLS范围定义
#define MAX_HUE_VALUE           360
#define MIN_LUMINANCE_VALUE     0              // 亮度
#define MAX_LUMINANCE_VALUE     1            
#define MIN_SATURATION_VALUE    0              // 饱和度
#define MAX_SATURATION_VALUE    1            
#endif

#define CHANGE_SKIN_HLS_FLAG_H             1
#define CHANGE_SKIN_HLS_FLAG_L             2
#define CHANGE_SKIN_HLS_FLAG_S             4
#define CHANGE_SKIN_HLS_FLAG_HLS           (CHANGE_SKIN_HLS_FLAG_H|CHANGE_SKIN_HLS_FLAG_L|CHANGE_SKIN_HLS_FLAG_S)
#define CHANGE_SKIN_HLS_FALG_REPLACE_MODE  8 // 着色模式??


typedef struct 
{
    double  hue;         // [0 ~ 360^o)
    double  saturation;  // [0, 1]
    double  lightness;   // [0, 1]
}HSL;

typedef struct
{
    double  hue;         // [0 ~ 360^o)
    double  saturation;  // [0, 1]
    double  value;       // [0, 1]
}HSV, HSB;

class UIAPI Color
{
public:
    Color();
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    Color(DWORD rgba);
    ~Color();
    static Color* CreateInstance(COLORREF color);

    operator COLORREF();
    union
    {
        COLORREF   m_col;
        struct  
        {
			unsigned char   r;  // 注：位图内存中颜色的排列顺序为b g r a
			unsigned char   g;
			unsigned char   b;
			unsigned char   a;
        };
    };

	unsigned char   GetR();
	unsigned char   GetG();
	unsigned char   GetB();
	unsigned char   GetA();

	void  SetR(unsigned char n);
	void  SetG(unsigned char n);
	void  SetB(unsigned char n);
	void  SetA(unsigned char n);

    // 保留ALPHAW值颜色拷贝
    void  ReplaceRGB(COLORREF c);
	void  ReplaceRGB(unsigned char r1, unsigned char g1, unsigned char b1);
    void  SetColor(COLORREF c);
    COLORREF  GetGDIValue();

    void  GetHSL(HSL& hsl);
    void  GetHSV(HSV& hsv);
    void  SetHSL(HSL& hsl);
    void  SetHSV(HSV& hsv);
    
    void  ToHexString(TCHAR* szBuffer);
    void  ToWebString(TCHAR* szBuffer);

    // 在外部使用Color对象时，既有可能是通过GetColor获取到的Color*指针，也有可能外部自己直接设置
    // 一个COLORREF值，这个时候就得new一个Color*，使得逻辑与GetColor保持一致。为了达到释放逻辑一
    // 致的目的，将释放函数都写成Release()
    long  AddRef();
    long  Release();

private:
    long  lRef;

#ifdef EDITOR_MODE
public:
    // 为了在编辑器中区分开是使用id引用的颜色，还是直接书写的颜色值，增加该逻辑
    void  SetResId(LPCTSTR);
    LPCTSTR  GetResId();

private:
    LPTSTR  m_szResId;
#endif

};
typedef Color* ColorPtr;

}
#endif  // _UI_COLOR_H_
