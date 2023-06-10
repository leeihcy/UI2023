#include "stdafx.h"
#include "gdipen.h"

GdiPen::GdiPen()
{
	m_hPen = nullptr;
}
GdiPen::~GdiPen()
{
	DeleteObject();
}

void  GdiPen::CreateInstance(IRenderPen** ppOutRef)
{
	UIASSERT(nullptr != ppOutRef);
	if (nullptr == ppOutRef)
		return;

	GdiPen* p = new GdiPen();
    p->AddRef();
	*ppOutRef = static_cast<IRenderPen*>(p);
}

void GdiPen::DeleteObject()
{
	SAFE_DELETE_GDIOBJECT(m_hPen);
}

bool GdiPen::CreateSolidPen(int nWidth, Color* pColor)
{
	if (nWidth < 0 || nullptr == pColor)
		return false;
	
	DeleteObject();

	m_hPen = ::CreatePen(PS_SOLID, nWidth, RGB(pColor->r, pColor->g, pColor->b));
	return m_hPen != nullptr;
}
bool GdiPen::CreateDotPen(int nWidth, Color* pColor)
{
	if (nWidth < 0 || nullptr == pColor)
		return false;

	DeleteObject();

	LOGBRUSH logBrush = {BS_SOLID, RGB(pColor->r, pColor->g, pColor->b), 0};
	m_hPen = ExtCreatePen(PS_GEOMETRIC|PS_DOT, 1, &logBrush, 0, nullptr);	
	return m_hPen != nullptr;
}