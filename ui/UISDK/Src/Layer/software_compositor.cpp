#include "stdafx.h"
#include "software_compositor.h"
#include "software_layer.h"
#include "Inc/Interface/renderlibrary.h"
#include "windowrender.h"
#include "../UIObject/Window/windowbase.h"

Layer* SoftwareCompositor::virtualCreateLayer()
{
    return new SoftwareLayer;
}

void  SoftwareCompositor::virtualBindHWND(HWND)
{

}

// 软件渲染需要返回脏区域的窗口坐标，用于增量提交到最终窗口上面
void  SoftwareCompositor::UpdateDirty(__out_opt RectArray& arrDirtyInWindow)
{
	if (!m_pRootLayer)
		return;

    // 先返回当前窗口脏区域
    m_pRootLayer->CopyDirtyRect(arrDirtyInWindow);

    // 更新各个层
    m_pRootLayer->UpdateDirty();

// 	Layer* p = m_pRootLayer;
// 	while (p)
// 	{
// 		update_dirty_recursion(p);
// 		p = p->GetNext();
// 	}

}

void  SoftwareCompositor::update_dirty_recursion(Layer* p)
{
	UIASSERT(p);

	static_cast<SoftwareLayer*>(p)->UpdateDirty();

	p = p->GetFirstChild();
	while (p)
	{
		update_dirty_recursion(p);
		p = p->GetNext();
	}
}

void  SoftwareCompositor::virtualCommit(const RectArray& arrDirtyInWindow)
{
    if (!m_pRootLayer)
        return;

	uint nCount = arrDirtyInWindow.GetCount();
	if (!nCount)
		return;

	// 给分层窗口提交的机会。
	if (m_pWindowRender->m_pWindow->virtualCommitReq())
		return;

	HDC hDC = GetDC(m_hWnd);
    for (uint i = 0; i < nCount; i++)
    {
        RECT& rcInWindow = ((LPRECT)arrDirtyInWindow.GetArrayPtr2())[i];

        Render2TargetParam param = {0};
        param.xSrc = param.xDst = rcInWindow.left;
        param.ySrc = param.yDst = rcInWindow.top;
        param.wSrc = param.wDst = rcInWindow.right - rcInWindow.left;
        param.hSrc = param.hDst = rcInWindow.bottom - rcInWindow.top;
        m_pRootLayer->GetRenderTarget()->Render2DC(hDC, &param);
    }
	ReleaseDC(m_hWnd, hDC);
}


void  SoftwareCompositor::commit_recursion(Layer* p)
{
//     static_cast<SoftwareLayer*>(p)->Commit();
// 
//     p = p->GetFirstChild();
//     while (p)
//     {
//         commit_recursion(p);
//         p = p->GetNext();
//     }
}

void  SoftwareCompositor::Resize(uint nWidth, uint nSize) 
{

}

