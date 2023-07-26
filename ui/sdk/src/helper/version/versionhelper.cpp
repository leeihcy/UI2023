#include "versionhelper.h"
#include "include/interface/iuiapplication.h"

namespace ui {
int SDKVersion::GetMajor() { return UISDK_VERSION_MAJOR; }
int SDKVersion::GetMinor() { return UISDK_VERSION_MINOR; }
int SDKVersion::GetPatch() { return UISDK_VERSION_PATCH; }
int SDKVersion::GetVersionText(char *szText, int nTextSize) {
  char szTemp[32] = {0};
  sprintf(szTemp, "%d.%d.%d", GetMajor(), GetMinor(), GetPatch());

  int nDesiredLen = (int)strlen(szTemp) + 1;
  if (nTextSize < nDesiredLen)
    return nDesiredLen;

  strcpy(szText, szTemp);
  return nDesiredLen;
}
}