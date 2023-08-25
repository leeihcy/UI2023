#include "dpihelper.h"
#include "include/util/math.h"

namespace ui {

int GetDpi() {
  static int dpi = 0;
#if defined(OS_WIN)
  if (0 == dpi) {
    // 先检测用户是否禁用了DPI缩放
    bool bDisableScale = false;

    ATL::CRegKey reg;
    if (ERROR_SUCCESS ==
        reg.Open(HKEY_CURRENT_USER,
                 TEXT("Software\\Microsoft\\Windows "
                      "NT\\CurrentVersion\\AppCompatFlags\\Layers"),
                 KEY_READ)) {
      // REG_SZ
      wchar_t szExePath[MAX_PATH] = {0};
      GetModuleFileName(nullptr, szExePath, MAX_PATH - 1);

      wchar_t szValue[256] = {0};
      unsigned int count = 256;
      if (ERROR_SUCCESS == reg.QueryStringValue(szExePath, szValue, &count)) {
        if (_tcsstr(szValue, TEXT("HIGHDPIAWARE"))) {
          bDisableScale = true;
        }
      }
    }

    if (bDisableScale) {
      dpi = DEFAULT_SCREEN_DPI;
    } else {
      // 由程序自己实现DPI缩放，不用系统帮忙。
      HMODULE hModule = GetModuleHandle(L"User32.dll");
      if (hModule) {
        // SetProcessDPIAware();
        FARPROC f = GetProcAddress(hModule, "SetProcessDPIAware");
        if (f)
          f();
      }

      HDC hDC = GetDC(nullptr);
      dpi = GetDeviceCaps(hDC, LOGPIXELSY);
      ReleaseDC(nullptr, hDC);
    }
  }
#else
  dpi = DEFAULT_SCREEN_DPI;
#endif
  return dpi;
}

float GetDpiScale() {
  static bool bInit = false;
  static float fScale = 1;

  if (!bInit) {
    bInit = true;
    fScale = (float)GetDpi() / DEFAULT_SCREEN_DPI;
  }

  return fScale;
}

int ScaleByDpi(int x) {
  if (0 == x)
    return 0;

  if (GetDpi() == DEFAULT_SCREEN_DPI)
    return x;

  return _round(x * GetDpiScale());
}
int RestoreByDpi(int x) {
  if (0 == x)
    return 0;

  if (GetDpi() == DEFAULT_SCREEN_DPI)
    return x;

  return _round(x / GetDpiScale());
}

// 宽度、高度小于0时，如AUTO/NDEF不适应于dpi
int ScaleByDpi_if_gt0(int x) {
  if (x <= 0)
    return x;

  return ScaleByDpi(x);
}

int RestoreByDpi_if_gt0(int x) {
  if (x <= 0)
    return x;

  return RestoreByDpi(x);
}

} // namespace ui