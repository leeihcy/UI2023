#include "stdafx.h"
/*#include "gdiplusfix.h"*/
#include "gdipluspen.h"

GdiplusPen::GdiplusPen()
{
	m_pPen = NULL;
}
GdiplusPen::~GdiplusPen()
{
	DeleteObject();
}

void  GdiplusPen::CreateInstance(IRenderPen** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return;

	GdiplusPen* p = new GdiplusPen();
    p->AddRef();
	*ppOutRef = static_cast<IRenderPen*>(p);
}

void GdiplusPen::DeleteObject()
{
	SAFE_DELETE(m_pPen);
}

bool GdiplusPen::CreateSolidPen(int nWidth, Color* pColor)
{
	if (nWidth < 0 || NULL == pColor)
		return false;
	
	DeleteObject();

	Gdiplus::Color color(254, pColor->GetR(), pColor->GetG(), pColor->GetB());
	m_pPen = new Gdiplus::Pen(color, (Gdiplus::REAL)nWidth);
	return m_pPen != NULL;
}
bool GdiplusPen::CreateDotPen(int nWidth, Color* pColor)
{
	if (nWidth < 0 || NULL == pColor)
		return false;

	DeleteObject();

	Gdiplus::Color color(254, pColor->GetR(), pColor->GetG(), pColor->GetB());
	m_pPen = new Gdiplus::Pen(color, (Gdiplus::REAL)nWidth);
	m_pPen->SetDashStyle(Gdiplus::DashStyleDot);

	return m_pPen != NULL;
}