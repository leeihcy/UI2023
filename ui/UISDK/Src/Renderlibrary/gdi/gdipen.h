#pragma once
#include "..\renderresourceimpl.h"
#include "Inc\Interface\renderlibrary.h"

namespace UI
{
class GdiPen : public IRenderResourceImpl<IRenderPen>
{
protected:
	GdiPen();
public:
	~GdiPen();
	static   void  CreateInstance(IRenderPen** ppOutRef);

public:
	virtual  GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }
	virtual  bool  CreateSolidPen(int nWidth, Color* pColor);
	virtual  bool  CreateDotPen(int nWidth, Color* pColor);

protected:
	void   DeleteObject();

public:
	HPEN   m_hPen;
};
}

