#include "stdafx.h"
#include "object.h"
#include "object_layer.h"

#include "Inc\Interface\iwindow.h"
#include "Inc\Interface\ilayout.h"
#include "Inc\Interface\ipanel.h"
#include "Src\Resource\uiresource.h"
#include "Src\Renderbase\renderbase\renderbase.h"
#include "Src\Renderbase\textrenderbase\textrender.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Src\UIObject\HwndHost\HwndHost.h"
#include "Src\Resource\uicursor.h"
#include "Src\Helper\layout\layout.h"
#include "Src\Base\Application\uiapplication.h"
#include "Inc\Interface\ixmlwrap.h"
#include "Inc\Interface\iuires.h"
#include "Src\Resource\cursorres.h"
#include "Src\Resource\stylemanager.h"
#include "Src\Resource\skinres.h"
#include "Src\Resource\colorres.h"
#include "Src\Atl\image.h"
#include "Inc\Interface\imapattr.h"
#include <algorithm>
#include "..\Accessible\accessibleimpl.h"
#include "..\Accessible\object_accessible.h"
#include "object_desc.h"


using namespace UI;


Object::Object(IObject* p) : ObjTree(p), m_objLayer(*this)
{
    m_pIObject = p;
	::SetRectEmpty(&m_rcParent);
	::SetRectEmpty(&m_rcExtNonClient);
	::SetRectEmpty(&m_rcMargin);
	::SetRectEmpty(&m_rcPadding);
	::SetRectEmpty(&m_rcBorder);

	// m_lCanRedrawRef = 0;
	m_pSkinRes = nullptr;
	m_ppOutRef = nullptr;
    m_lzOrder = 0;

	m_nMaxWidth = NDEF;
	m_nMaxHeight = NDEF;
	m_lMinWidth = m_lMinHeight = NDEF;

	m_pBkgndRender = nullptr;
	m_pForegndRender = nullptr;
	m_pTextRender = nullptr;

    m_pIMapAttributeRemain = nullptr;
    m_pLayoutParam = nullptr;
	m_pAccessible = nullptr;
    m_pDescription = UndefineDescription::Get();

	memset(&m_objStyle, 0, sizeof(m_objStyle));
	memset(&m_objState, 0, sizeof(m_objState));
	m_objState.visibility_ = VISIBILITY_VISIBLE;
}

 // ע�⣺��Ҫ�ڹ���������������е����麯��

Object::~Object(void)
{
	if (m_ppOutRef)
		*m_ppOutRef = nullptr;

    SAFE_RELEASE(m_pIMapAttributeRemain);
    SAFE_RELEASE(m_pLayoutParam);
	if (m_pAccessible)
		SAFE_RELEASE(m_pAccessible);
}

HRESULT Object::FinalConstruct(ISkinRes* pSkinRes)
{
	m_pSkinRes = pSkinRes->GetImpl();
    return S_OK;
}

void Object::FinalRelease()
{
	// ��ֹ������;���߻��������У�һЩ����ͻȻ�����٣����µ�Ұָ�����
    // ������DestroyChildObjectǰ���ã�����parent��ϵ�����
    IMKMgr*  pMKMgr = this->GetIMKMgr();
    if (pMKMgr)
    {
        pMKMgr->OnObjectRemoveInd(this);
    }

	this->DestroyChildObject();  // ��ɾ���Ӷ���

	IUIEditor* pEditor = GetUIApplication()->GetUIEditorPtr();
	if (pEditor)
	{
		// ������������֪ͨ�����ڴ�skinbuilder��tree��һ����ɾ��
		pEditor->OnObjectDeleteInd(m_pIObject);
	}

	//	�����Լ����ھӹ�ϵ
	RemoveMeInTheTree();

	SAFE_RELEASE(m_pBkgndRender);
	SAFE_RELEASE(m_pForegndRender);
	SAFE_RELEASE(m_pTextRender);

	// ������֮ǰ���٣����ⴰ�ڵ�window compositor�Ѿ������ˣ���layer��û�б�����
	m_objLayer.DestroyLayer();
}

IObject*  Object::GetIObject() 
{ 
    return m_pIObject; 
}

// ע�����������ģ��ֱ�ӵ��� pCtrl->m_strID=L"..."�Ļ����ڶ����ͷ�ʱ�������
void Object::SetId(LPCTSTR szText)
{
	if (szText)
		m_strId = szText;
	else
		m_strId.clear();
}
LPCTSTR  Object::GetId() 
{
    return m_strId.c_str(); 
}

Layer* Object::GetSelfLayer() const
{
	return m_objLayer.GetLayer();
}

// ��ȡ��������Layer
Layer*  Object::GetLayer()
{
	ObjectLayer*  pLayer = GetLayerEx();
    if (pLayer)
        return pLayer->GetLayer();

    return nullptr;
}

// Ϊ����׼��һ��layer�����û���򴴽�layer
Layer*  Object::GetLayerForAnimate()
{
	Layer* layer = GetSelfLayer();
	if (layer)
		return layer;

	m_objLayer.CreateLayer();
	return GetSelfLayer();
}

ObjectLayer*  Object::GetLayerEx()
{
    Object* pObj = this;
    while (pObj)
    {
        if (pObj->GetSelfLayer())
            return &pObj->m_objLayer;

        pObj = pObj->m_pParent;
    }

    return nullptr;
}

// ��ȡ�Լ����ڲ�Ĳ����
Object*  Object::GetLayerCreator()
{
    ObjectLayer*  p = GetLayerEx();
    if (p)
        return &p->GetObjet();

    return nullptr;
}


// ��ȡһ���ؼ����ڴ��ھ��
WindowBase* Object::GetWindowObject()
{
    Object*  pParent = this;
    do 
    {
        if (!pParent->m_pParent)
            break;
        pParent = pParent->m_pParent;
    } 
    while (1);

    IWindowBase*  pWindowBase = (IWindowBase*)pParent->
			GetIObject()->QueryInterface(__uuidof(IWindowBase));

    if (pWindowBase)
        return pWindowBase->GetImpl();
    return nullptr;
}

HWND Object::GetHWND()
{
	WindowBase* pWindow = this->GetWindowObject();
	if (!pWindow)
		return nullptr;

	return pWindow->GetHWND();
}

// TODO: �������޸�Ϊfind decendant object
//
//	�����е����������в���idΪobjId�Ķ���ֻ���ص�һ���ҵ��Ķ���
//
//	Parameter
//		objId
//			[in]	Ҫ���ҵĶ���id
//
//	Return
//		�ɹ����ض����ַ��ʧ�ܷ���nullptr
//
Object*  Object::FindObject(LPCTSTR szObjId)
{
    if (!szObjId)
        return nullptr;

	Object* pRet = this->find_child_object(szObjId, true);
	if (!pRet)
	{
		UI_LOG_WARN(_T("Find \"%s\" from \"%s\" failed."), szObjId, this->m_strId.c_str() );
        UIASSERT(0);
	}
	return pRet;
}

// ��д��־
Object*  Object::TryFindObject(LPCTSTR szObjId)
{
    if (!szObjId)
        return nullptr;

    Object* pRet = this->find_child_object(szObjId, true);
    return pRet;
}

// û�еݹ�
Object*  Object::FindNcObject(LPCTSTR szobjId)
{
	if (nullptr == szobjId)
		return nullptr;

	Object* pObjChild = nullptr;
	while (pObjChild = this->EnumNcChildObject(pObjChild))
	{
		if (0 == _tcscmp(pObjChild->GetId(), szobjId))
		{
			return pObjChild;
		}
	}	
	return nullptr;
}

Object*  Object::FindObject(UUID uuid)
{
    return this->find_child_object(uuid, true);
}
Object* Object::FindNcObject(UUID uuid)
{
	return this->find_ncchild_object(uuid, true);
}


Object* Object::find_child_object(LPCTSTR szobjId, bool bFindDecendant)
{
    if (nullptr == szobjId)
        return nullptr;

	Object* pObjChild = nullptr;
	while (pObjChild = this->EnumChildObject(pObjChild))
	{
		if (0 == _tcscmp(pObjChild->GetId(), szobjId))
        {
			return pObjChild;
        }
	}	

	if (!bFindDecendant)
		return nullptr;

	pObjChild = nullptr;
	while (pObjChild = this->EnumChildObject(pObjChild))
	{
		Object* pRet = ((Object*)pObjChild)->find_child_object(szobjId, bFindDecendant);
		if (pRet )
			return pRet;
	}	
	return nullptr;
}


Object* Object::find_child_object(UUID uuid, bool bFindDecendant)
{
    Object* pObjChild = nullptr;
    while (pObjChild = this->EnumChildObject(pObjChild))
    {
        IObjectDescription* pDesc = pObjChild->GetDescription();
        if (IsEqualGUID(pDesc->GetGUID(), uuid))
        {
            return pObjChild;
        }
    }

    if (!bFindDecendant)
        return nullptr;

    pObjChild = nullptr;
    while (pObjChild = this->EnumChildObject(pObjChild))
    {
        Object* pRet = ((Object*)pObjChild)->
            find_child_object(uuid, bFindDecendant);
        if (pRet)
            return pRet;
    }
    return nullptr;
}


Object* Object::find_ncchild_object(UUID uuid, bool bFindDecendant)
{
	Object* pObjChild = nullptr;
	while (pObjChild = this->EnumNcChildObject(pObjChild))
	{
		IObjectDescription* pDesc = pObjChild->GetDescription();
		if (IsEqualGUID(pDesc->GetGUID(), uuid))
		{
			return pObjChild;
		}
	}

	if (!bFindDecendant)
		return nullptr;

	pObjChild = nullptr;
	while (pObjChild = this->EnumNcChildObject(pObjChild))
	{
		Object* pRet = ((Object*)pObjChild)->
			find_ncchild_object(uuid, bFindDecendant);
		if (pRet)
			return pRet;
	}
	return nullptr;
}


void  Object::LoadAttributeFromMap(IMapAttribute* pMapAttrib, bool bReload)
{
    if (!pMapAttrib)
        return;

    String strStyle;
    String strId;
    
    LPCTSTR szText = pMapAttrib->GetAttr(XML_STYLECLASS, false);
    if (szText)
        strStyle = szText;
    
    szText = pMapAttrib->GetAttr(XML_ID, false);
    if (szText)
        strId = szText;

    StyleRes& styleRes = m_pSkinRes->GetStyleRes();
    styleRes.LoadStyle(
                m_pDescription->GetTagName(),
                strStyle.c_str(),
                strId.c_str(),
                pMapAttrib);

    SERIALIZEDATA data = { 0 };
    data.pUIApplication = GetIUIApplication();
	data.pSkinRes = m_pSkinRes->GetISkinRes();
	data.pMapAttrib = pMapAttrib;
	data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
	if (bReload)
		data.nFlags |= SERIALIZEFLAG_RELOAD;
	

	// �������ԣ�������չ��
	SAFE_RELEASE(m_pIMapAttributeRemain);
	m_pIMapAttributeRemain = pMapAttrib;
	if (m_pIMapAttributeRemain)
		m_pIMapAttributeRemain->AddRef();

	UISendMessage(m_pIObject, UI_MSG_SERIALIZE, (WPARAM)&data);

	// ���û�ж�������ԣ�ֱ���ͷţ���ʡ�ڴ�
	if (m_pIMapAttributeRemain && 0 == m_pIMapAttributeRemain->GetAttrCount())
	{
		SAFE_RELEASE(m_pIMapAttributeRemain);
	}
	else
	{
		int a = 0;
	}
}

// ��xml��ȡ�Լ�������
void  Object::LoadAttributeFromXml(UIElement* pElement, bool bReload)
{
	if (!pElement)
		return;

	IMapAttribute*  pMapAttrib = nullptr;
	pElement->GetAttribList(&pMapAttrib);
	{
		this->LoadAttributeFromMap(pMapAttrib, bReload);
	}
	SAFE_RELEASE(pMapAttrib);

	// ֪ͨ�༭�������ؼ���xml���.
	// ��֪ͨ�������������layoutmanager�У���Ϊ�˽�����Ͽؼ��е��ӿؼ���������
	// �����ӿؼ�������layoutmanager�б��������ԣ������ɸ��Ͽؼ��Լ���������յ���
	// ���ӿؼ���SetAttributeFromXmlElement��������˽�֪ͨ�������
	IUIEditor* pEditor = GetUIApplication()->GetUIEditorPtr();
	if (pEditor)
	{
		pEditor->OnObjectAttributeLoad(m_pIObject, pElement->GetIUIElement());
	}
}

// ��ȡһ��δ���������ԡ����bErase==true���򽫷���һ����ʱ���ַ���ָ�룬������Ӧ�þ��챣����߽���ʱʹ��
LPCTSTR  Object::GetAttribute(LPCTSTR szKey, bool bErase)
{
    if (nullptr == szKey || nullptr == m_pIMapAttributeRemain)
        return nullptr;

    return m_pIMapAttributeRemain->GetAttr(szKey, bErase);
}
void  Object::AddAttribute(LPCTSTR szKey, LPCTSTR  szValue)
{
    if (nullptr == m_pIMapAttributeRemain) 
    {
        UICreateIMapAttribute(&m_pIMapAttributeRemain);
    }
    m_pIMapAttributeRemain->AddAttr(szKey, szValue);

}
void  Object::GetMapAttribute(IMapAttribute** ppMapAttribute)
{
    if (ppMapAttribute && m_pIMapAttributeRemain)
    {
        *ppMapAttribute = m_pIMapAttributeRemain;
        m_pIMapAttributeRemain->AddRef();
    }
}
void  Object::ClearMapAttribute()
{
	if (m_pIMapAttributeRemain)
	{
		m_pIMapAttributeRemain->Release();
		m_pIMapAttributeRemain = nullptr;
	}
}


// ����padding��ֵ��ͬʱ���·ǿͻ����Ĵ�С
void Object::SetPaddingRegion(REGION4* prc)
{
	m_rcPadding.CopyRect(prc);
}


void Object::SetBorderRegion(RECT* prc)
{
	m_rcBorder.CopyRect(prc);
}

// void*  Object::QueryInterface(const IID* pIID)
// {
//     if (IsEqualIID(*pIID, uiiidof(IObject)) ||
//         IsEqualIID(*pIID, uiiidof(IMessage)) )
//     {
//         return m_pIObject;
//     }
// 
//     return nullptr;
// }
//
// 
// void  Object::OnCreateByEditor(CREATEBYEDITORDATA* pData)
// {
//     // �ڱ༭���д����Ķ���Ҳ������һ���������б��������л�Ĭ�ϵ�TEXTSTYLE�ȡ�
//     assert(nullptr == m_pIMapAttributeRemain);
//     UICreateIMapAttribute(&m_pIMapAttributeRemain);
// }
//

// TODO: �Ż�
//
//	����Object��Ĭ�������ʽ
//
//	Remark:
//		�����ʵ�ָ���Ϣ�����ܵ�������ƶ���һ��Object����ʱ����Ȼ��ʾ����Window��OnSetCursor
//
// 
// BOOL Object::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
// {
// 	if (nullptr == m_pCursor || nullptr == m_pCursor->GetCursor())
// 	{
// 		::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
//  		return TRUE;
// 	}
// 
// 	::SetCursor(m_pCursor->GetCursor());
// 	return TRUE;
// }

//	��ȡ�Լ�������,���ﷵ�صĶ���ֻ��һ����ʱ����
//	�����Ҫ����ʹ�ã�����Ҫ����AddRef
// IRenderFont* Object::GetRenderFont()
// {
//     IRenderFont* pRenderFont = nullptr;
// 
//     // ���Լ���textrender��ȡ
//     if (m_pTextRender)
//         pRenderFont = m_pTextRender->GetRenderFont();
// 
//     if (pRenderFont)
//         return pRenderFont;
// 
//     // �򴰿ڻ�ȡĬ��
//     WindowBase* pWindow = GetWindowObject();
//     if (pWindow)
//         return pWindow->GetWindowDefaultRenderFont();
// 
//     return nullptr;
// }

//
//	[public] [virtual]  UINT HitTest( POINT* ptInParent )
//
//	Parameters
//		ptInParent
//			[in]  Ҫ��̽��λ�ã�ͨ��Ϊ��굱ǰλ��
//
//	Return
//		��겻�ڶ������о�Χ�ڷ���HTNOWHERE�����򷵻�HTCLIENT�����������������չ��
//
//	Remark
//		����������������һ�����⣬��ΪObject�䱸һ��RECT�����ʾ��Χ�����Ǹ�һ����ǿ���RGN��ʾ��Χ?
//		����RGN���������ڴ棬����ʱ��͵�ʹ��RGN��ʵ��ĳЩ���ܣ������������������������ߵ����ߣ�
//		�����λ�ô�����ʲô��λ��ͬʱ��Object�䱸һ��RECT������ʾÿһ������ķ�Χ����Ϊ�󲿷�ʱ��ʹ�õĶ���RECT����
//

UINT Object::OnHitTest(POINT* ptInParent, __out POINT* ptInChild)
{
	BOOL  bIn = FALSE;

// 	if (this->m_hRgn)
// 	{
//      UIASSERT(0);  // δʵ��
// 		bIn = ::PtInRegion(m_hRgn, ptInParent->x, ptInParent->y);
//      if (bIn)
// 		    return HTCLIENT;
// 	}

	Layer* layer = GetSelfLayer();
    if (layer)
    {
		POINT ptObj = 
		{
			ptInParent->x-m_rcParent.left, 
			ptInParent->y-m_rcParent.top
		};

		layer->MapView2Layer(&ptObj);

		RECT rcObj = {0, 0, m_rcParent.Width(), m_rcParent.Height()};
		if (PtInRect(&rcObj, ptObj))
		{
			if (ptInChild)
				*ptInChild = ptObj;

			return HTCLIENT;
		}
    }
	else
	{
		bIn = ::PtInRect(&m_rcParent, *ptInParent);
		if (bIn)
		{
			if (ptInChild)
			{
				ptInChild->x = ptInParent->x - m_rcParent.left;
				ptInChild->y = ptInParent->y - m_rcParent.top;
			}

			return HTOBJECT;
		}
	}

	return HTNOWHERE;
}

// void Object::OnThemeChanged()
// {
// 	SetMsgHandled(FALSE);
//     
//     if (m_pBkgndRender)
//         m_pBkgndRender->CheckThemeChanged();
//     if (m_pForegndRender)
//         m_pForegndRender->CheckThemeChanged();
// }
// 
// LRESULT  Object::OnSkinTextureChanged(UINT, WPARAM, LPARAM)
// {
//     if (m_pTextRender)
//     {
//         m_pTextRender->CheckSkinTextureChanged();
//     }
//     return 0;
// }


IMKMgr*  Object::GetIMKMgr()
{
    IMKMgr*  pMKMgr = nullptr;

    Object*  pParent = m_pParent;
    while (pParent)
    {
		pMKMgr = pParent->virtualGetIMKMgr();;
        if (pMKMgr)
           return pMKMgr;

        pParent = pParent->m_pParent;
    }
    return nullptr;
}

void  Object::OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
{
//     if (!bVisible)
//     {
//         // ��ֹ������;���߻��������У�һЩ����ͻȻ�����٣����µ�Ұָ�����
//         WindowBase* pWindow = GetWindowObject();
//         if (pWindow)  // ������DestroyChildObjectǰ���ã�����parent��ϵ�����
//         {
//             pWindow->OnObjectHideInd(this);
//         }
//     }

    // ������صĶ�����һ����������򽫽��������лص���һ������
    // ע�������objectҲ������λ��һ��listctrl�У������Ӧ��mouse/keyboard
    //     manager�����Ǵ��ڵģ�����listctrl�Լ���

    IMKMgr*  pMKMgr = GetIMKMgr();
    if (pMKMgr)
    {
        pMKMgr->OnObjectVisibleChangeInd(this, bVisible?true:false);
    }
}

void  Object::ModifyObjectStyle(OBJSTYLE* add, OBJSTYLE* remove)
{

#define __REMOVE(x) \
    if (remove->x) m_objStyle.x = 0;
#define __ADD(x) \
	if (add->x) m_objStyle.x = 1;

	if (add)
	{
		__ADD(initialized);
		__ADD(transparent);
		__ADD(float_on_parent_content);
		__ADD(post_paint);
		__ADD(hscroll);
		__ADD(vscroll);
		__ADD(ncobject);
		__ADD(reject_all_mouse_msg);
		__ADD(reject_self_mouse_msg);
		__ADD(receive_dragdrop_event);
		__ADD(enable_ime);
		__ADD(zindex_overlap);
		__ADD(noclip);
		__ADD(clip_client);
		__ADD(tabstop);

		if (add->layer)
			m_objLayer.CreateLayer();

		// Ĭ��ֵΪ1ʱ�����û����xml�����ã����ᴥ��setter����
		// ���������Ĭ��ֵ��ʱ��Ӧ��ͬ��һ�θ�ֵ
		if (add->default_ncobject)
		{
			m_objStyle.default_ncobject = 1;
			m_objStyle.ncobject = 1;
		}
		if (add->default_reject_all_mouse_msg)
		{
			m_objStyle.default_reject_all_mouse_msg = 1;
			m_objStyle.reject_all_mouse_msg = 1;
		}
		if (add->default_reject_self_mouse_msg)
		{
			m_objStyle.default_reject_self_mouse_msg = 1;
			m_objStyle.reject_self_mouse_msg = 1;
		}
		if (add->default_transparent)
		{
			m_objStyle.default_transparent = 1;
			m_objStyle.transparent = 1;
		}
		if (add->default_tabstop)
		{
			m_objStyle.default_tabstop = 1;
			m_objStyle.tabstop = 1;
		}
	}

	if (remove)
	{
		__REMOVE(initialized);
		__REMOVE(transparent);
		__REMOVE(float_on_parent_content);
		__REMOVE(post_paint);
		__REMOVE(hscroll);
		__REMOVE(vscroll);
		__REMOVE(ncobject);
		__REMOVE(reject_all_mouse_msg);
		__REMOVE(reject_self_mouse_msg);
		__REMOVE(receive_dragdrop_event);
		__REMOVE(enable_ime);
		__REMOVE(zindex_overlap);
		__REMOVE(noclip);
		__REMOVE(clip_client);
		__REMOVE(tabstop);
		if (remove->layer)
			m_objLayer.TryDestroyLayer();

		if (remove->default_ncobject)
		{
			m_objStyle.default_ncobject = 0;
			m_objStyle.ncobject = 0;
		}
		if (remove->default_reject_all_mouse_msg)
		{
			m_objStyle.default_reject_all_mouse_msg = 0;
			m_objStyle.reject_all_mouse_msg = 0;
		}
		if (remove->default_reject_self_mouse_msg)
		{
			m_objStyle.default_reject_self_mouse_msg = 0;
			m_objStyle.reject_self_mouse_msg = 0;
		}
		if (remove->default_transparent)
		{
			m_objStyle.default_transparent = 0;
			m_objStyle.transparent = 0;
		}
		if (remove->default_tabstop)
		{
			m_objStyle.default_tabstop = 0;
			m_objStyle.tabstop = 0;
		}
	}
}

bool Object::TestObjectStyle(const OBJSTYLE& test)
{

#define TEST(x) \
    if (test.x && !m_objStyle.x) return false; 

    TEST(transparent);
	TEST(float_on_parent_content);
    TEST(post_paint);
	TEST(hscroll);
	TEST(vscroll);
	TEST(ncobject);
	TEST(reject_all_mouse_msg);
	TEST(reject_self_mouse_msg);
	TEST(receive_dragdrop_event);
	TEST(enable_ime);
	TEST(zindex_overlap);
	TEST(noclip);
    TEST(clip_client);
	TEST(tabstop);
	TEST(layer);

    return true;
}

// void Object::ModifyStyleEx(UINT nStyleAdd, UINT nStyleRemove, bool bNotify)
// {
//     STYLESTRUCT s;
//     s.styleOld = m_nStyle2;
//     s.styleNew = m_nStyle2;
// 
//     if (nStyleAdd != 0)
//     {
//         s.styleNew |= nStyleAdd;
//     }
//     if (nStyleRemove != 0)
//     {
//         s.styleNew &= ~nStyleRemove;
//     }
// 
//     if (s.styleNew == s.styleOld)
//         return;
// 
//     if (bNotify)
//     {
//         ::UISendMessage(this, WM_STYLECHANGING, (WPARAM)GWL_EXSTYLE, (LPARAM)&s);
//     }
// 
//     m_nStyle2 = s.styleNew;
// 
//     if (bNotify)
//     {
//         s.styleOld = s.styleOld;
//         ::UISendMessage(this, WM_STYLECHANGED, (WPARAM)GWL_EXSTYLE, (LPARAM)&s);
//     }
// }

bool Object::IsTransparent()
{
	if (m_objStyle.transparent)
        return true;

    return false;
}
void  Object::SetTransparent(bool b)
{
	m_objStyle.transparent = b;
}

bool  Object::IsNoClip()
{
	return !NeedClip();
}

bool  Object::NeedClip()
{
	if (m_objStyle.noclip)
		return false;

//  TODO: ��Ҫ��д��
// 	// ����Ӷ�����һ���㣬�����б任���򲻸��¼������򣬶����л���������ļ�������
// 	// 2015.9.23 add �����Ӳ���ϳ�ģʽ�����ùܣ������֮�������û�й�����
// 	if (m_pRenderLayer && m_pRenderLayer->HasTransform())
// 	{
// 		WindowBase* pWindow = GetWindowObject();
// 		if (!pWindow || !pWindow->IsGpuComposite())
// 			return false;
// 	}
// 
	return true;
}

void  Object::SetNoClip(bool b)
{
	m_objStyle.noclip = b;
}

void  Object::SetClipClient(bool b)
{
    m_objStyle.clip_client = b;
}
bool  Object::NeedClipClient()
{
    return m_objStyle.clip_client;
}

bool Object::IsFocus()
{
	return m_objState.focus;
}


void Object::SetFocus(bool b, bool bNoitfy)
{
    if (m_objState.focus == b)
        return;

    m_objState.focus = b;
    if (bNoitfy)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_FOCUS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

// ע���ؼ��п���λ���б�����(listitem)����������Ŀؼ����ý��㲻Ӧ������listtrl�Ľ���
bool  Object::SetFocusInWindow()
{
    IMKMgr*  pMKMgr = GetIMKMgr();
    if (!pMKMgr)
        return false;

    pMKMgr->SetFocusObject(this);

// 	WindowBase* pWnd = GetWindowObject();
// 	if (!pWnd)
// 		return false;
// 
// 	pWnd->SetFocusObject(this);
	return true;
}


bool Object::IsTabstop()
{
	return m_objStyle.tabstop;
}

bool Object::CanTabstop()
{
	return this->IsTabstop() && this->IsEnable() && this->IsVisible();
}

void Object::SetTabstop(bool b)
{
	m_objStyle.tabstop = b;
}

bool Object::IsSelfVisible()
{
	bool bVisible = m_objState.visibility_ == VISIBILITY_VISIBLE?true:false;
	return bVisible;
}

bool Object::IsVisible()
{
	bool bVisible = m_objState.visibility_ == VISIBILITY_VISIBLE?true:false;
	if (false == bVisible || nullptr == m_pParent)
	{
		return bVisible;
	}
	// �жϸ����󲻿ɼ������ 
	else
	{
		// ע�������ﲻ�������Ĵ��ڽ����жϵ�ԭ���ǣ��������ڴ��ڳ�ʼ���ĺ������棬
		// ��Ȼ������ʱ�ǲ��ɼ��ģ�������Ķ����IsVisibleӦ���Ƿ���true�źô���
		if (m_pParent->GetDescription() && 
			OBJ_WINDOW == m_pParent->GetDescription()->GetMajorType())
		{ 
			return true;       
		}
		else
		{
			return m_pParent->IsVisible();
		}
	}
    return true;
}


bool  Object::IsCollapsed()
{
    bool bCollapsed = m_objState.visibility_ == VISIBILITY_COLLAPSED?true:false;
    if (bCollapsed || nullptr == m_pParent)
    {
        return bCollapsed;
    }
    // �жϸ����󲻿ɼ������ 
    else
    {
        return m_pParent->IsCollapsed();
    }

    return false;
}
bool  Object::IsSelfCollapsed()
{
    if (m_objState.visibility_ == VISIBILITY_COLLAPSED)
        return true;

    return false;
}


bool Object::IsEnable()
{
	return !m_objState.disable;
}

void  Object::LoadVisibleEx(long l)
{
	m_objState.visibility_ = static_cast<VISIBILITY_TYPE>(l);
}
long  Object::SaveVisibleEx()
{
	return m_objState.visibility_;
}

void Object::SetVisible(bool b)
{
    SetVisibleEx(b?VISIBILITY_VISIBLE:VISIBILITY_COLLAPSED);
}

void  Object::virtualSetVisibleEx(VISIBILITY_TYPE eType)
{
}


//
//	[static]
//   ��pParent���������ݹ�ת����Ϣ
//
void Object::ForwardMessageToChildObject(
	Object* pParent, UIMSG* pMsg)
{
	Object*  pChild = nullptr;
	while (pChild = pParent->EnumChildObject(pChild))
	{
		pMsg->pMsgTo = pChild->GetIMessage();
		::UISendMessage(pMsg);
		Object::ForwardMessageToChildObject(pChild, pMsg);
	}

	Object* pNcChild = nullptr;
	while (pNcChild = pParent->EnumNcChildObject(pNcChild))
	{
		pMsg->pMsgTo = pNcChild->GetIMessage();
		::UISendMessage(pMsg);
		Object::ForwardMessageToChildObject(pNcChild, pMsg);
	}
}

// �����������ٷ����Ӷ��������֪ͨһ�θ�����
// Ӧ�ó�����UI_WM_INITIALIZE/UI_WM_INITIALIZE2
// ע������Ѿ���ʼ�����˵Ŀؼ������ٷ�����Ϣ���绻���������֣���������Щ�ؼ������´�����
//    ���ǻ��ᷢ��һ��bindind֪ͨ���������°��µ��ӿؼ����������°󶨹�����
void Object::ForwardInitializeMessageToDecendant(Object* pObj)
{
	UIMSG msg;
	UIMSG msg2;
	UIMSG msg3;
	msg.message = UI_MSG_INITIALIZE;
	msg2.message = UI_MSG_INITIALIZE2;
	msg3.message = UI_MSG_DOBINDPLZ;
	msg3.wParam = 1; 

	Object*  pChild = nullptr;
	while (pChild = pObj->EnumAllChildObject(pChild))
	{

		if (pChild->m_objStyle.initialized)
		{
			Object::ForwardInitializeMessageToDecendant(pChild);
		}
		else
		{
			pChild->m_objStyle.initialized = 1;

			pChild->virtualOnLoad();

			msg.pMsgTo = pChild->GetIMessage();
			::UISendMessage(&msg);

			Object::ForwardInitializeMessageToDecendant(pChild);

			msg2.pMsgTo = pChild->GetIMessage();
			::UISendMessage(&msg2);
		}

		msg3.pMsgTo = pChild->GetIMessage();
		::UISendMessage(&msg3);
	}
}
void  Object::SetVisibleEx(VISIBILITY_TYPE eType)
{
    if (m_objState.visibility_ == eType)
        return;

	bool bOld = IsVisible();
	bool bVisibleCompatible = eType==VISIBILITY_VISIBLE ? true:false;

	m_objState.visibility_ = eType;
    virtualSetVisibleEx(eType);
	
	// ֪ͨ�Ӷ���
	UISendMessage(this, UI_MSG_VISIBLE_CHANGED, 
        bVisibleCompatible?TRUE:FALSE, 
        (LPARAM)this);

    UIMSG msg;
	msg.message = UI_MSG_VISIBLE_CHANGED;
    msg.wParam = bVisibleCompatible?TRUE:FALSE;
    msg.lParam = (LPARAM)this;
	Object::ForwardMessageToChildObject(this, &msg);

    if (m_pParent)
    {
		ILayout*  pLayout = 
			(ILayout*)UISendMessage(m_pParent, UI_MSG_GETLAYOUT);
        if (pLayout)
        {
            pLayout->ChildObjectVisibleChanged(m_pIObject);
        }
    }

	if (bVisibleCompatible != bOld)
	{
        // ȷʵ�����ڳ�ʼ���е���setvisible������ʱ��û�� m_pParent ���������SystemScrollBar
// 		if (bUpdateLayout)
// 		{
// 			this->UpdateLayout(bRedraw);
//         }
//         else if (bRedraw)
//         {
//             if (bVisibleCompatible)
//                 this->Invalidate(); 
//             else
//                 this->UpdateObjectBkgnd(true); 
//         }
//         else
//         {
//         }
	}
}



void  Object::SetDisableDirect(bool b)
{
    m_objState.disable = b;
}
bool  Object::IsSelfDisable()
{
	return m_objState.disable;
}

void  Object::virtualSetEnable(bool b)
{
	SetDisableDirect(!b);
}

void Object::SetEnable(bool b, bool bNoitfy)
{
	bool bOld = IsEnable();
	
	virtualSetEnable(b);
	
    if (bNoitfy && b != bOld)
	    UISendMessage(GetIMessage(), UI_MSG_STATECHANGED, (WPARAM)OSB_DISABLE);

	if (b != bOld)
    {
		// [ע] ���û��ָ��ˢ�£�����Ҫ�ⲿ��ʾ����UpdateObject����Ϊ�ÿؼ����ڲ�
		//      ��û������Ϊdirty��ֱ��ˢ����������Ҳ������¸ÿؼ�
		    this->Invalidate();

        // ���·��������Ϣ�������������λ�ڸÿؼ����棬����Ҫ���ÿؼ�����Ϊhover����������Ч
        POINT pt = {0, 0};
        ::GetCursorPos(&pt);
        HWND hWnd = GetHWND();
        ::MapWindowPoints(nullptr, hWnd, &pt, 1);
        ::PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
    }
}


bool Object::IsDefault()
{
	return m_objState.default_;
}

// bool Object::IsReadonly()
// {
// 	return this->testStateBit(OSB_READONLY);
// }

bool Object::IsHover()
{
	return m_objState.force_hover || m_objState.hover;
}

bool Object::IsPress()
{
	return m_objState.force_press || m_objState.press;
}

bool Object::IsForceHover()
{
	return m_objState.force_hover;
}

bool Object::IsForcePress()
{
	return m_objState.force_press;
}
bool  Object::IsSelected()
{
    return m_objState.selected;
}

void  Object::SetDefault(bool b, bool bNotify)
{
    if (m_objState.default_ == b)
        return;

    m_objState.default_ = b;
    if (bNotify)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_DEFAULT;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void  Object::SetSelected(bool b, bool bNotify)
{
    if (m_objState.selected == b)
        return;
    
    m_objState.selected = b;
    if (bNotify)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_SELECTED;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void Object::SetForceHover(bool b, bool bNotify)
{
    if (m_objState.force_hover == b)
        return;

    bool bOldHover = IsHover();
    
    m_objState.force_hover = b;
    if (bNotify && IsHover() != bOldHover)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void Object::SetForcePress(bool b, bool bNotify)
{
    if (m_objState.force_press == b)
        return;

    bool bOldPress = IsPress();

    m_objState.force_press = b;
    if (bNotify && IsPress() != bOldPress)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_PRESS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetHover(bool b, bool bNotify)
{
    if (m_objState.hover == b)
        return;

    m_objState.hover = b;
    if (bNotify)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetPress(bool b, bool bNotify)
{
    if (m_objState.press == b)
        return;

    m_objState.press = b;
    if (bNotify)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_PRESS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void  Object::SetAsNcObject(bool b) 
{
	m_objStyle.ncobject = b;
}
bool  Object::IsNcObject()   
{
	return m_objStyle.ncobject;
}
bool  Object::IsRejectMouseMsgAll()  
{
	return m_objStyle.reject_all_mouse_msg;
}
bool  Object::IsRejectMouseMsgSelf() 
{
	return m_objStyle.reject_self_mouse_msg;
}
void  Object::SetRejectMouseMsgAll(bool b)
{
	m_objStyle.reject_all_mouse_msg = b;
}
void  Object::SetRejectMouseMsgSelf(bool b)
{   
    m_objStyle.reject_self_mouse_msg = b;
}

// ���ֶ�����һ�����󣨷Ǵ�xml�м���ʱ�����Ե��øú�����ȫĬ�����Եļ���)
void  Object::InitDefaultAttrib()
{
    IMapAttribute* pMapAttrib = nullptr;
	if (m_pIMapAttributeRemain)
	{
		pMapAttrib = m_pIMapAttributeRemain;
		pMapAttrib->AddRef();
	}
	else
	{
		UICreateIMapAttribute(&pMapAttrib);
	}

    UIASSERT(m_strId.empty() && TEXT("��setid���ڸú���֮����ã����⸲��"));
	// pMapAttrib->AddAttr(XML_ID, m_strId.c_str()); // ��ֹid������??

    // ������ʽ
    UIASSERT(m_pSkinRes);
	
    StyleRes& styleRes = m_pSkinRes->GetStyleRes();

	LPCTSTR szStyle = pMapAttrib->GetAttr(nullptr, XML_STYLE, true);
	String strStyle;
	if (szStyle)
		strStyle = szStyle;

    styleRes.LoadStyle(
        m_pDescription->GetTagName(),
		strStyle.c_str(),
        nullptr,
        pMapAttrib);
	

	SERIALIZEDATA data = {0};
    data.pUIApplication = GetIUIApplication();
	data.pSkinRes = m_pSkinRes->GetISkinRes();
	data.pMapAttrib = pMapAttrib;
	data.nFlags = SERIALIZEFLAG_LOAD;

	SAFE_RELEASE(m_pIMapAttributeRemain);
	m_pIMapAttributeRemain = pMapAttrib;
	if (m_pIMapAttributeRemain)
		m_pIMapAttributeRemain->AddRef();

	UISendMessage(m_pIObject, UI_MSG_SERIALIZE, (WPARAM)&data);

	// ���û�ж�������ԣ�ֱ���ͷţ���ʡ�ڴ�
	if (m_pIMapAttributeRemain && 0 == m_pIMapAttributeRemain->GetAttrCount())
	{
		SAFE_RELEASE(m_pIMapAttributeRemain);
	}
    SAFE_RELEASE(pMapAttrib);
}
// 
// void Object::SetUserData(LPVOID p)
// {
// 	m_pUserData = p;
// }
// 
// LPVOID Object::GetUserData()
// {
// 	return m_pUserData;
// }


void  Object::SetOutRef(void** ppOutRef)
{
	m_ppOutRef = ppOutRef;
}

SkinRes*  Object::GetSkinRes()
{
	return m_pSkinRes;
}

ISkinRes*  Object::GetISkinRes()
{
	if (!m_pSkinRes)
		return nullptr;
	
	return m_pSkinRes->GetISkinRes();
}

UIApplication*  Object::GetUIApplication() 
{ 
	UIASSERT(m_pSkinRes);
	UIASSERT(m_pSkinRes->GetUIApplication());

	return m_pSkinRes->GetUIApplication();
}

IUIApplication*  Object::GetIUIApplication()
{
	UIASSERT(m_pSkinRes);
	UIASSERT(m_pSkinRes->GetUIApplication());

	return m_pSkinRes->GetUIApplication()->GetIUIApplication();
}


int  Object::GetZorder() 
{
    return m_lzOrder; 
}

void  Object::SetZorderDirect(int z) 
{
    m_lzOrder = z;

    //bool bOldHasLayer = GetSelfLayer()?true:false;
    bool bPosChanged = false;

    // ������zorder���޸��Լ���parent�е�λ��
    if (m_pParent)
    {
        if ((m_pPrev && m_pPrev->m_lzOrder > m_lzOrder) ||
            (m_pNext && m_pNext->m_lzOrder < m_lzOrder))
        {
            RemoveMeInTheTree();
            if (IsNcObject())
                m_pParent->AddNcChild(this);
            else
                m_pParent->AddChild(this);

            bPosChanged = true;
        }
    }
    
//     update_layer_ptr();
//     
//     bool bNowHasLayer = GetSelfLayer() ? true : false;
    if (/*bOldHasLayer && bNowHasLayer && */bPosChanged)
    {
        m_objLayer.OnObjPosInTreeChanged();
    }
}

bool  SortByZorder(Object* p1, Object* p2)
{
	return p1->GetZorder() < p2->GetZorder();
}

void  Object::SortChildByZorder()
{
	if (!m_pChild || !m_pChild->m_pNext)
		return;

	// ����
	vector<Object*> vec;
	Object* p = m_pChild;
	while (p)
	{
		vec.push_back(p);
		p = p->m_pNext;
	}
	std::sort(vec.begin(), vec.end(), SortByZorder);

	// Ӧ��
	vector<Object*>::size_type count = vec.size();
	m_pChild = vec[0];
	m_pChild->m_pPrev = nullptr;

	for (uint i = 1; i < count; i++)
	{
		vec[i]->m_pNext = nullptr;
		vec[i]->m_pPrev = vec[i-1];

		vec[i-1]->m_pNext = vec[i];
	}
}

void Object::SetBackRender(IRenderBase* p)
{
	SAFE_RELEASE(m_pBkgndRender);
	m_pBkgndRender = p;

	if (m_pBkgndRender)
		m_pBkgndRender->AddRef();
}


void Object::SetForegndRender(IRenderBase* p)
{
	SAFE_RELEASE(m_pForegndRender);
	
	m_pForegndRender = p;
	if (m_pForegndRender)
		m_pForegndRender->AddRef();
}


void Object::SetTextRender(ITextRenderBase* p)
{
	SAFE_RELEASE(m_pTextRender);
	
	m_pTextRender = p;
	if (m_pTextRender)
		m_pTextRender->AddRef();
}

ITextRenderBase*  Object::GetTextRender()
{
    return m_pTextRender; 
}
IRenderBase*  Object::GetBackRender() 
{ 
    return m_pBkgndRender; 
}
IRenderBase*  Object::GetForeRender()
{
    return m_pForegndRender; 
}

//	��ȡ�Լ�������,���ﷵ�صĶ���ֻ��һ����ʱ����
//	�����Ҫ����ʹ�ã�����Ҫ����AddRef
IRenderFont* Object::GetRenderFont()
{
	IRenderFont* pRenderFont = nullptr;

	// ���Լ���textrender��ȡ
	if (m_pTextRender)
		pRenderFont = m_pTextRender->GetRenderFont();

	if (pRenderFont)
		return pRenderFont;

	// �򴰿ڻ�ȡĬ��
	WindowBase* pWindow = GetWindowObject();
	if (pWindow)
		return pWindow->GetWindowDefaultRenderFont();

	return nullptr;
}

void  Object::load_renderbase(LPCTSTR szName, IRenderBase*& pRender)
{
    SAFE_RELEASE(pRender);
    if (szName)
	{
		GetUIApplication()->GetRenderBaseFactory().CreateRenderBaseByName(
            m_pSkinRes->GetISkinRes(), szName, m_pIObject, &pRender);
	}
}

void  Object::load_textrender(LPCTSTR szName, ITextRenderBase*& pTextRender)
{
    SAFE_RELEASE(pTextRender);
    if (szName)
		GetUIApplication()->GetTextRenderFactroy().CreateTextRenderBaseByName(
		m_pSkinRes->GetISkinRes(), szName, m_pIObject, &pTextRender);
}

LPCTSTR  Object::get_renderbase_name(IRenderBase*& pRender)
{
    if (!pRender)
        return nullptr;

    return GetUIApplication()->GetRenderBaseFactory().GetRenderBaseName(pRender->GetType());
}
LPCTSTR  Object::get_textrender_name(ITextRenderBase*& pTextRender)
{
    if (!pTextRender)
        return nullptr;

	return GetUIApplication()->GetTextRenderFactroy().GetTextRenderBaseName(pTextRender->GetType());
}

void  Object::LoadBkgndRender(LPCTSTR szName)   
{ 
    load_renderbase(szName, m_pBkgndRender); 
}
void  Object::LoadForegndRender(LPCTSTR szName) 
{ 
    load_renderbase(szName, m_pForegndRender);
}
void  Object::LoadTextRender(LPCTSTR szName)  
{ 
    load_textrender(szName, m_pTextRender); 
}
LPCTSTR  Object::SaveBkgndRender()  
{ 
    return get_renderbase_name(m_pBkgndRender);
}
LPCTSTR  Object::SaveForegndRender() 
{ 
    return get_renderbase_name(m_pForegndRender); 
}
LPCTSTR  Object::SaveTextRender() 
{ 
    return get_textrender_name(m_pTextRender); 
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               ��Ϣӳ��                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
//   �����յ�WM_DESTROY��Ϣ֮�󣬴��ݸ����е��Ӷ���
//
//void Object::OnDestroy()
//{
//	Object*  pObj = nullptr;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// ���Ӷ�����WM_DESTROY��Ϣ
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}


//void Object::OnPaint( HDC hDC )


// �ڽ��յ�OnSize��Ϣ��ʱ����Ҫ�ı��Լ���pos,rectP,rectW��Ա��
// ��֪ͨ�Լ����Ӷ���ȥ����
//void Object::OnSize( UINT nType, int cx, int cy )
//{
//	this->UpdateRect();
//
//	Object*  pObj = nullptr;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// ���Ӷ�����WM_ERASEBKGND��Ϣ
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}


//
//  ���ݶ������ڵı��������������ı���ɫ������ͼƬ����
//
// Two colors provide good color visibility if the brightness difference and the color difference between the two colors are greater than a set range.
// Color brightness is determined by the following formula:
// ((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
// Color difference is determined by the following formula:
// (max (Red 1, Red 2) - min (Red 1, Red 2)) + (max (Green 1, Green 2) - min (Green 1, Green 2)) + (max (Blue 1, Blue 2) - min (Blue 1, Blue 2))
//  The rage for color brightness difference is 125. The range for color difference is 500.
// 
//  google text background color black white
//  http://juicystudio.com/services/aertcolourcontrast.php
//  http://www.colourlovers.com/web/blog/2010/03/11/calculating-color-contrast-for-legible-text
//
#if 0
DWORD  Object::CalcContrastTextColor()
{
    WindowBase*  pWindowBase = GetWindowObject();
    if (nullptr == pWindowBase)
        return 0;

	assert (0 && TEXT("TODO"));
	return 0;
#if 0
    HDC hDC = pWindowBase->GetRenderLayer2()->GetRenderTarget()->GetHDC();
    if (nullptr == hDC)
        return 0;

    HBITMAP  hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
    if (nullptr == hBitmap)
        return 0;

    CRect rcLayer;
    GetRectInWindow(&rcLayer, true);

    Image  image;
    image.Attach(hBitmap, Image::DIBOR_TOPDOWN);
// #ifdef _DEBUG
//     image.Save(L"C:\\aa.png", Gdiplus::ImageFormatPNG);
// #endif
    DWORD  dwAverageColor = image.GetAverageColor(&rcLayer);
    image.Detach();

    // ��alphaֵӦ����
//    int a = dwAverageColor >> 24;
    int r = GetRValue(dwAverageColor);
    int g = GetGValue(dwAverageColor);
    int b = GetBValue(dwAverageColor);

    // TODO ?? �Ƿ���Ҫ����һ������Ӧ�ÿ���ɫԤ����û����
//     if (a)
//     {
//         r = (r<<8)/a;
//         g = (g<<8)/a;
//         b = (b<<8)/a;
// 
//         r = min(255, r);
//         g = min(255, g);
//         b = min(255, b);
//     }

//     int v = ((r * 299) + (g * 587) + (b * 114)) >> 10;
//     if (v < 125)
//         return 0xFFFFFFFF;
//     else
//         return 0xFF000000;

    // ����ü������ȣ������ǻ�ɫ
    int max = max(max(r,g),b);
    int min = min(min(r,g),b);
    int light = (min + max) >> 1;
    if (light < 128)
        return 0xFFFFFFFF;
    else
        return 0xFF000000;
#endif
}
#endif

Object*  Object::GetObjectByPos(POINT* pt)
{
    WindowBase* pWindow = GetWindowObject();
    if (nullptr == pWindow)
        return nullptr;

    return pWindow->GetObjectByPos(this, pt, false);
}

bool  Object::SetMouseCapture(int nNotifyMsgId)
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->SetMouseCapture(m_pIObject, nNotifyMsgId);
    return true;
}
bool  Object::ReleaseMouseCapture()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->ReleaseMouseCapture(m_pIObject);
    return true;
}
bool  Object::SetKeyboardCapture(int nNotifyMsgId)
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->SetKeyboardCapture(m_pIObject, nNotifyMsgId);
    return true;
}
bool  Object::ReleaseKeyboardCapture()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->ReleaseKeyboardCapture(m_pIObject);
    return true;
}

void  Object::load_layer_config(bool b)
{
    m_objStyle.layer = b;
}

bool  Object::HasLayer()
{
	return m_objStyle.layer;
}

void  Object::OnLayerDestory()
{
	m_objStyle.layer = false;

	// ֪ͨ��layer���»��棬�Ӷ������Լ��Ļ��棬������Ҫ�����Ӷ���
	if (m_pParent)
	{
		m_pParent->Invalidate(&m_rcParent);
	}
}
void  Object::OnLayerCreate()
{
	// ֪ͨ��layer���»��棬�Ӷ������Լ��Ļ��棬������Ҫ�����Ӷ���
	if (m_pParent)
	{
		m_pParent->Invalidate(&m_rcParent);
	}
}


// ���л���������
/*
void  Object::LoadColor(LPCTSTR szColorId, Color*& pColor)
{
    SAFE_RELEASE(pColor);
    if (!szColorId || !m_pUIApplication)
        return;

    ColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
    if (!pColorRes)
        return;

    pColorRes->GetColor(szColorId, &pColor);
}

LPCTSTR  Object::SaveColor(Color*& pColor)
{
    if (!pColor || !m_pUIApplication)
        return nullptr;

    ColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
    if (pColorRes)
    {
        LPCTSTR szId = pColorRes->GetColorId(pColor);
        if (szId)
            return szId;
    }

    TCHAR* szBuffer = GetTempBuffer();
    pColor->ToHexString(szBuffer);
    return szBuffer;
}
*/

// ��������
unsigned long  Object::GetChildCount()
{
	long lCount = 0;
	Object* p = this->GetChildObject();
	while (p)
	{
		lCount++;
		p = p->GetNextObject();
	}
	p = this->GetNcChildObject();
	while (p)
	{
		lCount++;
		p = p->GetNextObject();
	}

	return lCount;
}

Object*  Object::GetChildObjectByIndex(unsigned long lIndex)
{
	unsigned long lCount = 0;
	Object* p = this->GetChildObject();
	while (p)
	{
		if (lCount == lIndex)
			return p;

		lCount++;
		p = p->GetNextObject();
	}
	p = this->GetNcChildObject();
	while (p)
	{
		if (lCount == lIndex)
			return p;
		lCount++;
		p = p->GetNextObject();
	}

	return nullptr;
}

unsigned long  Object::GetChildObjectIndex(Object* pChild)
{
	unsigned long lCount = 0;
	Object* p = this->GetChildObject();
	while (p)
	{
		if (p == pChild)
			return lCount;

		lCount++;
		p = p->GetNextObject();
	}
	p = this->GetNcChildObject();
	while (p)
	{
		if (p == pChild)
			return lCount;
		lCount++;
		p = p->GetNextObject();
	}

	return 0;
}

// �ⲿ���IUIAccessible�Ľӿ�ʵ����
class UIAccessibleCreator : public IUIAccessibleCreator
{
public:
	UIAccessibleCreator(AccessibleImpl* p)
	{
		m_pAccessible = p;
	}
	virtual void  Add(IUIAccessible* p) override
	{
		if (m_pAccessible)
			m_pAccessible->AddUIAccessible(p);
	}
private:
	AccessibleImpl*  m_pAccessible;
};

bool  Object::CreateAccesible(IAccessible** pp)
{
	if (!pp)
		return false;

	if (!m_pAccessible)
	{
		AccessibleImpl* p = new AccessibleImpl;
		m_pAccessible = static_cast<IAccessible*>(p);
		m_pAccessible->AddRef();

		UIAccessibleCreator creator(p);

		// �������accessible
		UISendMessage(m_pIObject, UI_WM_CREATE_ACCESSIBLE, 
			(WPARAM)static_cast<IUIAccessibleCreator*>(&creator), 0);

		// ���Ĭ�ϵ�object accessible
		// �ڲ������addref

		// TODO1: Object����ʱ����ôȷ����Щ�ӿ�Ҳ���٣�����������release��
		// TODO2: ��ʹ�ô���������ҳ��ʱ�������Ȼ������ڴ�й¶���޷����
		p->AddUIAccessible(new ObjectAccessible(*this));
	}

	*pp = m_pAccessible;
	m_pAccessible->AddRef();
	return true;
}

void  Object::SetDescription(IObjectDescription* p)
{
    m_pDescription = p;
}
IObjectDescription*  Object::GetDescription()
{
    return m_pDescription;
}

// �Լ������е�λ�øı䡣���ڱ༭���У���ק�ؼ�����һ��panel����
void  Object::position_in_tree_changed()
{
    if (!m_pLayoutParam)
        return;

    // ���ݸ������������ɲ��ֲ���
    SAFE_RELEASE(m_pLayoutParam);
    GetSafeLayoutParam();
}

int UI::Object::GetMaxWidth()
{
	return m_nMaxWidth;
}

int UI::Object::GetMaxHeight()
{
	return m_nMaxHeight;
}

void  Object::SetMaxWidth(int n)
{
	m_nMaxWidth = n;
}
void  Object::SetMaxHeight(int n)
{
	m_nMaxHeight = n;
}

int   Object::GetMinWidth()
{
	return m_lMinWidth;
}
int   Object::GetMinHeight()
{
	return m_lMinHeight;
}
void  Object::SetMinWidth(int n)
{
	m_lMinWidth = n;
}
void  Object::SetMinHeight(int n)
{
	m_lMinHeight = n;
}