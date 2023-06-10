#include "stdafx.h"
#include "sync_window.h"

SyncWindow::SyncWindow()
{
    m_bSendByDeferWindowPos = false;
    m_hHostWnd = nullptr;
    m_hWnd = nullptr;

    m_nAnchorMagnetCapability = 10;
    m_bAnchorToScreenEdge = false;
    m_bAnchorToOtherSize = false;
    m_bSizeMove = false;
}

void SyncWindow::SetHwnd(HWND hWnd)
{
    m_hWnd = hWnd;
}

void  SyncWindow::_OnShowWindow(WPARAM wParam, LPARAM lParam)
{
    int nSize = (int)m_vecAnchorItems.size();
    for (int i = 0; i < nSize; i++)
    {
        HWND hWnd = m_vecAnchorItems[i].m_hWnd;
        if (wParam)
        {
            ::ShowWindow(hWnd, SW_SHOWNOACTIVATE);
        }
        else
        {
            ::ShowWindow(hWnd, SW_HIDE);
        }
    }
}

void SyncWindow::_OnWindowPosChanging(LPWINDOWPOS lpWndPos, BOOL& bHandled)
{
    if (lpWndPos->flags & SWP_SHOWWINDOW)
    {
        if (m_hHostWnd)
        {
            int a = 0;
        }
    }
    if (lpWndPos->flags & SWP_NOMOVE && lpWndPos->flags & SWP_NOSIZE)
    {
        // 不加这个条件判断会导致HostWindow在从最小化恢复时，SiteWindow不跟随一起恢复
        bHandled = FALSE;
        return;
    }

    bHandled = TRUE;
    // 先判断自己当前是否跟随于其它窗口，然后再处理自己的移动。
    if (m_hHostWnd)
    {
        if (this->OnSiteWindowPosChanging(lpWndPos))
            return;
    }

    if (m_vecAnchorItems.size() > 0)
    {
        this->OnHostWindowPosChanging(lpWndPos);
    }
    else
    {
        bHandled = FALSE;
    }
}

// 立即同步一次，模拟一把窗口大小改变
void SyncWindow::SyncNow()
{
    WINDOWPOS wndPos = { 0 };
    wndPos.hwnd = m_hWnd;

    CRect rc;
    GetWindowRect(m_hWnd, &rc);

    vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
    vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
    for (; iter != iterEnd; iter++)
    {
        HWND hWnd = iter->m_hWnd;
        if (hWnd /*&& IsWindowVisible(hWnd)*/) //  不可见也要操作
        {
            HDWP hdwpNull = nullptr;
            HandleWindowAnchor(hdwpNull, rc, *iter);
        }
    }

//     wndPos.x = rc.left;
//     wndPos.y = rc.top;
//     wndPos.cx = rc.Width();
//     wndPos.cy = rc.Height();
//     wndPos.flags = 0;
// 
//     OnHostWindowPosChanging(&wndPos);
}

void SyncWindow::OnHostWindowPosChanging(LPWINDOWPOS lpWndPos)
{
    if (m_bSendByDeferWindowPos)
    {
        m_bSendByDeferWindowPos = false;  // DeferWindowPos传递过来的消息，放行。
        return;
    }

    // 计算当前窗口的新坐标大小
    CRect rc;
    this->GetRectByWNDPOS(lpWndPos, &rc);

    do
    {
        // 注：DeferWindowPos不支持SWP_NOSENDCHANGING，会报无效参数，因此在这里才增加了一个
        //     bool 变量来防止死循环

        // 计算需要同步移动的窗口数量 
        int nCount = this->GetAvailableHWNDCount();

        // +1 自己本身的m_hWnd.（分层窗口自己维护自己的大小改变）
        bool IsLayeredWindow = lpWndPos->flags & SWP_LAYEREDWINDOW_SIZEMOVE ? true : false;
        if (!IsLayeredWindow)
            nCount += 1;

        HDWP hdwp = BeginDeferWindowPos(nCount);
        if (nullptr == hdwp) break;

        if (!IsLayeredWindow)
        {
            UINT nFlag = lpWndPos->flags & ~0x01000000;  // ?? 不过滤掉该标记会导致DeferWindodwPos失败，不知道为什么
            hdwp = ::DeferWindowPos(hdwp,
                m_hWnd,
                lpWndPos->hwndInsertAfter,
                lpWndPos->x,
                lpWndPos->y,
                lpWndPos->cx,
                lpWndPos->cy,
                nFlag/*|SWP_NOSENDCHANGING*/);
            if (nullptr == hdwp)
            {
                // HOST窗口最大小、还原的时候会失败
                break;
            }
        }

        bool bLoopResult = true;
        vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
        vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
        for (; iter != iterEnd; iter++)
        {
            HWND hWnd = iter->m_hWnd;
            if (hWnd /*&& IsWindowVisible(hWnd)*/ && iter->m_bAnchorOn)
            {
                hdwp = HandleWindowAnchor(hdwp, rc, *iter);
                if (nullptr == hdwp)
                {
                    bLoopResult = false;
                    break;
                }
            }
        }
        if (false == bLoopResult)  break;

        if (!IsLayeredWindow)
        {
            m_bSendByDeferWindowPos = true;
        }
        BOOL bRet = EndDeferWindowPos(hdwp);
        UIASSERT(bRet);

        // 注：在这里不能添加SWP_NOZORDER，否则会导致窗口拖拽时Z次序不正确
        lpWndPos->flags |= SWP_NOMOVE | SWP_NOSIZE; // 拒绝本次修改，使用DeferWindowPos的消息

        return;
    } while (0);

    // 失败了，调用默认过程
    vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
    vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
    for (; iter != iterEnd; iter++)
    {
        HWND hWnd = iter->m_hWnd;
        if (hWnd /*&& IsWindowVisible(hWnd)*/) //  不可见也要操作
        {
            HDWP hdwpNull = nullptr;
            HandleWindowAnchor(hdwpNull, rc, *iter);
        }
    }
}

void SyncWindow::OnHostWindowPosChanging()
{
    UIASSERT(nullptr != m_hHostWnd);
    m_bSendByDeferWindowPos = true;
}

bool SyncWindow::OnSiteWindowPosChanging(LPWINDOWPOS lpWndPos)
{
    if (m_bSendByDeferWindowPos)  // 本次的移动是因为HOST调用deferwindowpos产生的，忽略
    {
        m_bSendByDeferWindowPos = false;
        return false;
    }

    // 如果不是真的在用鼠标拖拽拉伸，则不处理，例如窗口刚创建，
    // 另外分层窗口在拖拽改变大小时，目前不发送ENTERSIZEMOVE消息，可以直接处理
    if (m_bSizeMove || lpWndPos->flags & SWP_LAYEREDWINDOW_SIZEMOVE)
    {
        bool bSize = (lpWndPos->flags & SWP_NOSIZE) ? false : true;   // 该字段不正确，即使只拖动窗口，也没有SWP_NOSIZE标志
        bool bMove = (lpWndPos->flags & SWP_NOMOVE) ? false : true;
        if (!bSize && !bMove)
            return false;

        SyncWindowData data;
        data.m_hWnd = GetMyHWND();

        CRect rcHost, rcSite, rcSiteOld;
        ::GetWindowRect(m_hHostWnd, &rcHost);
        ::GetWindowRect(data.m_hWnd, &rcSiteOld);
        this->GetRectByWNDPOS(lpWndPos, &rcSite);

        if (rcSiteOld.Width() != lpWndPos->cx || rcSiteOld.Height() != lpWndPos->cy)
        {
            bSize = true;
        }
        else
        {
            bSize = false;
        }
        bool bRet = this->CheckRectAnchor(&rcHost, &rcSite, bSize, &data);

        if (bRet)  // 粘上了，修改窗口的位置
        {
            lpWndPos->x = rcSite.left;
            lpWndPos->y = rcSite.top;
            lpWndPos->cx = rcSite.Width();
            lpWndPos->cy = rcSite.Height();
        }

        SendMessage(m_hHostWnd, UI_WM_SYNC_WINDOW, MODIFY_SYNC_WINDOW, (LPARAM)&data);
    }
    return false;
}

//
// 判断两个区域的粘合关系，如果粘上，则修改prcSize的值
//
// Return
//    返回两个区域是否粘合上
//
// Remark
//  Q1. 如果区分本次是MOVE，还是SIZE？当前窗口现在的高和宽，对比lpWindowPos中的高和宽来决定现在是否是在change size.
//      TODO: 可能会不准确吧...
//
bool SyncWindow::CheckRectAnchor(const CRect* prcHost, CRect* prcSite, bool bChangeSize, SyncWindowData* pData)
{
    bool bLeftLeft = (abs(prcSite->left - prcHost->left) < m_nAnchorMagnetCapability);
    bool bLeftRight = (abs(prcSite->left - prcHost->right) < m_nAnchorMagnetCapability);
    bool bRightLeft = (abs(prcSite->right - prcHost->left) < m_nAnchorMagnetCapability);
    bool bRightRight = (abs(prcSite->right - prcHost->right) < m_nAnchorMagnetCapability);
    bool bX = bLeftLeft || bLeftRight || bRightRight || bRightLeft ||
        (prcSite->left > prcHost->left && prcSite->left<prcHost->right) ||
        (prcSite->right>prcHost->left && prcSite->right<prcHost->right) ||
        (prcHost->left>prcSite->left && prcHost->left<prcSite->right) ||
        (prcHost->right>prcSite->left && prcHost->right < prcSite->right);

    bool bTopTop = (abs(prcHost->top - prcSite->top) < m_nAnchorMagnetCapability);
    bool bTopBottom = (abs(prcSite->top - prcHost->bottom) < m_nAnchorMagnetCapability);
    bool bBottomTop = (abs(prcSite->bottom - prcHost->top) < m_nAnchorMagnetCapability);
    bool bBottomBottom = (abs(prcSite->bottom - prcHost->bottom) < m_nAnchorMagnetCapability);
    bool bY = bTopTop || bTopBottom || bBottomTop || bBottomBottom ||
        (prcSite->top > prcHost->top && prcSite->top<prcHost->bottom) ||
        (prcSite->bottom>prcHost->top && prcSite->bottom<prcHost->bottom) ||
        (prcHost->top>prcSite->top && prcHost->top<prcSite->bottom) ||
        (prcHost->bottom>prcSite->top && prcHost->bottom < prcSite->bottom);

    bool bXAnchored = false;
    bool bYAnchored = false;
    int  nAnchorType = ANCHOR_NONE;
    if (bY)
    {
        if (bLeftLeft)
        {
            if (bChangeSize)
            {
                prcSite->left = prcHost->left;
            }
            else
            {
                int W = prcSite->Width();
                prcSite->left = prcHost->left;
                prcSite->right = prcSite->left + W;
            }
            bXAnchored = true;
            nAnchorType |= ANCHOR_LEFT;
        }
        else if (bLeftRight)
        {
            if (bChangeSize)
            {
                prcSite->left = prcHost->right;
            }
            else
            {
                int W = prcSite->Width();
                prcSite->left = prcHost->right;
                prcSite->right = prcSite->left + W;
            }
            bXAnchored = true;
            nAnchorType |= ANCHOR_OUT_RIGHT;
        }
        else if (bRightLeft)
        {
            if (bChangeSize)
            {
                prcSite->right = prcHost->left;
            }
            else
            {
                int W = prcSite->Width();
                prcSite->right = prcHost->left;
                prcSite->left = prcSite->right - W;
            }
            bXAnchored = true;
            nAnchorType |= ANCHOR_OUT_LEFT;
        }
        else if (bRightRight)
        {
            if (bChangeSize)
            {
                prcSite->right = prcHost->right;
            }
            else
            {
                int W = prcSite->Width();
                prcSite->right = prcHost->right;
                prcSite->left = prcSite->right - W;
            }
            bXAnchored = true;
            nAnchorType |= ANCHOR_RIGHT;
        }
    }

    if (bX)
    {
        if (bTopTop)
        {
            if (bChangeSize)
            {
                prcSite->top = prcHost->top;
            }
            else
            {
                int H = prcSite->Height();
                prcSite->top = prcHost->top;
                prcSite->bottom = prcSite->top + H;
            }
            bYAnchored = true;
            nAnchorType |= ANCHOR_TOP;
        }
        else if (bTopBottom)
        {
            if (bChangeSize)
            {
                prcSite->top = prcHost->bottom;
            }
            else
            {
                int H = prcSite->Height();
                prcSite->top = prcHost->bottom;
                prcSite->bottom = prcSite->top + H;
            }
            bYAnchored = true;
            nAnchorType |= ANCHOR_OUT_BOTTOM;
        }
        else if (bBottomTop)
        {
            if (bChangeSize)
            {
                prcSite->bottom = prcHost->top;
            }
            else
            {
                int H = prcSite->Height();
                prcSite->bottom = prcHost->top;
                prcSite->top = prcSite->bottom - H;
            }
            bYAnchored = true;
            nAnchorType |= ANCHOR_OUT_TOP;
        }
        else if (bBottomBottom)
        {
            if (bChangeSize)
            {
                prcSite->bottom = prcHost->bottom;
            }
            else
            {
                int H = prcSite->Height();
                prcSite->bottom = prcHost->bottom;
                prcSite->top = prcSite->bottom - H;
            }
            bYAnchored = true;
            nAnchorType |= ANCHOR_BOTTOM;
        }
    }

    // 修正nAnchorType只停靠了一个方向，另一个方向没有赋值的问题。
    if (0 != nAnchorType)
    {
        pData->m_nMask |= SWDS_MASK_ANCHORTYPE | SWDS_MASK_ANCHORON | SWDS_MASK_ANCHORDATA;
        pData->m_nAnchorType = nAnchorType;
        pData->m_bAnchorOn = true;
        pData->m_rcAnchorData.Width = -1;
        pData->m_rcAnchorData.Height = -1;
        if (false == bXAnchored)
        {
            pData->m_nAnchorType |= ANCHOR_LEFT;
            pData->m_rcAnchorData.xOffset = prcSite->left - prcHost->left;
        }
        else if (false == bYAnchored)
        {
            pData->m_nAnchorType |= ANCHOR_TOP;
            pData->m_rcAnchorData.yOffset = prcSite->top - prcHost->top;
        }

        return true;
    }
    else
    {
        pData->m_nMask |= SWDS_MASK_ANCHORON;
        pData->m_bAnchorOn = false;

        return false;
    }

    return false;
}

void SyncWindow::GetRectByWNDPOS(LPWINDOWPOS lpWndPos, CRect* prc)
{
    int   x = 0, y = 0, w = 0, h = 0;
    ::GetWindowRect(GetMyHWND(), prc);
    if (lpWndPos->flags & SWP_NOMOVE)
    {
        x = prc->left;
        y = prc->top;
    }
    else
    {
        x = lpWndPos->x;
        y = lpWndPos->y;
    }
    if (lpWndPos->flags & SWP_NOSIZE)
    {
        w = prc->right - prc->left;
        h = prc->bottom - prc->top;
    }
    else
    {
        w = lpWndPos->cx;
        h = lpWndPos->cy;
    }
    prc->SetRect(x, y, x + w, y + h);
}

HDWP SyncWindow::HandleWindowAnchor(HDWP& hdwp, const CRect& _rcSrcWindow, const SyncWindowData& rData)
{
    CRect rcSrcWindow(_rcSrcWindow);
    if (rData.m_nAnchorType == ANCHOR_CUSTOM)
    {
        return (HDWP)::SendMessage(rData.m_hWnd, 
            UI_WM_SYNC_WINDOWPOSCHANING, (WPARAM)hdwp, (LPARAM)&rcSrcWindow);
    }

    if (rData.m_nAnchorType & ANCHOR_CLIENTREGION)
    {
        SendMessage(m_hWnd, WM_NCCALCSIZE, TRUE, (LPARAM)&rcSrcWindow);
    }

    int x = 0, y = 0, w = 0, h = 0;
    int nFlag = SWP_NOZORDER | SWP_NOACTIVATE;

    CRect  rcThisWindow;
    ::GetWindowRect(rData.m_hWnd, &rcThisWindow);
    w = rcThisWindow.Width();
    h = rcThisWindow.Height();

    // 分析对齐方式
    bool bLeft = rData.m_nAnchorType&ANCHOR_LEFT ? true : false;
    bool bRight = rData.m_nAnchorType&ANCHOR_RIGHT ? true : false;
    bool bTop = rData.m_nAnchorType&ANCHOR_TOP ? true : false;
    bool bBottom = rData.m_nAnchorType&ANCHOR_BOTTOM ? true : false;

    bool bOutLeft = rData.m_nAnchorType&ANCHOR_OUT_LEFT ? true : false;
    bool bOutRight = rData.m_nAnchorType&ANCHOR_OUT_RIGHT ? true : false;
    bool bOutTop = rData.m_nAnchorType&ANCHOR_OUT_TOP ? true : false;
    bool bOutBottom = rData.m_nAnchorType&ANCHOR_OUT_BOTTOM ? true : false;

    if (bLeft && bRight)
    {
        x = rcSrcWindow.left + rData.m_rcAnchorData.xOffset;
        int xRight = rcSrcWindow.right - rData.m_rcAnchorData.xOffset2;
        w = xRight - x;
    }
    else
    {
        if (-1 != rData.m_rcAnchorData.Width)
            w = rData.m_rcAnchorData.Width;

        if (bLeft)
            x = rcSrcWindow.left + rData.m_rcAnchorData.xOffset;
        else if (bRight)
            x = rcSrcWindow.right - rData.m_rcAnchorData.xOffset - w;
        else if (bOutLeft)
            x = rcSrcWindow.left - rData.m_rcAnchorData.xOffset - w;
        else if (bOutRight)
            x = rcSrcWindow.right + rData.m_rcAnchorData.xOffset;
        else
            x = rcThisWindow.left;
    }

    if (bTop && bBottom)
    {
        y = rcSrcWindow.top + rData.m_rcAnchorData.yOffset;
        int yBottom = rcSrcWindow.bottom - rData.m_rcAnchorData.yOffset2;
        h = yBottom - y;
    }
    else
    {
        if (-1 != rData.m_rcAnchorData.Height)
            h = rData.m_rcAnchorData.Height;

        if (bTop)
            y = rcSrcWindow.top + rData.m_rcAnchorData.yOffset;
        else if (bBottom)
            y = rcSrcWindow.bottom - rData.m_rcAnchorData.yOffset - h;
        else if (bOutTop)
            y = rcSrcWindow.top - rData.m_rcAnchorData.yOffset - h;
        else if (bOutBottom)
            y = rcSrcWindow.bottom + rData.m_rcAnchorData.yOffset;
        else
            y = rcThisWindow.top;
    }
    if (nullptr == hdwp)
    {
        ::SetWindowPos(rData.m_hWnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
        return nullptr;
    }
    else
    {
        SendMessage(rData.m_hWnd, UI_WM_SYNC_WINDOW, HOST_WINDOW_POSCHANGING, 0);
        return ::DeferWindowPos(hdwp, rData.m_hWnd, nullptr, x, y, w, h, nFlag);
    }
}

void SyncWindow::_OnNcDestroy()
{
    // 自己被销毁时，检查跟随情况
    if (m_hHostWnd)  // 通知宿主窗口移除自己
    {
        SyncWindowData data;
        data.m_hWnd = GetMyHWND();
        ::SendMessage(m_hHostWnd, UI_WM_SYNC_WINDOW, REMOVE_SYNC_WINDOW, (LPARAM)&data);
    }

    // 通知其它跟随自己的窗口，宿主窗口销毁了
    this->ClearAnchorItem();
}

bool SyncWindow::AddAnchorItem(const SyncWindowData& data)
{
    if (nullptr == data.m_hWnd)
        return false;

    if (GetWindowLongPtr(data.m_hWnd, GWL_STYLE) & WS_CHILD)
        return false;

    if (-1 != this->FindAnchorItem(data.m_hWnd))
        return false;

    m_vecAnchorItems.push_back(data);
    ::SendMessage(data.m_hWnd, UI_WM_SYNC_WINDOW, ADD_SYNC_WINDOW_RESULT, (LPARAM)GetMyHWND());
    return true;
}

bool SyncWindow::RemoveAnchorItem(HWND hWnd)
{
    vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
    vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
    for (; iter != iterEnd; iter++)
    {
        if (iter->m_hWnd == hWnd)
        {
            m_vecAnchorItems.erase(iter);
            return true;
        }
    }
    ::SendMessage(hWnd, UI_WM_SYNC_WINDOW, REMOVE_SYNC_WINDOW_RESULT, (LPARAM)GetMyHWND());
    return false;
}

bool SyncWindow::ModifyAnchorItem(const SyncWindowData& data)
{
    int nIndex = this->FindAnchorItem(data.m_hWnd);
    if (-1 == nIndex)
        return false;

    if (data.m_nMask & SWDS_MASK_ANCHORON)
        m_vecAnchorItems[nIndex].m_bAnchorOn = data.m_bAnchorOn;
    if (data.m_nMask & SWDS_MASK_ANCHORTYPE)
        m_vecAnchorItems[nIndex].m_nAnchorType = data.m_nAnchorType;
    if (data.m_nMask & SWDS_MASK_ANCHORDATA)
        memcpy(&m_vecAnchorItems[nIndex].m_rcAnchorData, &data.m_rcAnchorData, sizeof(AnchorData));

    ::SendMessage(data.m_hWnd, UI_WM_SYNC_WINDOW, MODIFY_SYNC_WINDOW_RESULT, (LPARAM)GetMyHWND());
    return true;
}

void SyncWindow::OnAddAnchorItem(HWND hHostWnd) // 自己作为跟随者
{
    UIASSERT(nullptr == m_hHostWnd);
    m_hHostWnd = hHostWnd;
}

void SyncWindow::OnRemoveAnchorItem(HWND hHostWnd) // 自己作为跟随者
{
    UIASSERT(hHostWnd == m_hHostWnd);
    m_hHostWnd = nullptr;
}

void SyncWindow::OnModifyAnchorItem(HWND hHostWnd) // 自己作为跟随者
{
    UIASSERT(nullptr != m_hHostWnd);
}

bool SyncWindow::ClearAnchorItem()
{
    int nSize = (int)m_vecAnchorItems.size();
    for (int i = 0; i < nSize; i++)
    {
        ::SendMessage(m_vecAnchorItems[i].m_hWnd, UI_WM_SYNC_WINDOW, HOST_WINDOW_DESTROYED, (LPARAM)GetMyHWND());
    }
    m_vecAnchorItems.clear();
    return true;
}

int SyncWindow::FindAnchorItem(HWND hWnd)
{
    int nSize = (int)m_vecAnchorItems.size();
    for (int i = 0; i < nSize; i++)
    {
        if (m_vecAnchorItems[i].m_hWnd == hWnd)
        {
            return i;
        }
    }
    return -1;
}

int SyncWindow::GetAvailableHWNDCount() // 放弃判断是否可见。有时不可见也需要调整，要不然再显示时，位置就不正确了
{
    int nCount = 0;
    int nSize = (int)m_vecAnchorItems.size();
    for (int i = 0; i < nSize; i++)
    {
        HWND hWnd = m_vecAnchorItems[i].m_hWnd;
        if (hWnd /*&& ::IsWindowVisible(hWnd)*/ && m_vecAnchorItems[i].m_bAnchorOn)
        {
            nCount++;
        }
    }
    return nCount;
}

void SyncWindow::HideAllAnchorItem()
{
    int nSize = (int)m_vecAnchorItems.size() + 1;
    HDWP hdwp = ::BeginDeferWindowPos(nSize);

    hdwp = ::DeferWindowPos(hdwp, m_hWnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);

    vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
    vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
    for (; iter != iterEnd; iter++)
    {
        hdwp = ::DeferWindowPos(hdwp, iter->m_hWnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
    }

    ::EndDeferWindowPos(hdwp);
}

bool SyncWindow::IsSizeMoveIng()
{
    return m_bSizeMove;
}

HWND SyncWindow::GetMyHWND()
{
    return m_hWnd;
}

void SyncWindow::_OnExitSizeMove()
{
    m_bSizeMove = false;
}

void SyncWindow::_OnEnterSizeMove()
{
    m_bSizeMove = true;
}
