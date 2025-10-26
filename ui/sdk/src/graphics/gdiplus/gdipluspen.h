#pragma once
#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"

namespace ui
{
class GdiplusPen : public IRenderResourceImpl<IRenderPen>
{
protected:
	GdiplusPen();
public:
	~GdiplusPen();
	static   void  CreateInstance(IRenderPen** ppOutRef);

public:
	virtual  eGraphicsLibraryType GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS; }
	virtual  bool  CreateSolidPen(int nWidth, Color* pColor);
	virtual  bool  CreateDotPen(int nWidth, Color* pColor);

protected:
	void   DeleteObject();

public:
	Gdiplus::Pen*  m_pPen;
};
}