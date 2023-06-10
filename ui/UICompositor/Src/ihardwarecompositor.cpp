#include "stdafx.h"
#include "gpu_compositor.h"
#include "gpu_layer.h"
#include "d3d10\d3dapp.h"
#include "hard3dtransform.h"
using namespace UI;

/*
IHardwareComposition::IHardwareComposition()
{
	m_pImpl = nullptr;
}
IHardwareComposition::~IHardwareComposition()
{

}
void  IHardwareComposition::SetImpl(GpuComposition* p)
{
    m_pImpl = p;
}
void  IHardwareComposition::Release()
{
	delete m_pImpl;
}

bool  IHardwareComposition::BeginCommit()
{
    return m_pImpl->BeginCommit();
}
void  IHardwareComposition::EndCommit()
{
    m_pImpl->EndCommit();
}
void  IHardwareComposition::Resize(UINT nWidth, UINT nHeight)
{
    m_pImpl->Resize(nWidth, nHeight);
}

IGpuRenderLayer*  IHardwareComposition::CreateLayerTexture()
{
    GpuRenderLayer* p = m_pImpl->CreateLayerTexture();
    if (p)
        return p->GetIGpuLayerTexture();

    return nullptr;
}
void  IHardwareComposition::SetRootLayerSurface(IGpuRenderLayer* p)
{
    m_pImpl->SetRootLayerTexture(p?p->GetImpl():nullptr);
}

//////////////////////////////////////////////////////////////////////////

IGpuRenderLayer::IGpuRenderLayer()
{
    m_pImpl = nullptr;
}
void  IGpuRenderLayer::SetImpml(GpuRenderLayer* p)
{
    m_pImpl = p;
}
GpuRenderLayer*  IGpuRenderLayer::GetImpl()
{
    return m_pImpl;
}
void  IGpuRenderLayer::Release()
{
    m_pImpl->Release();
}

void  IGpuRenderLayer::UploadHBITMAP(HBITMAP hBitmap, LPRECT prcArray, UINT nCount, bool bTopdown, bool bHasAlphaChannel)
{
    m_pImpl->UploadHBITMAP(hBitmap, prcArray, nCount, bTopdown, bHasAlphaChannel);
}

void  IGpuRenderLayer::Resize(UINT nWidth, UINT nHeight)
{
    m_pImpl->Resize(nWidth, nHeight);
}

void  IGpuRenderLayer::Compositor(GpuLayerCommitContext* pContext, float* pMatrixTransform)
{
    m_pImpl->Compositor(pContext, pMatrixTransform);
}
#if 0
void  IGpuLayerTexture::Translation(float xPos, float yPos, float zPos)
{
	m_pImpl->Translation(xPos, yPos, zPos);
}
void  IGpuLayerTexture::TranslationBy(float xPos, float yPos, float zPos)
{
	m_pImpl->TranslationBy(xPos, yPos, zPos);
}
void  IGpuLayerTexture::Rotate(float xRotate, float yRotate, float zRotate)
{
    m_pImpl->Rotate(xRotate, yRotate, zRotate);
}
void  IGpuLayerTexture::RotateBy(float xRotate, float yRotate, float zRotate)
{
    m_pImpl->RotateBy(xRotate, yRotate, zRotate);
}
#endif
//////////////////////////////////////////////////////////////////////////



*/
//////////////////////////////////////////////////////////////////////////


void  MultiMatrix(GpuLayerCommitContext& c, float* matrix16)
{
	D3DXMATRIX  mat1(matrix16);
	if (D3DXMatrixIsIdentity(&mat1))
		return;

	D3DXMATRIX  mat2((float*)c.m_matrixTransform);

	mat1 *= mat2;
	memcpy(&c.m_matrixTransform, &mat1, sizeof(mat1));

	c.m_bTransformValid = true;
}

extern "C" 
{
    UICOMPOSITOR_API IHardwareComposition*  UICreateHardwareComposition(
        HWND hWnd)
    {
	    // ¼ì²âÓ²¼þ£º
	    GpuComposition* p = new GpuComposition(hWnd);
	    return p;
    }
    UICOMPOSITOR_API long  UIStartupGpuCompositor()
    {
        D3D10App::Startup();
        return 0;
    }

    UICOMPOSITOR_API long  UIShutdownGpuCompositor()
    {
        D3D10App::Shutdown();
        return 0;
    }

	UICOMPOSITOR_API IRenderLayerTransform2*  CreateHard3DTransform()
	{
		return Hard3DTransform::CreateInstance();
	}
}
