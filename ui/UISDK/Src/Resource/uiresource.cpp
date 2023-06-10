#include "stdafx.h"
#include "uiresource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\iuires.h"

OutCtrlDllItem::OutCtrlDllItem() 
{
    hModule = NULL;
}

OutCtrlDllItem::~OutCtrlDllItem()
{
	if (hModule)
	{
		FreeLibrary(hModule);
		hModule = NULL;
	}
}


bool SkinBuilderRes::Register(OutCtrlDllItem* pItem)
{
	if (NULL == pItem)
		return false;

	if (pItem->hModule)
		return false;

	pItem->hModule = ::LoadLibrary(pItem->strDllPath.c_str());
	if (NULL == pItem->hModule)
	{
		UI_LOG_ERROR(_T("LoadLibrary Failed. DllPath=%s"), pItem->strDllPath.c_str());
		return false;
	}
	UI_LOG_DEBUG(_T("LoadLibrary To Register OutCtrl CreateInstanceFun: %s"), pItem->strDllPath.c_str());

	funRegisterUIObjectPtr pFunc = (funRegisterUIObjectPtr)::GetProcAddress(pItem->hModule, FUN_RegisterUIObject_NAME);
	if (NULL == pFunc)
	{
		UI_LOG_ERROR(_T("GetProcAddress Failed. DllPath=%s"), pItem->strDllPath.c_str());
		return false;
	}

	pFunc(m_pUIApp);
	return true;
}
void SkinBuilderRes::AddOuterCtrlsDllPath(const String& strDll)
{
	if (FindOuterCtrlsDll(strDll))
		return;

	OutCtrlDllItem* pItem = new OutCtrlDllItem;
	pItem->strDllPath = strDll;
	m_vecOuterCtrlsDll.push_back(pItem);

	this->Register(pItem);
}
OutCtrlDllItem*  SkinBuilderRes::FindOuterCtrlsDll(const String& strDll)
{
	vector<OutCtrlDllItem*>::iterator  iter = m_vecOuterCtrlsDll.begin();
	vector<OutCtrlDllItem*>::iterator  iterEnd = m_vecOuterCtrlsDll.end();
	for (; iter != iterEnd; iter++)
	{
		OutCtrlDllItem* p = *iter;
		if (p->strDllPath == strDll)
			return p;
	}
	return NULL;
}
OutCtrlDllItem*  SkinBuilderRes::FindOuterCtrlsDll(HMODULE hModule)
{
	vector<OutCtrlDllItem*>::iterator  iter = m_vecOuterCtrlsDll.begin();
	vector<OutCtrlDllItem*>::iterator  iterEnd = m_vecOuterCtrlsDll.end();
	for (; iter != iterEnd; iter++)
	{
		OutCtrlDllItem* p = *iter;
		if (p->hModule == hModule)
			return p;
	}
	return NULL;
}
void  SkinBuilderRes::Clear()
{
	vector<OutCtrlDllItem*>::iterator  iter = m_vecOuterCtrlsDll.begin();
	vector<OutCtrlDllItem*>::iterator  iterEnd = m_vecOuterCtrlsDll.end();
	for (; iter != iterEnd; iter++)
	{
		OutCtrlDllItem* p = *iter;
		delete p;
	}
	m_vecOuterCtrlsDll.clear();
}

SkinConfigRes::SkinConfigRes()
{
	memset(&m_sHlsInfo, 0, sizeof(SKIN_HLS_INFO)); 
}
void SkinConfigRes::SetHLS(char h, char l, char s, int nFlag)
{
	if (nFlag & CHANGE_SKIN_HLS_FLAG_H)
	{
		m_sHlsInfo.h = h;
	}

	if (nFlag & CHANGE_SKIN_HLS_FLAG_L)
	{
		m_sHlsInfo.l = l;
	}

	if (nFlag & CHANGE_SKIN_HLS_FLAG_S)
	{
		m_sHlsInfo.s = s;
	}

	m_sHlsInfo.nFlag |= nFlag;
}
