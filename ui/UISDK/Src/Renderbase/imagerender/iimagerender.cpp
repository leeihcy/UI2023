#include "stdafx.h"
#include "Inc\Interface\iimagerender.h"
#include "Src\Renderbase\imagerender\imagerender.h"

namespace UI
{

UI_IMPLEMENT_INTERFACE(ImageRender, RenderBase)
UI_IMPLEMENT_INTERFACE(ImageListItemRender, ImageRender)
UI_IMPLEMENT_INTERFACE(ImageListRender, RenderBase)

void  IImageRender::SetAlpha(int nAlpha) 
{ 
	__pImpl->SetAlpha(nAlpha); 
}
int   IImageRender::GetAlpha() 
{
	return __pImpl->GetAlpha(); 
}
void  IImageRender::SetColor(Color c) 
{ 
	__pImpl->SetColor(c); 
}
Color IImageRender::GetColor()
{
	return __pImpl->GetColor(); 
}
void  IImageRender::SetImageDrawType(int n)
{ 
	__pImpl->SetImageDrawType(n);
}
int   IImageRender::GetImageDrawType()
{
	return __pImpl->GetImageDrawType(); 
}
void  IImageRender::SetImageStretch9Region(const C9Region& r)
{
	__pImpl->SetImageStretch9Region(r);

}
BKCOLOR_FILL_TYPE  IImageRender::GetBkColorFillType()
{
	return __pImpl->GetBkColorFillType(); 
}
void  IImageRender::SetBkColorFillType(BKCOLOR_FILL_TYPE eType) 
{
	__pImpl->SetBkColorFillType(eType);
}
IRenderBitmap*  IImageRender::GetRenderBitmap()
{ 
	return __pImpl->GetRenderBitmap();
}
void  IImageRender::SetRenderBitmap(IRenderBitmap* pBitmap)
{
	__pImpl->SetRenderBitmap(pBitmap);
}

void  IImageListRender::SetIImageListRenderBitmap(IImageListRenderBitmap* pBitmap)
{ 
	__pImpl->SetIImageListRenderBitmap(pBitmap);
}
IRenderBitmap*  IImageListRender::GetRenderBitmap() 
{ 
	return __pImpl->GetRenderBitmap(); 
}
void  IImageListRender::SetImageDrawType(int n)
{
	__pImpl->SetImageDrawType(n);
}
int   IImageListRender::GetImageDrawType() 
{ 
	return __pImpl->GetImageDrawType(); 
}
void  IImageListRender::SetImageStretch9Region(C9Region* p)
{
	return __pImpl->SetImageStretch9Region(p);
}
int   IImageListRender::GetItemWidth() 
{ 
	return __pImpl->GetItemWidth();
}
int   IImageListRender::GetItemHeight()
{
	return __pImpl->GetItemHeight(); 
}
int   IImageListRender::GetItemCount() 
{ 
	return __pImpl->GetItemCount(); 
}


}