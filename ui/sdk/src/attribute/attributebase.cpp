#include "include/inc.h"
#include "attributebase.h"

namespace ui
{
AttributeBase::AttributeBase()
{
	/*m_pSerializer = nullptr;*/
	m_pUIApplication = nullptr;
	m_bData = false;
	m_bParentKeyIsNull = true;
	m_bReloadOnChanged = false;
}

void  AttributeBase::SetKey(const wchar_t* szKey)
{
	SETSTRING(m_strKey, szKey);
}
const wchar_t*  AttributeBase::GetKey()
{
	return m_strKey.c_str();
}
AttributeBase*  AttributeBase::SetCompatibleKey(const wchar_t* szKey)
{
	SETSTRING(m_strKeyCompatible, szKey);
	return this;
}
const wchar_t*  AttributeBase::GetCompatibleKey()
{
	return m_strKeyCompatible.c_str();
}
bool  AttributeBase::IsKey(const wchar_t* szKey)
{
	if (!szKey || !szKey[0])
		return false;
	if (m_strKey == szKey)
		return true;
	if (m_strKeyCompatible == szKey)
		return true;

	return false;
}

const wchar_t*  AttributeBase::GetDesc()
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

AttributeBase*  AttributeBase::SetParentKey(const wchar_t* szParent)
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
const wchar_t*  AttributeBase::GetParentKey()
{
	if (m_bParentKeyIsNull)
		return nullptr;
	return m_strParentKey.c_str();
}

void  AttributeBase::SetGroupName(const wchar_t* szGroupName)
{
	SETSTRING(m_strGroupName, szGroupName);
}
const wchar_t*  AttributeBase::GetGroupName()
{
	return m_strGroupName.c_str();
}
void  AttributeBase::SetUIApplication(Application* p)
{
	m_pUIApplication = p;
}
void  AttributeBase::SetSkinRes(ResBundle* p)
{
	m_pSkinRes = p;
}
Application*  AttributeBase::GetUIApplication()
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

		const wchar_t* szKey = GetKey();
		const wchar_t* szText = pData->pMapAttrib->GetAttr(nullptr, szKey, bEraseAttr);
		if (!szText)
		{
			const wchar_t* szCompatibleKey = GetCompatibleKey();
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
		const wchar_t* szKey = GetKey();
		const wchar_t* szText = pData->pMapAttrib->GetAttr(
			nullptr, szKey, bEraseAttr);

		if (!szText)
		{
			const wchar_t* szCompatibleKey = GetCompatibleKey();
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
	const wchar_t* szKey = GetKey();
	if (!szKey)
		return;

	const wchar_t* szValue = Get();
	if (szValue && !IsDefaultValue())
	{
		pData->pListAttrib->AddAttr(szKey, szValue);
	}
}


}