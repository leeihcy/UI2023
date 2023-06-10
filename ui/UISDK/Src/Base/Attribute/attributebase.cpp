#include "stdafx.h"
#include "attributebase.h"

AttributeBase::AttributeBase()
{
	/*m_pSerializer = nullptr;*/
	m_pUIApplication = nullptr;
	m_bData = false;
	m_bParentKeyIsNull = true;
	m_bReloadOnChanged = false;
}

void  AttributeBase::SetKey(LPCTSTR szKey)
{
	SETSTRING(m_strKey, szKey);
}
LPCTSTR  AttributeBase::GetKey()
{
	return m_strKey.c_str();
}
AttributeBase*  AttributeBase::SetCompatibleKey(LPCTSTR szKey)
{
	SETSTRING(m_strKeyCompatible, szKey);
	return this;
}
LPCTSTR  AttributeBase::GetCompatibleKey()
{
	return m_strKeyCompatible.c_str();
}
bool  AttributeBase::IsKey(LPCTSTR szKey)
{
	if (!szKey || !szKey[0])
		return false;
	if (m_strKey == szKey)
		return true;
	if (m_strKeyCompatible == szKey)
		return true;

	return false;
}

LPCTSTR  AttributeBase::GetDesc()
{
	return m_strDesc.c_str();
}

AttributeBase*  AttributeBase::AsData()
{
	m_bData = true;
	return this;
}
bool  AttributeBase::IsData()
{
	return m_bData;
}

AttributeBase*  AttributeBase::ReloadOnChanged()
{
	m_bReloadOnChanged = true;
	return this;
}
bool  AttributeBase::IsReloadOnChanged()
{
	return m_bReloadOnChanged;
}

AttributeBase*  AttributeBase::SetParentKey(LPCTSTR szParent)
{
	if (szParent)
	{
		m_strParentKey = szParent;
		m_bParentKeyIsNull = false;
	}
	else
	{
		m_strParentKey.clear();
		m_bParentKeyIsNull = true;
	}
	return this;
}
LPCTSTR  AttributeBase::GetParentKey()
{
	if (m_bParentKeyIsNull)
		return nullptr;
	return m_strParentKey.c_str();
}

void  AttributeBase::SetGroupName(LPCTSTR szGroupName)
{
	SETSTRING(m_strGroupName, szGroupName);
}
LPCTSTR  AttributeBase::GetGroupName()
{
	return m_strGroupName.c_str();
}
void  AttributeBase::SetUIApplication(UIApplication* p)
{
	m_pUIApplication = p;
}
void  AttributeBase::SetSkinRes(SkinRes* p)
{
	m_pSkinRes = p;
}
UIApplication*  AttributeBase::GetUIApplication()
{
	return m_pUIApplication;
}

void  AttributeBase::Load(SERIALIZEDATA* pData)
{
	bool bEraseAttr = pData->NeedErase();

	if (pData->IsReload())
	{
		if (IsData())
			return;

		LPCTSTR szKey = GetKey();
		LPCTSTR szText = pData->pMapAttrib->GetAttr(nullptr, szKey, bEraseAttr);
		if (!szText)
		{
			LPCTSTR szCompatibleKey = GetCompatibleKey();
			if (szCompatibleKey && szCompatibleKey[0])
				szText = pData->pMapAttrib->GetAttr(nullptr, szCompatibleKey, bEraseAttr);
		}

		if (szText)
		{
			Set(szText);
		}
		// 没有配置该属性，重置。
		// 会也调用到setter，让控件去释放一些资源，如renderbase。
		// 这样就不再需要UI_WM_RESETEATTRIBUTE
		else if (!IsDefaultValue())
		{
			Reset();
		}
	}
	else
	{
		LPCTSTR szKey = GetKey();
		LPCTSTR szText = pData->pMapAttrib->GetAttr(
			nullptr, szKey, bEraseAttr);

		if (!szText)
		{
			LPCTSTR szCompatibleKey = GetCompatibleKey();
			if (szCompatibleKey && szCompatibleKey[0])
				szText = pData->pMapAttrib->GetAttr(nullptr, szCompatibleKey, bEraseAttr);
		}

		if (szText)
		{
			Set(szText);
		}
		else
		{
			// 增加该代码，以防变量在构造函数中的赋值与在属性的默认值
			// 设置上不一样，导致变量序列化失败
			Reset();
		}
	}
}

void  AttributeBase::Save(SERIALIZEDATA* pData)
{
	LPCTSTR szKey = GetKey();
	if (!szKey)
		return;

	LPCTSTR szValue = Get();
	if (szValue && !IsDefaultValue())
	{
		pData->pListAttrib->AddAttr(szKey, szValue);
	}
}

