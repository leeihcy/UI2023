#include "stdafx.h"
#include "windowrender.h"
#include "Src\Base\Object\object.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Inc\Interface\iwndtransmode.h"
#include "Inc\Interface\irenderlayer.h"
#include "Src\Base\Attribute\attribute.h"
#include "Src\Base\Attribute\enum_attribute.h"

#include "Src\Layer\hardware_compositor.h"
#include "Src\Layer\software_compositor.h"
#include "..\Util\Stopwatch\stopwatch.h"
#include "..\UIObject\ListCtrl\ListItemBase\listitembase.h"

using namespace UI; 

WindowRender::WindowRender(WindowBase*  p)
{
    m_eGRL = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
    m_pWindow = p;
    m_lRefCanCommit = 0;
    
    m_pIWindowRender = nullptr;
//	m_pHardwareComposition = nullptr;
//    m_bNeedRebuildGpuLayerTree = true;
    m_bNeedAlphaChannel = true;

	m_pCompositor = nullptr;
	m_pCommitListener = nullptr;
}

WindowRender::~WindowRender()
{
    SAFE_DELETE(m_pCompositor);
    SAFE_DELETE(m_pIWindowRender);
//	SAFE_RELEASE(m_pHardwareComposition);
}

IWindowRender*  WindowRender::GetIWindowRender()
{
    if (!m_pIWindowRender)
        m_pIWindowRender = new IWindowRender(this);

    return m_pIWindowRender;
}

void  WindowRender::OnSerialize(SERIALIZEDATA* pData)
{
	AttributeSerializer s(pData, TEXT("WindowRender"));

	s.AddEnum(XML_WINDOW_GRAPHICS_RENDER_LIBRARY, *(long*)&m_eGRL)
		->FillGraphicsRenderLibraryData()
		->SetDefault(GRAPHICS_RENDER_LIBRARY_TYPE_GDI);

    s.AddBool(XML_WINDOW_NEED_ALPHACHANNEL, m_bNeedAlphaChannel)
		->SetDefault(true);
}

// IGpuRenderLayer*  WindowRender::CreateGpuLayerTexture(RenderLayer* p)
// {
// 	if (nullptr == m_pHardwareComposition)
// 	{
// 		if (m_pWindow->IsGpuComposite())
// 		{
// 			m_pHardwareComposition = CreateHardwareComposition(m_pWindow->m_hWnd);
// 		}
// 	}
// 
// 	IGpuRenderLayer* pGpuTexture = nullptr;
// 	if (m_pHardwareComposition)
// 	{
// 		pGpuTexture = m_pHardwareComposition->CreateLayerTexture();
// 		if (p->GetCreateObject() == m_pWindow)
// 			m_pHardwareComposition->SetRootLayerSurface(pGpuTexture);
// 
// 		m_bNeedRebuildGpuLayerTree = true;
// 	}
// 
// 	return pGpuTexture;
// }
//
// void  WindowRender::OnRenderLayerDestroy(RenderLayer* p)
// {
//     if (m_pHardwareComposition)
//     {
//         m_bNeedRebuildGpuLayerTree = true;
//     }
// }   

// IRenderTarget* 没有引用计数机制
// 但仍然采用Release进行释放（delete） 
bool  WindowRender::CreateRenderTarget(IRenderTarget** pp)
{
    if (!pp)
        return false;

    *pp = UICreateRenderTarget(m_pWindow->GetIUIApplication(), m_eGRL, m_bNeedAlphaChannel);
    return true;
}

// void  WindowRender::UpdateWindow(HDC hDC, RECT* prcDamageArray, uint nCount)
// {
//     CRect rcClient;
//     if (!prcDamageArray)
//     {
// 		::GetClientRect(m_pWindow->m_hWnd, &rcClient);
// 		prcDamageArray = &rcClient;
// 		nCount = 1;
// 	}
// 
// 	RenderLayer* pRenderLayer = m_pWindow->GetRenderLayer2();
// 	if (!pRenderLayer)
// 		return;
// 
// 	pRenderLayer->UpdateLayer(prcDamageArray, nCount);
// 	Commit(hDC, prcDamageArray, nCount);
// }

void WindowRender::OnWindowSize(UINT nWidth, UINT nHeight)
{
//     if (m_pHardwareComposition)
//         m_pHardwareComposition->Resize(nWidth, nHeight);

    if (m_pCompositor)
        m_pCompositor->Resize(nWidth, nHeight);
}

void  WindowRender::SetCanCommit(bool b)
{
    if (b)
        m_lRefCanCommit --;
    else
        m_lRefCanCommit ++;
}

bool  WindowRender::CanCommit() 
{ 
    return 0 == m_lRefCanCommit; 
}

// void  WindowRender::DrawIncrement()
// {
//     m_pCompositor->UpdateDirty(nullptr);
//     
// }
//
// void  WindowRender::Commit(HDC hDC, RECT* prc, int nCount)
// {
//     if (!CanCommit())
//         return;
// 
//     if (m_pHardwareComposition)
//     {
//         HardComposite();
//     }
//     else
//     {
//         m_pWindow->CommitDoubleBuffet2Window(hDC, prc, prc?nCount:0);
//     }
// }
// 
// void  WindowRender::HardComposite()
// {
//     if (m_pHardwareComposition)
//     {
//         if (m_bNeedRebuildGpuLayerTree)
//             this->RebuildCompositingLayerTree();
// 
//         if (m_pHardwareComposition->BeginCommit())
//         {
//             RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();
// 
// 			RECT rcClip;
// 			::GetClientRect(m_pWindow->GetHWND(), &rcClip);
// 
// 			GpuLayerCommitContext context;
// 			context.SetClipRect(&rcClip);
//             pRootLayer->Compositor2Gpu(context);
//             m_pHardwareComposition->EndCommit();
//         }
//     }
// }
// 
// void  WindowRender::OnHardCompositeChanged(bool bEnable)
// {
// 	if (m_bNeedRebuildGpuLayerTree)
// 		this->RebuildCompositingLayerTree();
// 
// 	RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();
//     if (pRootLayer)
//     {
// 	    pRootLayer->OnHardCompositeChanged(bEnable);
//     }
// }
// 
// void  WindowRender::SetNeedRebuildCompositingLayerTree()
// {
// 	m_bNeedRebuildGpuLayerTree = true;
// }
// 
// // 通过renderlayer树来构建gpulayer树
// void  WindowRender::RebuildCompositingLayerTree()
// {
//     if (!m_bNeedRebuildGpuLayerTree)
//         return;
// 
//     RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();
//     if (!pRootLayer)
//         return;
// 
//     pRootLayer->ClearChildren();
//     rebuildCompositingLayerTree(m_pWindow,pRootLayer);
// 
//     m_bNeedRebuildGpuLayerTree = false;
// }
// void  WindowRender::rebuildCompositingLayerTree(
//         Object* pParent,
//         RenderLayer* pParentLayer)
// {
//     if (!pParent || !pParentLayer)
//         return;
// 
//     Object*  pChild = nullptr;
//     RenderLayer*  pLayer = nullptr;
// 
//     while (pChild = pParent->EnumAllChildObject(pChild))
//     {
//         pLayer = pChild->GetSelfRenderLayer2();
//         if (pLayer)
//         {
// //             IGpuRenderLayer*  pGpuLayerTexture = pLayer->GetGpuTexture();
// //             UIASSERT(pGpuLayerTexture);
// 
// //             pGpuLayerTexture->ClearChildren();
// //             pParentLayer->AddChild(pGpuLayerTexture);
// // #ifdef _DEBUG
// //             POINT pt = pChild->GetWindowPoint();
// //             pGpuLayerTexture->SetWindowPos(pt.x, pt.y);
// // #endif            
// //             rebuildCompositingLayerTree(pChild, pGpuLayerTexture);
// 
//             pLayer->ClearChildren();
//             pParentLayer->AddChild(pLayer);
//             rebuildCompositingLayerTree(pChild, pLayer);
//         }
//         else
//         {
//             rebuildCompositingLayerTree(pChild, pParentLayer);
//         }
//     }
// }

void  WindowRender::SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye)
{
	// 仅在窗口创建之前设置有用
	m_eGRL = eTpye;
}
GRAPHICS_RENDER_LIBRARY_TYPE  WindowRender::GetGraphicsRenderType()
{
	return m_eGRL;
}
bool  WindowRender::GetRequireAlphaChannel()
{
    return m_bNeedAlphaChannel;
}

Layer*  WindowRender::CreateLayer(IObjectLayerContent* pContent)
{
	if (!pContent)
		return nullptr;

	Object& obj = pContent->GetObject();
	Layer* pNewLayer = get_create_compositor()->CreateLayer();
	pNewLayer->SetContent(pContent);

	
	if (!m_pCompositor->GetRootLayer())
	{
		m_pCompositor->SetRootLayer(pNewLayer);
		return pNewLayer;
	}

	// 获取父layer
	Layer* pParentLayer = pContent->GetParentLayer();
	if (!pParentLayer)
	{
		// 有根结点，但没父结点？
		UIASSERT(0);
		pParentLayer = m_pCompositor->GetRootLayer();
	}

	// 计算这个layer应该插入在layer tree的哪个位置，需要与object tree对应
	Layer* pNextLayer = pContent->GetNextLayer();
	pParentLayer->AddSubLayer(pNewLayer, pNextLayer);

	// 整理子对象的layer
	Layer* p = pParentLayer->GetFirstChild();
	while (p)
	{
		if (p != pNewLayer)
		{
			ILayerContent* pLayerContent = p->GetContent();

			// 发现自己的子对象，替换它，并将它作为自己的子layer
			if (pLayerContent && pLayerContent->IsChildOf(pContent))
			{
				p->MoveLayer2NewParentEnd(pParentLayer, pNewLayer);
			}
		}
		p = p->GetNext();
	}

	return pNewLayer;
}

Layer*  WindowRender::CreateLayer(IListItemLayerContent* pContent)
{
	if (!pContent)
		return nullptr;

	ListItemBase& item = pContent->GetListItem();
	Layer* pNewLayer = get_create_compositor()->CreateLayer();
	pNewLayer->SetContent(pContent);

	if (!m_pCompositor->GetRootLayer())
	{
		m_pCompositor->SetRootLayer(pNewLayer);
		return pNewLayer;
	}

	// 获取父layer
	Layer* pParentLayer = pContent->GetParentLayer();
	if (!pParentLayer)
	{
		// 有根结点，但没父结点？
		UIASSERT(0);
		pParentLayer = m_pCompositor->GetRootLayer();
	}

	// 计算这个layer应该插入在layer tree的哪个位置，需要与object tree对应
	Layer* pNextLayer = pContent->GetNextLayer();
	pParentLayer->AddSubLayer(pNewLayer, pNextLayer);

	// 整理子对象的layer
	Layer* p = pParentLayer->GetFirstChild();
	while (p)
	{
		if (p != pNewLayer)
		{
			ILayerContent* pLayerContent = p->GetContent();

			// 发现自己的子对象，替换它，并将它作为自己的子layer
			if (pLayerContent && pLayerContent->IsChildOf(pContent))
			{
				p->MoveLayer2NewParentEnd(pParentLayer, pNewLayer);
			}
		}
		p = p->GetNext();
	}

	return pNewLayer;
}

Compositor* UI::WindowRender::get_create_compositor()
{
    if (!m_pCompositor)
    {
        if (m_pWindow->IsGpuComposite())
        {
            m_pCompositor = new ::HardwareCompositor();
        }
        else
        {
            m_pCompositor = new SoftwareCompositor;
        }
        m_pCompositor->SetUIApplication(m_pWindow->GetUIApplication());
        m_pCompositor->SetWindowRender(this);
        m_pCompositor->BindHWND(m_pWindow->GetHWND());
    }

    return m_pCompositor;
}

void  WindowRender::InvalidateNow()
{
	if (!m_pCompositor)
		return;

#ifdef _DEBUGx
	StopWatch w;
	w.Start();
	m_pCompositor->DoInvalidate();
	UI_LOG_DEBUG(L"compositor cost: %d", (int)w.Now__());
#else
	m_pCompositor->DoInvalidate();
#endif
}

void  WindowRender::OnPaint(HDC hDC, RECT* prcInvalid)
{
    if (m_pCompositor)
    {
        RectArray arr;
        arr.AddRect(prcInvalid);
        m_pCompositor->Commit(arr);
    }
}

void  WindowRender::RequestInvalidate()
{
	if (m_pCompositor)
	{
		m_pCompositor->RequestInvalidate();
	}
}

void WindowRender::BindHWND(HWND hWnd)
{
    if (m_pCompositor)
        m_pCompositor->BindHWND(hWnd);
}

void WindowRender::SetCommitListener(IWindowCommitListener* p)
{
    m_pCommitListener = p;
}
IWindowCommitListener*  WindowRender::GetCommitListener()
{
    return m_pCommitListener;
}