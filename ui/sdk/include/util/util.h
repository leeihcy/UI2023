#ifndef _UI_UTIL_H_
#define _UI_UTIL_H_
#include "sdk/include/interface.h"
#include "sdk/include/util/windows.h"
#include "sdk/include/util/color.h"

namespace ui {
class C9Region;
struct Rect;
struct Size;
struct IRenderTarget;

class CTimeout;
struct ITimeoutCallback {
  virtual void OnTimeout(long lId, long wParam, long lParam) = 0;
};

namespace util {
bool UIAPI IsKeyDown(unsigned int vk);

void UIAPI DeflatRect(Rect *pfc, const Rect *pDeflatRc);

struct ISplitStringEnum {
  virtual void Release() = 0;
  virtual const char *GetText(unsigned int nIndex) = 0;
  virtual int GetCount() = 0;
};
unsigned int UIAPI SplitString(const char *szText, char szSep,
                               ISplitStringEnum **pEnum);

struct ISplitStringEnumW {
  virtual void Release() = 0;
  virtual const wchar_t *GetText(unsigned int nIndex) = 0;
  virtual int GetCount() = 0;
};
unsigned int UIAPI SplitStringW(const wchar_t *szText, wchar_t szSep,
                               ISplitStringEnumW **pEnum);

bool IsFullPath(const char *szPath);
bool GetPathFileName(const char *szPath, char *szOutDir);
bool GetPathFileExt(const char *szPath, char *szOutExt);

bool UIAPI IsWin7OrLater();
bool UIAPI IsWin8OrLater();

class UIAPI CTimeoutWrap {
public:
  CTimeoutWrap();
  ~CTimeoutWrap();

  void Start(int nElapse);
  void Cancel();
  void Init(ITimeoutCallback *pCallback);
  void SetParam(long lId, long wParam = 0, long lParam = 0);

private:
  CTimeout *m_Impl;
};

enum FixAlphaMode {
  // 直接将alpha设置为255
  SET_ALPHA_255,

  // 如果当前alpha==0，将alpha设置为255
  SET_ALPHA_255_IF_ALPHA_IS_0,

  // 如果当前像素不是0，则将alpha设置为255
  SET_ALPHA_255_IF_RGBA_ISNOT_0,

  // alpha 反转 0->255, 255->0
  SET_ALPHA_INVERSE_0_255,

  // 将alpha设置为指定值wParam
  SET_ALPHA_VALUE,

  // 如果当前alpha值是255, 将alpha设置为指定值wParam
  // 但是没有值预乘处理
  SET_ALPHA_VALUE_IF_ALPHA_IS_255

  // ...
  // 如果当前alpha==wParam，将alpha设置为lParam
  //	SET_ALPHA_IF_ALPHA_EQUAL,

  // 如果当前alpha<wParam，将alpha设置为lParam
  //	SET_ALPHA_IF_ALPHA_LESS,
};
struct FixAlphaData {
  // 当仅指定了hDC，没有指定hBitmap时，从hDC中获取CurrentBitap
  HDC hDC;
  HBITMAP hBitmap;
  bool bTopDownDib;
  Rect *lprc;
  FixAlphaMode eMode;
  long wParam;
  long lParam;
};
bool UIAPI FixBitmapAlpha(FixAlphaData *pData);
bool UIAPI FixRenderTargetClipRegionAlpha(IRenderTarget *);

void UIAPI PathInBin(const char *szRelative, char szAbsolution[MAX_PATH]);

bool PathIsDirectory(const char *path);
bool PathFileExists(const char *path);
int wtoi(const wchar_t *text);
} // namespace util
} // namespace ui

#endif // _UI_UTIL_H_