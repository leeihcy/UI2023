#include "gpu/include/api.h"
#if defined(ENABLE_D3D10)
#include "d3d10/d3d10_compositor.h"
#include "d3d10/d3d10_app.h"
#include "d3d10/hard3dtransform.h"
#else
#include "vulkan/vkapp.h"
#include "vulkan/vkcompositor.h"
#endif
namespace ui {

/*
IGpuCompositor::IGpuCompositor()
{
  m_pImpl = nullptr;
}
IGpuCompositor::~IGpuCompositor()
{

}
void  IGpuCompositor::SetImpl(GpuComposition* p)
{
  m_pImpl = p;
}
void  IGpuCompositor::Release()
{
    delete m_pImpl;
}

bool  IGpuCompositor::BeginCommit()
{
  return m_pImpl->BeginCommit();
}
void  IGpuCompositor::EndCommit()
{
  m_pImpl->EndCommit();
}
void  IGpuCompositor::Resize(UINT nWidth, UINT nHeight)
{
  m_pImpl->Resize(nWidth, nHeight);
}

IGpuLayer*  IGpuCompositor::CreateLayerTexture()
{
    GpuLayer* p = m_pImpl->CreateLayerTexture();
    if (p)
        return p->GetIGpuLayerTexture();

    return nullptr;
}
void  IGpuCompositor::SetRootLayerSurface(IGpuLayer* p)
{
    m_pImpl->SetRootLayerTexture(p?p->GetImpl():nullptr);
}

//////////////////////////////////////////////////////////////////////////

IGpuLayer::IGpuLayer()
{
    m_pImpl = nullptr;
}
void  IGpuLayer::SetImpml(GpuLayer* p)
{
    m_pImpl = p;
}
GpuLayer*  IGpuLayer::GetImpl()
{
    return m_pImpl;
}
void  IGpuLayer::Release()
{
    m_pImpl->Release();
}

void  IGpuLayer::UploadHBITMAP(HBITMAP hBitmap, LPRECT prcArray, UINT
nCount, bool bTopdown, bool bHasAlphaChannel)
{
    m_pImpl->UploadHBITMAP(hBitmap, prcArray, nCount, bTopdown,
bHasAlphaChannel);
}

void  IGpuLayer::Resize(UINT nWidth, UINT nHeight)
{
    m_pImpl->Resize(nWidth, nHeight);
}

void  IGpuLayer::Compositor(GpuLayerCommitContext* pContext, float*
pMatrixTransform)
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
#if defined(ENABLE_D3D10)
void MultiMatrix(GpuLayerCommitContext &c, float *matrix16) {
  D3DXMATRIX mat1(matrix16);
  if (D3DXMatrixIsIdentity(&mat1))
    return;

  D3DXMATRIX mat2((float *)c.m_matrixTransform);

  mat1 *= mat2;
  memcpy(&c.m_matrixTransform, &mat1, sizeof(mat1));

  c.m_bTransformValid = true;
}
#endif

extern "C" {
UIGPUAPI IGpuCompositor *CreateGpuComposition(IGpuCompositorWindow* window) {
  std::unique_ptr<IGpuCompositor> p;
#if defined(ENABLE_D3D10)  
  p.reset(new D3D10Compositor());
#else
  p.reset(new VulkanCompositor());
#endif

  if (!p->Initialize(window)) {
    return nullptr;
  }
  return p.release();
}

UIGPUAPI bool GpuStartup() {
#if defined(ENABLE_D3D10)
  return D3D10App::Startup();
#else
  return VulkanApplication::Get().Startup();
#endif
}

UIGPUAPI void GpuShutdown() {
#if defined(ENABLE_D3D10)
  D3D10App::Shutdown();
#else
  VulkanApplication::Get().Shutdown();
#endif
}

#if defined(ENABLE_D3D10) // TODO:
UIGPUAPI IRenderLayerTransform2 *CreateHard3DTransform() {
  return Hard3DTransform::CreateInstance();
}
#endif
}
} // namespace ui