// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4996)
#pragma warning(disable:4302)
#pragma warning(disable:4838)

// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <xstring>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <functional>
#include <algorithm>
using namespace std;

#include <atlbase.h>
// #include <Src\Atl\atlstdthunk.h> <-- 会崩溃，先不用了
#include <../3rd/wtl90/Include/atlapp.h>
#include <../3rd/wtl90/Include/atlcrack.h>

#define _WTL_NO_CSTRING
#include <../3rd/wtl90/Include/atlmisc.h>
using namespace WTL;

#include "Inc/inc.h"
#include "Inc/private_inc.h"
using namespace UI;

extern HINSTANCE g_hInstance;