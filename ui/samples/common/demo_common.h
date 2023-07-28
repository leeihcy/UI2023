#pragma once
#include <shlwapi.h>
#pragma warning(disable:4996)

void  Demo_GetAppDir(__out TCHAR* szDir)
{
	GetModuleFileName(GetModuleHandle(NULL), szDir, MAX_PATH);
	_tcsrchr(szDir, '\\')[1] = 0;
}

void  Demo_SetDllDir(__in LPCTSTR szAppDir)
{
	TCHAR szUIDllPath[MAX_PATH] = {0};
	TCHAR szUIDllPath2[MAX_PATH] = {0};
	_tcscpy(szUIDllPath, szAppDir);
	_tcscat(szUIDllPath, TEXT("..\\..\\UI"));

	PathCanonicalize(szUIDllPath2, szUIDllPath);
	SetDllDirectory(szUIDllPath2);
}

void  Demo_Log(UI::IApplication* pUIApp, __in LPCTSTR szAppDir)
{
	TCHAR szLogXml[MAX_PATH];
	_tcscpy(szLogXml, szAppDir);
	_tcscat(szLogXml, TEXT("uilog.xml"));
	//pUIApp->LogUI(szLogXml);
}

UI::IResBundle*  Demo_LoadSkin(
		UI::IApplication* pUIApp, 
		__in LPCTSTR szAppDir)
{
	TCHAR szSkinPath[MAX_PATH];
	_tcscpy(szSkinPath, szAppDir);
	_tcscat(szSkinPath, TEXT("Default"));
	return pUIApp->LoadResBundle(szSkinPath);
}