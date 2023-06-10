#include "stdafx.h"
#include "object_layer.h"
#include "object.h"
#include "Src/Layer/windowrender.h"
#include "Src/UIObject/Window/windowbase.h"
#include "Src/UIObject/ListCtrl/ListItemBase/listitembase.h"

using namespace UI;

ObjectLayer::ObjectLayer(Object& o) : m_obj(o)
{
	m_pLayer = nullptr;
}

ObjectLayer::~ObjectLayer()
{
	DestroyLayer();
}

void UI::ObjectLayer::CreateLayer()
{
	if (m_pLayer)
		return;

	WindowRender* pWndRender = nullptr;

	WindowBase* pWindow = m_obj.GetWindowObject();
	if (pWindow)
		pWndRender = pWindow->GetWindowRender();

	if (pWndRender)
	{
		m_pLayer = pWndRender->CreateLayer(static_cast<IObjectLayerContent*>(this));

		CRect rcParent;
		m_obj.GetParentRect(&rcParent);
		if (!IsRectEmpty(&rcParent))
			OnSize(rcParent.Width(), rcParent.Height());

		m_obj.OnLayerCreate();
	}
	else
	{
		// ��resize��ʱ�򴴽�
		// UIASSERT(0);// �п��ܴ����������������ڼ䣬�߼��ֵ�����һ�ζ�����
	}
}

void  ObjectLayer::TryDestroyLayer()
{
	if (!m_pLayer)
		return;

	m_pLayer->TryDestroy();
}

void  ObjectLayer::DestroyLayer()
{
	if (!m_pLayer)
		return;

	Layer*  p = m_pLayer;
	m_pLayer = nullptr;

	if (p)
		p->Destroy();
}

void  ObjectLayer::Draw(UI::IRenderTarget* pRenderTarget) 
{
    m_obj.DrawToLayer__(pRenderTarget);
}

void  ObjectLayer::GetWindowRect(RECT* prcOut) 
{
    m_obj.GetWindowRect(prcOut);
}

void  ObjectLayer::GetParentWindowRect(RECT* prcOut)
{
    if (m_obj.GetParentObject())
        m_obj.GetParentObject()->GetWindowRect(prcOut);  // TODO: -->> visible part only
}

void  ObjectLayer::OnSize(uint nWidth, uint nHeight)
{
    if (m_pLayer)
    {
        m_pLayer->OnSize(nWidth, nHeight);
    }
}

bool  ObjectLayer::IsChildOf(ILayerContent* pParentLayer)
{
    if (!pParentLayer)
        return false;

	if (pParentLayer->Type() == LayerContentTypeObject)
	{
		Object& parentObject =
			static_cast<ObjectLayer*>(pParentLayer)->GetObject();

		return parentObject.IsMyChild(&m_obj, true);
	}
	else if (pParentLayer->Type() == LayerContentTypeListItem)
	{
		Panel* panel = 
			static_cast<IListItemLayerContent*>(pParentLayer)
			->GetListItem().GetRootPanel();
		if (!panel)
			return false;

		return panel->IsMyChild(&m_obj, true);
	}
	else
	{
		UIASSERT(0);
	}

    return false;
}

bool  ObjectLayer::IsVisible()
{
    return m_obj.IsVisible();
}

// object�ڶ������е�λ�øı��ˣ�ͬ�����ֲ�������
void  ObjectLayer::OnObjPosInTreeChanged()
{
    UIASSERT (m_pLayer);

    Layer* pParentLayer = m_pLayer->GetParent();
	UIASSERT(pParentLayer);

    m_pLayer->RemoveMeInTheTree();
    pParentLayer->AddSubLayer(m_pLayer, GetNextLayer());
}

void  ObjectLayer::OnLayerDestory()
{
	if (m_pLayer)  // ��ObjectLayer::~ObjectLayer()�����ģ���֪ͨ
	{
		m_pLayer = nullptr;
		m_obj.OnLayerDestory();
	}
}

Layer*  ObjectLayer::GetParentLayer()
{
	Object* parent = m_obj.GetParentObject();
	if (!parent)
		return nullptr;

	return parent->GetLayer();
}


// Layer Tree ���ɹؼ�����������webkit��
// RenderLayer* RenderObject::findNextLayer(RenderLayer* parentLayer, RenderObject* startPoint, bool checkParent)
// http://opensource.apple.com//source/WebCore/WebCore-658.28/rendering/RenderObject.cpp
// ��ȡpStartObj��layerӦ�ò�����pParentLayer���ĸ�λ��
// ע�����ﲢû�д����Լ����Ӷ����Ѿ���layer��pParentLayer������������Ҫ��������
// Layer*  Object::FindNextLayer(
//             Layer* pParentLayer, 
//             Object* pStartObj,
//             bool bCheckParent)
// {
//     if (!pParentLayer)
//         return nullptr;
//     
//     Layer* pThisLayer = m_pLayer ? m_pLayer->GetLayer() : nullptr;
// 
//     // �ҵ�һ��ͬ����layer�������������pStartObj�ĺ��棬��˿��Խ�pStartObj
//     // ��Ӧ��layer���뵽��������layerǰ�档
//     if (pThisLayer && pThisLayer->GetParent() == pParentLayer)
//         return pThisLayer;
// 
//     // ����������û��layer�������ݹ����
//     if (!pThisLayer || pThisLayer == pParentLayer)
//     {
//         Object* p = pStartObj ? pStartObj->GetNextObject() : GetChildObject();
//         for (; p; p = p->GetNextObject())
//         {
//             Layer* pReturn = p->FindNextLayer(
//                 pParentLayer, nullptr, false);
// 
//             if (pReturn)
//                 return pReturn;
//         }
//     }
// 
//     // �ҵ����ˣ�û������layer
//     if (pThisLayer == pParentLayer)
//         return nullptr;
// 
//     // ������
//     if (m_pParent && bCheckParent)
//         return m_pParent->FindNextLayer(pParentLayer, this, true);
// 
//     return nullptr;
// }

// �ж�˼·������������һ���㣬ֻ��������������һ��Object�г��֡�
Layer*  ObjectLayer::GetNextLayer()
{
	Layer* pParentLayer = GetParentLayer();

	Object* pNextTreeObject = m_obj.GetNextTreeItemObject();
	while (pNextTreeObject)
	{
		Layer* pThisLayer = pNextTreeObject->GetSelfLayer();

		// �һص���layer�ˣ�˵���Լ��������һ����û��next
		if (pThisLayer && pThisLayer == pParentLayer)
			return nullptr;

		// �ں���Ľ���У��ҵ���һ��ͬ���㣬��������ǰ�� 
		if (pThisLayer && pThisLayer->GetParent() == pParentLayer)
			return pThisLayer;

		// �����������һ����������
		pNextTreeObject = pNextTreeObject->GetNextTreeItemObject();
	}

	return nullptr;
}
