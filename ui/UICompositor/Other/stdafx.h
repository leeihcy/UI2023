// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Windows Header Files:
#include <windows.h>

#pragma warning(disable:4996)
// warning C4201: ʹ���˷Ǳ�׼��չ : �����ƵĽṹ/����
#pragma warning(disable:4201)
// warning C4100: ��lpReserved��: δ���õ��β�
#pragma warning(disable:4100)
// warning C4481: ʹ���˷Ǳ�׼��չ: ����д˵������override��
#pragma warning(disable:4481)

#pragma warning(disable:4838)

#include <windows.h>
#include <tchar.h>
#include <vector>
#include <assert.h>
#include <atlcomcli.h>

using namespace std;


#include <D3D10_1.h> 
#include <d3dx10.h>


#include "../UISDK/Inc/inc.h"
#include "../common/define.h"
#pragma comment(lib, "UISDK.lib")
using namespace ui;
#include "common\math\rect.h"
#include "common\math\color.h"

#include "Inc\inc.h"

namespace ui
{
typedef struct _D3DCOLORVALUE {
	float r;
	float g;
	float b;
	float a;
} D3DCOLORVALUE;
}

#ifdef _DEBUG
#pragma comment(lib, "d3dx10d.lib") 
#else
#pragma comment(lib, "d3dx10.lib")
#endif
#pragma comment(lib, "D3D10_1.lib")
#pragma comment(lib, "d3d10.lib")

extern HINSTANCE g_hInstance;

#ifdef _DEBUG
#define ENABLE_TRACE  0
#else
#define ENABLE_TRACE  0
#endif