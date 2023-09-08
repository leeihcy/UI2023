#ifndef _UI_COMPOSITOR_H_
#define _UI_COMPOSITOR_H_

// UISDK 只依赖于接口，UICompsitor工程依赖于UISDK工程
#if defined(OS_WIN)
#ifdef UICOMPOSITOR_EXPORTS
#define UICOMPOSITOR_API __declspec(dllexport)
#else
#define UICOMPOSITOR_API __declspec(dllimport)
#endif
#else
#define UICOMPOSITOR_API 
#endif

namespace ui
{

// struct  IUICompositor
// {
//     virtual void  Upload() = 0;
//     virtual void  SetNeedCommit() = 0;
// };
    
	struct UploadGpuBitmapInfo
	{
		/*HBITMAP*/llong hBitmap;
		int width;
		int height;
		int bpp;
		int pitch;
		byte* pFirstLineBits;
		bool hasAlphaChannel;

		Rect* prcArray;
		unsigned int nCount;
	};

struct IGpuRenderLayer;
struct IHardwareComposition
{
	virtual void  Release() = 0;
    virtual IGpuRenderLayer*  CreateLayerTexture() = 0;
    virtual void  SetRootLayerTexture(IGpuRenderLayer* p) = 0;

    virtual bool  BeginCommit() = 0;
    virtual void  EndCommit() = 0;
    virtual void  Resize(unsigned int nWidth, unsigned int nHeight) = 0;
};

class GpuLayerCommitContext
{
public:
	GpuLayerCommitContext();
	~GpuLayerCommitContext();

	void  SetOffset(int x, int y);
	void  ClipRect(Rect* prc);
	void  SetClipRect(Rect* prc);
	//void  MultiMatrix(float* matrix16);
    void  MultiAlpha(byte alpha);

public:
	int  m_xOffset;
	int  m_yOffset;

	Rect  m_rcClip;
    float  m_fAlpha;

	bool   m_bTransformValid;
	float  m_matrixTransform[4][4];
};


inline GpuLayerCommitContext::GpuLayerCommitContext()
{
	m_xOffset = 0;
	m_yOffset = 0;

	m_rcClip.SetEmpty();

	m_fAlpha = 1.0f;
	m_bTransformValid = false;
	memset(&m_matrixTransform, 0, sizeof(m_matrixTransform));
	m_matrixTransform[0][0]
	= m_matrixTransform[1][1]
	= m_matrixTransform[2][2]
	= m_matrixTransform[3][3] 
	= 1;
}

inline GpuLayerCommitContext::~GpuLayerCommitContext()
{

}

// void  GpuLayerCommitContext::OffsetBy(int x, int y)
// {
// 	m_xOffset += x;
// 	m_yOffset += y;
// }

inline void  GpuLayerCommitContext::SetOffset(int x, int y)
{
	m_xOffset = x;
	m_yOffset = y;
}

inline void  GpuLayerCommitContext::ClipRect(Rect* prc)
{
	if (!prc)
		return;

	m_rcClip.Intersect(*prc, &m_rcClip);
}

inline void  GpuLayerCommitContext::SetClipRect(Rect* prc)
{
	if (prc)
		m_rcClip.CopyFrom(*prc);
	else
		m_rcClip.SetEmpty();
}

inline void  GpuLayerCommitContext::MultiAlpha(byte alpha)
{
	if (alpha == 255)
		return;

	if (alpha == 0)
	{
		m_fAlpha = 0;
		return;
	}

	m_fAlpha *= alpha/255.0f;
}

struct IGpuRenderLayer
{
    virtual void  Release() = 0;

#if 0
	void  Translation(float xPos, float yPos, float zPos);
	void  TranslationBy(float xPos, float yPos, float zPos);
    void  Rotate(float xRotate, float yRotate, float zRotate);
    void  RotateBy(float xRotate, float yRotate, float zRotate);
#endif
    virtual void  Compositor(
		GpuLayerCommitContext* pContext, 
		float* pMatrixTransform) = 0;

    virtual void  UploadHBITMAP(UploadGpuBitmapInfo& info) = 0;
    virtual void  Resize(unsigned int nWidth, unsigned int nHeight) = 0;
};


//extern "C" UICOMPOSITOR_API IRenderLayerTransform2*  CreateHard3DTransform();
}

// return:
//     0  代表成功
//     -1 代表失败
extern "C" UICOMPOSITOR_API long  UIStartupGpuCompositor();

// return:
//     无意义
extern "C" UICOMPOSITOR_API long  UIShutdownGpuCompositor();

extern "C" UICOMPOSITOR_API 
	ui::IHardwareComposition*  UICreateHardwareComposition(/*HWND*/int hWnd);

#endif