#include "stdafx.h"
#include "Inc\Util\igifimage.h"
#include "Src\Util\Gif\GifImage.h"
#include "Inc\Interface\iuiapplication.h"

IGifImage::IGifImage(GifImageBase* p)
{
    m_pImpl = p;
    m_bCreateGifImageImpl = false;
}
IGifImage::IGifImage()
{
	m_pImpl = nullptr;
	m_bCreateGifImageImpl = false;
}
IGifImage::~IGifImage()
{
    if (m_bCreateGifImageImpl)
    {
        SAFE_DELETE(m_pImpl);
    }
}

void  IGifImage::CreateGifImpl(IUIApplication* pUIApp)
{
    if (nullptr == pUIApp)
        return;

    m_pImpl = new GifImage();
    m_bCreateGifImageImpl = true;
}
void  IGifImage::CreatePnglistGifImpl()
{

}
GifImageBase*  IGifImage::GetImpl()
{
    return m_pImpl;
}
void  IGifImage::SetImpl(GifImageBase* p)
{
    if (m_pImpl && m_bCreateGifImageImpl)
    {
        SAFE_DELETE(m_pImpl);
    }
    m_pImpl = p;
    m_bCreateGifImageImpl = false;

}

bool  IGifImage::Load(const TCHAR* szPath, IMapAttribute* pMapAttrib)
{
    return m_pImpl->Load(szPath, pMapAttrib);
}
bool  IGifImage::Destroy()
{
    return m_pImpl->Destroy();
}

IGifImageRender*  IGifImage::AddRender(Gif_Timer_Notify* pNotify,  IUIApplication*  pUIApp, int* pIndex)
{
	return m_pImpl->AddRender(pNotify, pUIApp ? pUIApp->GetImpl():nullptr, pIndex);
}
bool  IGifImage::ModifyRender(Gif_Timer_Notify* pNotify, int nIndex)
{
    return m_pImpl->ModifyRender(pNotify, nIndex);
}
bool  IGifImage::DeleteRender(int nIndex)
{
    return m_pImpl->DeleteRender(nIndex);
}

bool  IGifImage::SetTransparentColor(COLORREF colTransparent)
{
    return m_pImpl->SetTransparentColor(colTransparent);
}

ImageWrap*  IGifImage::GetFrameIImage( int nIndex )
{
	return m_pImpl->GetFrameIImage(nIndex);
}