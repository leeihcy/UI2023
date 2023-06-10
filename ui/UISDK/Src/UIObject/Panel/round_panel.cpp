#include "stdafx.h"
#include "round_panel.h"
#include "Src\Util\RoundRect\roundrect.h"
#include "panel_desc.h"
#include "Src\Base\Attribute\attribute.h"

namespace UI
{

static  BufferSwitcher  s_parentBuf;
static  RoundRectHandler  s_RoundRectHandler;


 RoundPanel::RoundPanel(IRoundPanel* p) : Panel(p)
{
    m_lefttop = 0;
    m_righttop = 0;
    m_leftbottom = 0;
    m_rightbottom = 0;
	m_pIRoundPanel = p;

	m_objStyle.post_paint = 1;
	SetDescription(RoundPanelDescription::Get());
    s_parentBuf.AddRef();
}
RoundPanel::~RoundPanel()
{
    s_parentBuf.Release();
}



void  RoundPanel::PrePaint(HDC hDC, int width, int height)
{
    if (m_lefttop)   
    {
        s_parentBuf.SaveLeftTop(hDC, m_lefttop);
    }
    if (m_righttop)
    {
        s_parentBuf.SaveRightTop(hDC, m_righttop, width);
    }
    if (m_leftbottom)
    {
        s_parentBuf.SaveLeftBottom(hDC, m_leftbottom, height);
    }
    if (m_rightbottom)
    {
        s_parentBuf.SaveRightBottom(hDC, m_rightbottom, width, height);
    }
}

void  RoundPanel::SetRadius(
        ULONG lefttop, ULONG righttop, ULONG leftbottom, ULONG rightbottom)
{
    m_lefttop = lefttop;
    m_righttop = righttop;
    m_leftbottom = leftbottom;
    m_rightbottom = rightbottom;
}

void  RoundPanel::OnSerialize(SERIALIZEDATA* pData)
{
	__super::OnSerialize(pData);

	AttributeSerializer s(pData, TEXT("RoundPanel"));
	s.AddLong(XML_ROUNDPANEL_CORNER_LEFTTOP, *(long*)&m_lefttop);
	s.AddLong(XML_ROUNDPANEL_CORNER_RIGHTTOP, *(long*)&m_righttop);
	s.AddLong(XML_ROUNDPANEL_CORNER_LEFTBOTTOM, *(long*)&m_leftbottom);
	s.AddLong(XML_ROUNDPANEL_CORNER_RIGHTBOTTOM, *(long*)&m_rightbottom);
}

void RoundPanel::OnPaint(IRenderTarget* pRenderTarget)
{
	PrePaint(pRenderTarget->GetHDC(), GetWidth(), GetHeight());
	__super::OnPaint(pRenderTarget);
} 

void RoundPanel::OnPostPaint(IRenderTarget* pRenderTarget)
{
	__super::OnPostPaint(pRenderTarget);
	PostPaint(pRenderTarget->GetHDC(), GetWidth(), GetHeight());
} 

void  RoundPanel::PostPaint(HDC hDC, int width, int height)
{
    if (m_lefttop == 0 && 
        m_righttop == 0 &&
        m_leftbottom == 0 && 
        m_rightbottom == 0)
        return;

    // 1. 圆角化新背景

    HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
    RECT rcPanel = {0,0, width, height};
    POINT ptOffset;
    ::GetViewportOrgEx(hDC, &ptOffset);
    OffsetRect(&rcPanel, ptOffset.x, ptOffset.y);

    // 直接change bitmap bit无视了剪裁区域，而后面的还原用的bitblt又受
    // 剪裁区影响，因此这里需要更进一步的限制一下，不在剪裁区域内的，不
    // 能调用Apply2Bitmap
    RECT rcClip;
    GetClipBox(hDC, &rcClip);
	::OffsetRect(&rcClip, ptOffset.x, ptOffset.y); 

    // TODO:

    s_RoundRectHandler.Set(m_lefttop, m_righttop, m_rightbottom, m_leftbottom);
    s_RoundRectHandler.Apply2Bitmap(
			hBitmap, 
			true, 
			&rcPanel, 
			&rcClip);

    // 2. 将圆角图画到保存的背景上去
    // 3. 还原背景
    if (m_lefttop)
    {
        s_parentBuf.UpdateLeftTop(hDC, m_lefttop);
        s_parentBuf.RestoreLeftTop(hDC, m_lefttop);
    }
    if (m_righttop)
    {
        s_parentBuf.UpdateRightTop(hDC, m_righttop, width);
        s_parentBuf.RestoreRightTop(hDC, m_righttop, width);
    }
    if (m_leftbottom)
    {
        s_parentBuf.UpdateLeftBottom(hDC, m_leftbottom, height);
        s_parentBuf.RestoreLeftBottom(hDC, m_leftbottom, height);
    }
    if (m_rightbottom)
    {
        s_parentBuf.UpdateRightBottom(hDC, m_rightbottom, width, height);
        s_parentBuf.RestoreRightBottom(hDC, m_rightbottom, width, height);
    }
}

//////////////////////////////////////////////////////////////////////////


BufferSwitcher::BufferSwitcher()
{
    m_lRef = 0;
}
BufferSwitcher::~BufferSwitcher()
{

}

void  BufferSwitcher::AddRef()
{
    m_lRef ++;
}
void  BufferSwitcher::Release()
{
    -- m_lRef;

    if (0 == m_lRef)
    {
        m_lefttop.Destroy();
        m_righttop.Destroy();
        m_leftbottom.Destroy();
        m_rightbottom.Destroy();
    }
}


void  BufferSwitcher::SaveLeftTop(HDC hDC, ULONG radius)
{
    save(m_lefttop, hDC, radius, 0, 0);
}

void  BufferSwitcher::SaveRightTop(HDC hDC, ULONG radius, int width)
{
    save(m_righttop, hDC, radius, width-radius, 0);
}

void  BufferSwitcher::SaveLeftBottom(HDC hDC, ULONG radius, int height)
{
    save(m_leftbottom, hDC, radius, 0, height-radius);
}

void  BufferSwitcher::SaveRightBottom(HDC hDC, ULONG radius, int width, int height)
{
    save(m_rightbottom, hDC, radius, width-radius, height-radius);
}


void  BufferSwitcher::UpdateLeftTop(HDC hDC, ULONG radius)
{
    update(m_lefttop, hDC, radius, 0, 0);
}

void  BufferSwitcher::UpdateRightTop(HDC hDC, ULONG radius, int width)
{
    update(m_righttop, hDC, radius, width-radius, 0);
}

void  BufferSwitcher::UpdateLeftBottom(HDC hDC, ULONG radius, int height)
{
    update(m_leftbottom, hDC, radius, 0, height-radius);
}

void  BufferSwitcher::UpdateRightBottom(HDC hDC, ULONG radius, int width, int height)
{
    update(m_rightbottom, hDC, radius, width-radius, height-radius);
}

void  BufferSwitcher::RestoreLeftTop(HDC hDC, ULONG radius)
{
    restore(m_lefttop, hDC, radius, 0, 0);
}

void  BufferSwitcher::RestoreRightTop(HDC hDC, ULONG radius, int width)
{
    restore(m_righttop, hDC, radius, width-radius, 0);
}
void  BufferSwitcher::RestoreLeftBottom(HDC hDC, ULONG radius, int height)
{
    restore(m_leftbottom, hDC, radius, 0, height-radius);
}
void  BufferSwitcher::RestoreRightBottom(HDC hDC, ULONG radius, int width, int height)
{
    restore(m_rightbottom, hDC, radius, width-radius, height-radius);
}

void  BufferSwitcher::save(
        UI::Image& image, HDC hDC, ULONG radius, int x, int y)
{
    if (image.IsNull() || 
        image.GetWidth() < (int)radius || 
        image.GetHeight() < (int)radius)
    {
        image.Destroy();
        image.Create(radius, radius, 32, Image::createAlphaChannel);
    }

    HDC hDestDC = image.GetDC();
    ::BitBlt(hDestDC, 0, 0, radius, radius, hDC, x, y, SRCCOPY);
    image.ReleaseDC();
}

void  BufferSwitcher::update(UI::Image& image, HDC hDC, ULONG radius, int x, int y)
{
    if (image.IsNull())
        return;

    HDC hDestDC = image.GetDC();
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    ::AlphaBlend(hDestDC, 0, 0, radius, radius, hDC, x, y, radius, radius, bf);
    image.ReleaseDC();
}

void  BufferSwitcher::restore(
        UI::Image& image, HDC hDC, ULONG radius, int x, int y)
{
    if (image.IsNull())
        return;

    image.BitBlt(hDC, x, y, radius, radius, 0, 0, SRCCOPY);
    //image.Draw(hDC, x, y, radius, radius);

}

//////////////////////////////////////////////////////////////////////////

}