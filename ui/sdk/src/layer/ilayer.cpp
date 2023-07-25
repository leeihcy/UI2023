#include "include/inc.h"
#include "include/interface/irenderlayer.h"
#include "windowrender.h"
#include "layer.h"

namespace ui {
IWindowRender::IWindowRender(WindowRender* p)
{
	m_pWindowRenderImpl = p;
}
IWindowRender::~IWindowRender()
{

}

WindowRender*  IWindowRender::GetImpl()
{
	return m_pWindowRenderImpl;
}
#if 0
void  IWindowRender::SetCanCommit(bool b)
{
	m_pWindowRenderImpl->SetCanCommit(b);
}
// void  IWindowRender::Commit(HDC hDC, RECT* prc, int nCount)
// {
//     m_pWindowRenderImpl->Commit(hDC, prc, nCount);
// }
void  IWindowRender::InvalidateNow()
{
    m_pWindowRenderImpl->InvalidateNow();
}
GRAPHICS_RENDER_LIBRARY_TYPE  IWindowRender::GetGraphicsRenderType()
{
	return m_pWindowRenderImpl->GetGraphicsRenderType();
}
void  IWindowRender::SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye)
{
	return m_pWindowRenderImpl->SetGraphicsRenderType(eTpye);
}
bool  IWindowRender::GetRequireAlphaChannel()
{
	return m_pWindowRenderImpl->GetRequireAlphaChannel();
}
void  IWindowRender::SetCommitListener(IWindowCommitListener* p)
{
    m_pWindowRenderImpl->SetCommitListener(p);
}

IWindowCommitListener*  IWindowRender::GetCommitListener()
{
    return m_pWindowRenderImpl->GetCommitListener();
}
#endif
//////////////////////////////////////////////////////////////////////////

ILayer::ILayer(Layer* p)
{
	m_pImpl = p;
}

void  ILayer::PostCompositorRequest()
{
	m_pImpl->PostCompositorRequest();
}
IRenderTarget*  ILayer::GetRenderTarget()
{
    return m_pImpl->GetRenderTarget();
}
void  ILayer::ScaleTo(float x, float y, LayerAnimateParam* param)
{
	m_pImpl->ScaleTo(x, y, param);
}
}