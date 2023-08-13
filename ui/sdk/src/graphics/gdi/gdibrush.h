#pragma once
#include "..\renderresourceimpl.h"
#include "include/interface/renderlibrary.h"

namespace ui
{
class GdiBrush : public IRenderResourceImpl<IRenderBrush>
{
protected:
	GdiBrush();
public:
	~GdiBrush();
	static   void  CreateInstance(IRenderBrush** ppOut);

public:
	virtual  GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }
	virtual  bool  CreateSolidBrush(Color* pColor);

protected:
	void     DeleteObject();

public:
	HBRUSH   m_hBrush;
};
}

