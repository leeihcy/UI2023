#ifndef _UI_COMPOSITOR_H_
#define _UI_COMPOSITOR_H_

// UISDK 只依赖于接口，UICompsitor工程依赖于UISDK工程
#ifdef UICOMPOSITOR_EXPORTS
#define UICOMPOSITOR_API __declspec(dllexport)
#else
#define UICOMPOSITOR_API __declspec(dllimport)
#endif

namespace UI
{

// interface  IUICompositor
// {
//     virtual void  Upload() = 0;
//     virtual void  SetNeedCommit() = 0;
// };
    
	struct UploadGpuBitmapInfo
	{
		HBITMAP hBitmap;
		int width;
		int height;
		int bpp;
		int pitch;
		byte* pFirstLineBits;
		bool hasAlphaChannel;

		LPRECT prcArray;
		UINT nCount;
	};

interface IGpuRenderLayer;
interface IHardwareComposition
{
	virtual void  Release() = 0;
    virtual IGpuRenderLayer*  CreateLayerTexture() = 0;
    virtual void  SetRootLayerTexture(IGpuRenderLayer* p) = 0;

    virtual bool  BeginCommit() = 0;
    virtual void  EndCommit() = 0;
    virtual void  Resize(UINT nWidth, UINT nHeight) = 0;
};

class GpuLayerCommitContext
{
public:
	GpuLayerCommitContext();
	~GpuLayerCommitContext();

	void  SetOffset(int x, int y);
	void  ClipRect(RECT* prc);
	void  SetClipRect(RECT* prc);
	//void  MultiMatrix(float* matrix16);
    void  MultiAlpha(byte alpha);

public:
	int  m_xOffset;
	int  m_yOffset;

	RECT  m_rcClip;
    float  m_fAlpha;

	bool   m_bTransformValid;
	float  m_matrixTransform[4][4];
};


inline GpuLayerCommitContext::GpuLayerCommitContext()
{
	m_xOffset = 0;
	m_yOffset = 0;

	SetRectEmpty(&m_rcClip);

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

inline void  GpuLayerCommitContext::ClipRect(RECT* prc)
{
	if (!prc)
		return;

	IntersectRect(&m_rcClip, prc, &m_rcClip);
}

inline void  GpuLayerCommitContext::SetClipRect(RECT* prc)
{
	if (prc)
		CopyRect(&m_rcClip, prc);
	else
		SetRectEmpty(&m_rcClip);
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

interface IGpuRenderLayer
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
    virtual void  Resize(UINT nWidth, UINT nHeight) = 0;
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
	IHardwareComposition*  UICreateHardwareComposition(HWND hWnd);

#endif