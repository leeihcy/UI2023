#include "stdafx.h"
#include "versionhelper.h"
#include "Inc/Interface/iuiapplication.h"

int  UISDKVersion::GetMajor()
{
	return UISDK_VERSION_MAJOR;
}
int  UISDKVersion::GetMinor()
{
	return UISDK_VERSION_MINOR;
}
int  UISDKVersion::GetPatch()
{
	return UISDK_VERSION_PATCH;
}
int  UISDKVersion::GetVersionText(TCHAR* szText, int nTextSize)
{
	TCHAR szTemp[32] = {0};
	wprintf(szTemp, TEXT("%d.%d.%d"), GetMajor(), GetMinor(), GetPatch());

	int nDesiredLen = (int)wcslen(szTemp)+1;
	if (nTextSize < nDesiredLen)
		return nDesiredLen;

	_tcscpy(szText, szTemp);
	return nDesiredLen;
}