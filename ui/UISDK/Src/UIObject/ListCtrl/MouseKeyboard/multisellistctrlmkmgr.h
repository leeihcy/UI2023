#pragma once
#include "listctrlmkmgr.h"

namespace UI
{


class ISelectRegionRender
{
public:
    ISelectRegionRender()
    {
        m_pObject = nullptr;
        m_rcSelectRegion.SetRectEmpty();
    }
    virtual ~ ISelectRegionRender() = 0 {};

public:
    virtual bool Init(Object*  pObj, POINT pt)
    {
        m_pObject = pObj;
        return true;
    }
    virtual void MouseMove(POINT pt) = 0;
    virtual void LButtonUp() = 0;
    // virtual void PrePaint() = 0;
    // virtual void PostPaint() = 0;
    virtual void GetSelectRegion(RECT* lprc) 
    {
        if (lprc)
            ::CopyRect(lprc, &m_rcSelectRegion);
    }

    RECT    NormalizeRect(RECT* prc)
    {
        RECT rc;
        CopyRect(&rc, prc);
        if (rc.left > rc.right)
        {
            rc.left = rc.right;
            rc.right = prc->left;
        }
        if (rc.top > rc.bottom)
        {
            rc.top = rc.bottom;
            rc.bottom = prc->top;
        }
        return rc;
    }
protected:
    Object*  m_pObject;
    CRect    m_rcSelectRegion;
};

class MultiSelListCtrlMKMgr : public ListCtrlMKMgrBase
{
public:
    MultiSelListCtrlMKMgr();
    ~MultiSelListCtrlMKMgr();

protected:
    virtual BOOL  DoProcessMessage(UIMSG* pMsg);

protected:
    void  OnMouseMove(UIMSG* pMsg);
    void  OnLButtonDown(UIMSG* pMsg);
    void  OnLButtonUp(UIMSG* pMsg);
    void  OnMouseLeave(UIMSG* pMsg);
    void  OnRButtonDown(UIMSG* pMsg);

    void  OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg);
    void  OnKeyDown_down(UIMSG* pMsg);
    void  OnKeyDown_up(UIMSG* pMsg);
    void  OnKeyDown_space(UIMSG* pMsg);
    void  OnKeyDown_prior(UIMSG* pMsg);
    void  OnKeyDown_next(UIMSG* pMsg);
    void  OnKeyDown_home(UIMSG* pMsg);
    void  OnKeyDown_end(UIMSG* pMsg);

    void  OnMouseMove_DragSelectRegion(UIMSG* pMsg);

protected:
    ISelectRegionRender*  m_pSelectRegionRender;
};




// XP下的虚线框选择
// 该样式不适用于分层窗口。因为该样式需要直接提交到窗口上面，但由于不需要刷新，其效率是最高的。
//
// 2013.1.7 注
//
//   本来是想实现无闪烁的虚线选择，分两次DrawFocusRect是会导致闪烁的，利用Rectangle/Polyline闪烁更严重。
//   但尝试了一些方法后最后放弃了。（例如利用polyline只画这一次增加和需要擦除的虚线，但不好对齐虚线点，
//   利用CLIP RGN，计算有些复杂，利用WM_REDRAW行不通，利用...)
//   以后有机遇再考虑吧。
//
class DotLine_SelectRegionRender : public ISelectRegionRender//, ICommitWindowBufferListener
{
public:
    DotLine_SelectRegionRender();
    ~DotLine_SelectRegionRender();

    virtual bool Init(Object* pObj, POINT pt);
    virtual void MouseMove(POINT point);
    virtual void LButtonUp();
//     virtual void PrePaint();
//     //	virtual void PostPaint();
// 
//     virtual void  PreCommitWindowBuffer(HDC hDC, HDC hMemDC, IWindowBase* pWindow, RECT* prcCommit, int nRectCount);
//     virtual void  PostCommitWindowBuffer(HDC hDC, HDC hMemDC, IWindowBase* pWindow, RECT* prcCommit, int nRectCount);
//     virtual bool  OnlyListenOnce() { return true; } 

    void  Draw(RECT* prc);

protected:
    HWND    m_hWnd;
    HDC     m_hWndDC;

    CRect   m_rcClip;
    HRGN    m_hClipRgn;

    CRect   m_rcView;
};
class AlphaRect_SelectRegionRender : public ISelectRegionRender
{
public:
};


}