//////////////////////////////////////////////////////////////////////////
//
//	Gif Image 
//
//		将gif的每一帧图片解析出来保存
//
//	[version]
//		1.0  libo 2012.3.17 beijing  
//			完成基本功能（剩下TODO）
//
//	[TODO]
//		1. 优化
//		2. 缩略图模式
//		4. 一些图片在WIN7上加载出来的数据有问题，导致显示错误，如（res\\椭圆规.gif）
//		5. 需要支持从资源中加载图片
//		7. 有一张图片的第三帧解码失败了
//
//	[Remark]
//		1. 在xp上面使用image加载gif会提示出异常
//		2. TransparentBlt MSDN解决只支持DDB，因此要将DIB转换成DDB进行透明处理
//		3. 如果存在local color table，要使用local color table，不能使用global color table
//		4. 结构的域中，不要使用 char，要使用unsigned char，否则会出错
//		5. GIF的透明色可能和背景色都是白色，但两者的索引是不一样的。因此在处理GIF透明的时候
//		   需要重新去解析索引值，将索引值==透明的特殊处理
//		6. 遇到一个gif图，共14帧，前面所有帧的disposal都是2，最后一帧是1，导致循环结束时时最后一帧的图像和第一帧的
//		   图像一起显示了。这是不是意味着当画完所有帧时，要清空背景？
//		7. 使用Gdiplus加载分拆出来的单帧GIF图时，会出现图片数据中，本来是透明位置的那个点的值的调色板索引值，居然
//		   和当前GIF的transparent index不一致，导致图片解析失败，背景色显示错误。
//         因此逐渐决定放弃Gdiplus，开始尝试自己解码
//		8. Delay Time 到底是哪一帧的时间<--应该是这一帧显示完后保留多长时间再显示下一帧
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "Src\atl\image.h"
#include "Inc\Util\igifimage.h"

using namespace ui;
#include <fstream>
#include <math.h>
#include <list>
#include <vector>

namespace ui
{
#ifdef _DEBUG
#define DEBUG_TRACE
#endif

#ifdef DEBUG_TRACE 
	#define GIFTRACE0(x) \
		::OutputDebugString(x); 
#else
	#define GIFTRACE0(x) 
#endif

#ifdef DEBUG_TRACE 
	#define GIFTRACE1_int(x,d) \
		{ \
			TCHAR szTrace[256] = _T(""); \
			wprintf(szTrace, x, d ); \
			::OutputDebugString(szTrace); \
		} 
#else
	#define GIFTRACE1_int(x,d)
#endif

interface IMessage;
class GifImageBase;
interface IStreamBufferReader;
class Application;

struct Gif_TimerItem  // 计时器数据
{
public:
	int  nId;       // 用于kill_timer
	int  nStart;    // 开始时戳
	int  nWait;     // 等待时长
	int  nRepeat;   // 循环次数 -1表示无限循环

	void* pData;    // GifImage*

public:
	// 获取剩余时间
	inline int  get_remain(){ return nWait - (::GetTickCount() - nStart); }

	// 更新下一次显示时间
//	inline void update_repeat() { nStart += nWait; };          // TODO: 使用该方法可保证准确性，但会导致在受到阻塞恢复时，gif会快进播放
    inline void update_repeat() {     // TODO: 使用该方法可保证平滑度，但不能保证响应的精度
		nStart = ::GetTickCount(); } 
};

// [0]关闭透明背景，每一帧完全覆盖前一帧     No disposal specified. The decoder is not required to take any action.
// [1]增量，每一帧相对于是前一帧增加的新内容 Do not dispose. The graphic is to be left in place.
// [2]使用透明度，绘制每帧前刷上透明背景色   Restore to background color. The area used by the graphic must be restored to the background color.
// [3]还原到前一幅图像                       Restore to previous. The decoder is required to restore the area overwritten by the graphic with what was there prior to rendering the graphic.
#define GIF_DISPOSAL_NULL               0
#define GIF_DISPOSAL_LEFT               1
#define GIF_DISPOSAL_RESTORE_BACKGROUND 2
#define GIF_DISPOSAL_RESTORE_PREVIOUS   3

#define GIF_BLOCK_FLAG_EXTENSION   0x21
#define GIF_BLOCK_FLAG_TRAILER     0x3B
#define GIF_BLOCK_FLAG_IMAGE       0x2C

#define GIF_EXT_FLAG_PLAIN         0x01
#define GIF_EXT_FLAG_CONTROL       0xF9
#define GIF_EXT_FLAG_COMMENT       0xFE
#define GIF_EXT_FLAG_APP           0xFF

#pragma pack(1)
struct GIF_FileMark
{
	GIF_FileMark()
	{
		strcpy(signature, "");
		strcpy(version, "");
	}

	char signature[3];                    //GIF标记 "GIF"
	char version[3];                      //GIF版本号 87a或89a

	bool Valid()
	{
		if( memcmp(signature, "GIF", 3) )
			return false;

		if( memcmp(version, "87a", 3) && memcmp(version, "89a", 3) )
			return false;

		return true;
	}
};

struct GIF_LogicalScreenDescriptor        // 逻辑屏幕描述块
{
	GIF_LogicalScreenDescriptor()
	{
		logical_screen_width = logical_screen_height = 0;
		size_of_global_color_table = 0;
		sort_flag = 0;
		color_resolution = 0;
		global_color_table_flag = 0;
		background_color_index = 0;
		pixel_aspect_ratio = 0;
	}

	unsigned short logical_screen_width;  // 逻辑屏幕宽度，WARNING:这个不是真实的最终的宽和高。例如一张34*33的gif图片，这两个值的结果是36*36
	unsigned short logical_screen_height; // 逻辑屏幕高度

	// 包装域
	unsigned char  size_of_global_color_table : 3;  // 全局彩色表大小（全局彩色表的长度为：3*pow(2,this+1)）
	unsigned char  sort_flag : 1;                   // 彩色排序标志（0表示没有重要性排序，1表示最重要的颜色在前）
	unsigned char  color_resolution : 3;            // 彩色分辨率（??）
	unsigned char  global_color_table_flag : 1;     // 全局彩色标志（是否存在全局彩色表）

	unsigned char  background_color_index;          // 背景颜色索引
	unsigned char  pixel_aspect_ratio;              // 像素宽高比
};


struct GIF_GraphicControlExtension    // gif图片帧按钮数据
{
	GIF_GraphicControlExtension()
	{
		block_size = 0;
		transparent_color_flag = 0;
		user_input_flag = 0;
		disposal_methold = 0;
		reserved = 0;
		delay_time = 0;
		transparent_color_index = 0;
		block_terminator = 0;
	}

	unsigned char  block_size;             // 固定为4
	unsigned char  transparent_color_flag : 1;
	unsigned char  user_input_flag : 1;
	unsigned char  disposal_methold : 3;   // 重要：见GIF_DISPOSAL_NULL的定义。在绘制完当前帧图像之后（不是之前），需要如何处理：Indicates the way in which the graphic is to be treated after being displayed.
	unsigned char  reserved : 3;
	unsigned short delay_time;             // 重要：每一帧的显示时间
	unsigned char  transparent_color_index;// 重要：每一帧的透明色
	unsigned char  block_terminator;
};

struct GIF_ImageDescriptor                // 图像描述块
{
	GIF_ImageDescriptor() 
	{
		image_left_position = image_top_position = 0;
		image_width = image_height = 0;
		size_of_local_color_table = 0;
		reserved = 0;
		sort_flag = 0;
		interlace_flag = 0;
		local_color_table_flag = 0;
	}

	unsigned short image_left_position;    // 图像左边位置(px)
	unsigned short image_top_position;     // 图像顶部位置(px)
	unsigned short image_width;            // 图像宽度
	unsigned short image_height;           // 图像高度

	unsigned char size_of_local_color_table:3;      // 用于计算局部彩色表中包含的字节数
	unsigned char reserved : 2;                     // 保留
	unsigned char sort_flag : 1;                    // 与全局彩色表中的sort flag域含义相同
	unsigned char interlace_flag : 1;               // 表示该图标是不是交插图像
	unsigned char local_color_table_flag : 1;       // 局部彩色表标志
};
#pragma pack()

//
//	gif帧数据
//	
struct GIF_Frame
{
	GIF_Frame();
	~GIF_Frame();

	UI::ImageWrap*  GetIImage();

public:
	GIF_GraphicControlExtension  control;      // 包含了这一帧图像的延时
	GIF_ImageDescriptor          descriptor;   // 包含了这一帧图像的偏移和大小

	int           nRealDelayTime;              // = control.delay_time*10，避免重复计算
	UI::Image     image;                       // 重要：图片解析器

private:
	UI::ImageWrap*   pImageWrap;                  // 返回给外部使用的指针，由内部负责维护其生命周期
};






// 一个GIF可能会在多处显示，因此GifImage类至少会包含一个GifImageDrawItem
class GifImageRender : public IGifImageRender
{
public:
	GifImageRender(GifImageBase* pGifImage, Gif_Timer_Notify* pNotify, Application*  pUIApp);
protected:
	~GifImageRender();

public:

	virtual void   Start();
	virtual void   Pause();
	virtual void   Stop();
	virtual void   OnAlphaPaint(HDC hDC, int x, int y);
	virtual void   OnAlphaPaint(HDC hDC, int x, int y, int cx, int cy);
	virtual void   OnPaint(HDC hDC, int x, int y);
	virtual GIF_DRAW_STATUS GetStatus();
	virtual void   Release();
	virtual int    GetWidth();
	virtual int    GetHeight();
	virtual IGifImage*  GetIGifImage();

	GifImageBase*  GetGifImage() { return m_pGifImage; }
	bool   ModifyRenderParam(Gif_Timer_Notify* pNotify); 	

protected:
	void   draw_frame(GIF_Frame* pFrame);
	int    get_next_frame_index();
	void   handle_disposal(GIF_Frame* pFrame);
	void   commit(HDC hDC, int x, int y);

public:  // Gif绘制线程调用函数
	void   on_tick(Gif_TimerItem* pTimerItem);   

protected:
	Gif_Timer_Notify  m_notify;       // 到过绘制下一帧时间时的通知方式

	HDC         m_hMemCanvasDC;       // m_hDC的兼容DC，双缓冲
	HBITMAP     m_hMemCanvasBitmap;   // 双缓冲
	HDC         m_hMemPrevSaveDC;     // 保存上一帧图片的兼容DC, disposal=3的情况
	HBITMAP     m_hMemPrevSaveBitmap; // 保存上一帧图片, disposal=3的情况

	int         m_nCurFrameIndex;     // 当前绘制帧索引
	GIF_DRAW_STATUS  m_nDrawStatus;   // 当前gif绘制状态:开始、暂停、停止

	GifImageBase*    m_pGifImage;
	Application*   m_pUIApp;
	friend class     GifImageBase;
	friend class     GifImage;
};

typedef map<int, GifImageRender*> GifImageRenderItemMap;

// 派生出 GifImage/PngListGifImage
class GifImageBase
{
public:
	GifImageBase();
	virtual ~GifImageBase();

    void  SetIGifImage(IGifImage* p);
    IGifImage*  GetIGifImage();

public:  // 外部接口
    virtual bool  Load(const TCHAR* szPath, IMapAttribute* pMapAttrib=nullptr) = 0;
    virtual bool  Load(IStreamBufferReader*  pBuffer, IMapAttribute* pMapAttrib=nullptr) = 0;
    virtual bool  RealLoad(IStreamBufferReader*  pBuffer) = 0;
	virtual bool  Destroy() = 0;
    
	bool   IsLoaded();
    bool   SetTransparentColor(COLORREF colTransparent = GetSysColor(COLOR_BTNFACE));
public:
	
	GifImageRender*  AddRender(Gif_Timer_Notify* pNotify, Application*  pUIApp, int* pIndex = nullptr);
	bool   ModifyRender(Gif_Timer_Notify* pNotify, int nIndex=-1);
	bool   DeleteRender(int nIndex=-1);

	void   Start(int nIndex = -1);
	void   Pause(int nIndex = -1);
	void   Stop(int nIndex = -1);
	void   OnPaint(HDC hDC, int nIndex = -1);
	void   OnPaint(HDC hDC, int x, int y, int nIndex = -1);
	GIF_DRAW_STATUS GetStatus(int nIndex = -1);

	int    GetWidth()  { return m_nImageWidth; }
	int    GetHeight() { return m_nImageHeight; }
	int    GetFrameCount() { return (int)m_vFrame.size(); }
	GIF_Frame*  GetFrame( int nIndex );
	Image*   GetFrameImage( int nIndex );
	ImageWrap*  GetFrameIImage( int nIndex );
	

protected:   // 内部接口
	GifImageRender*  GetDrawItemByIndex(int nIndex);
	void    DeleteRender(GifImageRender*  pItem);

protected:
	vector<GIF_Frame*>  m_vFrame;             // gif图片帧列表

	int                 m_nImageWidth;        // 图片宽度
	int                 m_nImageHeight;       // 图片高度

	int                 m_nNextRenderItemIndex; // 下一个GifImageDrawItem的标识
	GifImageRenderItemMap m_mapRenderItem;        // 绘图数据列表

	//////////////////////////////////////////////////////////////////////////
	//
	// 1. Gif中的背景画刷是可以自己定义的。
	//     a. 可以采用GIF中的background_color_index字段来获取其内部的背景色
	//     b. 可以采用windows的系统窗口颜色作为背景色
	//	   c. 可以自己定义一个颜色作为背景色
	//
	// 2. background_color_index引用的是全局颜色表中的值，如果没有全局颜色表就没有该背景画刷
	//
	// 3. 每一帧中的disposal==2 restore to background是表示还原该帧RECT范围内的背景，而不是还原整幅图片的背景
	//
	//////////////////////////////////////////////////////////////////////////
	HBRUSH              m_hBrushTransparent;  // 用于实现透明的画刷

	HWND                m_hForwardMsgHWND;    // 用于GifImageRenderItem的线程发送消息
	friend class GifImageRender;

private:
    IGifImage*  m_pIGifImage;
    bool  m_bCreateIGifImage;
};


class GifImage : public GifImageBase
{
public:
	GifImage();
	~GifImage();

protected:
	int    skip_data_block(IStreamBufferReader*  pBuffer, byte* pBits=nullptr);
	bool   decode_by_lzw(IStreamBufferReader*  pBuffer, GIF_Frame* pFrame, byte byte_LZW_code_size, byte* pColorTable, int nColorTableSize);
	bool   decode_by_gdiplus(IStreamBufferReader*  pBuffer, GIF_Frame* pFrame, int  nFrameStartPos, GIF_FileMark& header, GIF_LogicalScreenDescriptor& logicScreenDesc, byte* pColorTable, int nColorTableSize);
	bool   decode_gif_image_transparent(GIF_Frame* pFrame, int nTransparentIndex);
	void   build_one_frame_data(GIF_FileMark*, GIF_LogicalScreenDescriptor*, void* pColorTable, int nColorTableSize, void* pImageData, int nImageDataSize, void** ppOut, int* pOutSize  );

public:
	bool   Load(const TCHAR* szPath, IMapAttribute* pMapAttrib=nullptr);
    bool   Load(IStreamBufferReader*  pBuffer, IMapAttribute* pMapAttrib=nullptr);
	bool   RealLoad(IStreamBufferReader*  pBuffer);
	bool   Destroy();
};




//////////////////////////////////////////////////////////////////////////
//
//  LZW 解码 GIF 数据
//
//  参考资料:
//		http://www.w3.org/Graphics/GIF/spec-gif89a.txt
//		http://blog.csdn.net/norsd/article/details/2823159
//		http://blog.csdn.net/whycadi/article/details/760576
//
//	TODO:
//		有一张图片解码失败。(wSuffix>m_nDictUpper)，而不是刚好相等... 后来用
//      gdiplus解码成功了。到底是哪错了呢？
//
//////////////////////////////////////////////////////////////////////////
struct DictItem
{
	WORD  prefix;
	byte  suffix;
	byte  stack;  // ??
};

class GifLZWDecoder 
{
public:
	GifLZWDecoder(byte nInitBitLength, byte* pDecodeResultData, int nDecodeResultSize, int nwidth, int nheight);

	bool  Decode(const byte* pSrcData, int nSrcDataSize);

	// 检查prefix suffix是否在字典中存在
	inline bool  CheckExist(WORD wValue1, WORD wValue2);

	// 输出一个有效结果。如果w值仍然是一个字典项，继续搜索。
	inline void  Output(WORD w);

	// 向字典中添加一项
	inline void  PushDict(WORD wPrefix, byte wSuffix);
			    
private:
	DictItem  m_dict[4097];      // 字典数组. GIF规范建议的 2^12 大小，其中前2^8其实是不需要管的，为原始数据（0-255）的索引
	int    m_nDictLower;         // 当前字典中的第一个有效索引，初始为 255+2
	int    m_nDictUpper;         // 当前字典中的最大索引，初始为255+2

	int    m_nInitBitLength;     // 图片帧数据流的第一个数据，LZW code size，表示m_nCurBitLength的初始值
	int    m_nCurBitLength;      // 当前要从数据流中读取的位数（不是字节数）
	int    m_nCurBitLengthEx;    // 用于比较，避免重复计算

	byte*  m_pResultData;        // 解码后输出的数据
	int    m_nResultDataSize;    // m_pResultData的数据大小。解码完后，m_pResultData应该等于0
	int    m_nWidth;             // 初始化时 m_nResultDataSize = m_nWidth*m_nHeight，指图片的宽和高
	int    m_nHeight;

	WORD   GIF_LZW_CLEAN_TAG;    // CLEAR标记的值
	WORD   GIF_LZW_END_TAG;      // END标记的值
};

//
//	png列表形式的GIF图片
//
class PngListGifImage : public GifImageBase
{
public:
	PngListGifImage(){

	}

    virtual bool  Load(const TCHAR* szPath, IMapAttribute* pMapAttrib=nullptr) { return false; }
    virtual bool  Load(IStreamBufferReader*  pBuffer, IMapAttribute* pMapAttrib=nullptr) { return false; }
	virtual bool  Destroy() {return false;}

protected:
	virtual bool  RealLoad(IStreamBufferReader*  pBuffer) { return false; }

protected:
	Image  m_pngImage;
};

}