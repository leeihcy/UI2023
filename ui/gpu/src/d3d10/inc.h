#ifndef _UI_GPU_SRC_D3D10_INC_H_
#define _UI_GPU_SRC_D3D10_INC_H_
#include "sdk/include/common/math/rect.h"

#include <d3dcommon.h>
// #include <d3d10.h>
#include <d3d10_1.h>
#include <d3dx10.h>

#include <atlbase.h>
#include <assert.h>

namespace ui
{

typedef struct _D3DCOLORVALUE {
	float r;
	float g;
	float b;
	float a;
} D3DCOLORVALUE;

} // namespace ui

#endif