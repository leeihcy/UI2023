#pragma once
#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"

namespace UI
{
class GdiplusPen : public IRenderResourceImpl<IRenderPen>
{
protected:
	GdiplusPen();
public:
	~GdiplusPen();
	static   void  CreateInstance(IRenderPen** ppOutRef);

public:
	virtual  GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS; }
	virtual  bool  CreateSolidPen(int nWidth, Color* pColor);
	virtual  bool  CreateDotPen(int nWidth, Color* pColor);

protected:
	void   DeleteObject();

public:
	Gdiplus::Pen*  m_pPen;
};
}