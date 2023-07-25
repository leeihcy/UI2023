#ifndef _UI_ISCROLLBARMANAGER_H_
#define _UI_ISCROLLBARMANAGER_H_
#include "imessage.h"

namespace ui
{

#define UISIF_RANGE           0x0001
#define UISIF_PAGE            0x0002
#define UISIF_POS             0x0004
#define UISIF_DISABLENOSCROLL 0x0008
#define UISIF_BUTTONLINE      0x0010
#define UISIF_WHEELLINE       0x0020

typedef struct tagUISCROLLINFO
{
    tagUISCROLLINFO(){ nMask = nRange = nPage = nPos = 0; nButtonLine = nWheelLine = 10; }
    int  nMask;
    int  nRange;
    int  nPos;
    int  nPage;
    int  nButtonLine;
    int  nWheelLine;
}UISCROLLINFO, *LPUISCROLLINFO;

// 存在两种场景：
//-----------------------------------------------------------------
// ScrollBarManager通知滚动条参数改变的消息
//  wParam: UISCROLLINFO
//  lParam: bool NotifyHScrollInfoChanged 是否需要更新自己的显示状态
//  lRet: 0(), 
//        1(需要重新计算客户区域，如自己的显隐发生改变)
//-----------------------------------------------------------------
// ScrollBarManager通知绑定对象滚动条参数改变了
//  wParam: VSCROLLBAR/HSCROLLBAR
//------------------------------------------------------------------  
#define UI_WM_SCROLLBAR_INFO_CHANGED  136081044

// 通知列表控件滚动条显示了/隐藏了
#define  UI_WM_SCROLLBAR_VISIBLE_CHANGED  151121646

// ScrollBarManager通知滚动条控件大小发生变化，需要更新滚动条的位置
#define UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED  136091826
// 通知控件列表的hover样式改变
#define  UI_WM_SCROLLBAR_BINDOBJHOVER_CHANGED 148261005


class ScrollBarManager;
interface UIAPI IScrollBarManager : public IMessage
{
    IScrollBarManager(ScrollBarManager*);
    ~IScrollBarManager();
    ScrollBarManager*  GetImpl();

    static IScrollBarManager*  CreateIScrollBarManager();
    static void  DestroyIScrollBarManager(IScrollBarManager* p);

    void  SetBindObject(IObject* pBindObj);
    IObject*  GetBindObject();

    void  SetHScrollBar(IMessage* p);
    void  SetVScrollBar(IMessage* p);
    IMessage*  GetHScrollBar();
    IMessage*  GetVScrollBar();

    void  SetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eDirType, SCROLLBAR_VISIBLE_TYPE eVisType);
    SCROLLBAR_VISIBLE_TYPE GetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eType);

    virtual BOOL  virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook); 
    void  Serialize(SERIALIZEDATA* pData);
    void  FireScrollMessage(SCROLLBAR_DIRECTION_TYPE eType, int nCode, int nTrackPos = 0);

    // Set
    void  SetVScrollInfo(UISCROLLINFO* lpsi);

    void  SetHScrollPos(int nPos);
    void  SetVScrollPos(int nPos);

    void  SetVScrollLine(int nLine);
    void  SetHScrollLine(int nLine);
    void  SetVScrollWheel(int nWheel);
    void  SetHScrollWheel(int nWheel);

    void  SetScrollRange(int nX, int nY);
    void  SetHScrollRange(int nX);
    void  SetVScrollRange(int nY);
    void  SetScrollPageAndRange(const SIZE* pPage, const SIZE* pRange);

    // Get
    void  GetScrollInfo(SCROLLBAR_DIRECTION_TYPE eDirType, UISCROLLINFO* pInfo);

    void  GetScrollPos(int* pX, int* pY);
    int   GetScrollPos(SCROLLBAR_DIRECTION_TYPE eDirType);
    int   GetHScrollPos();
    int   GetVScrollPos();

    void  GetScrollPage(int* pX, int* pY);
    int   GetScrollPage(SCROLLBAR_DIRECTION_TYPE eDirType);
    int   GetHScrollPage();
    int   GetVScrollPage();
    void  SetVScrollPage(int nY);

    void  GetScrollRange(int *pX, int* pY);
    int   GetScrollRange(SCROLLBAR_DIRECTION_TYPE eDirType);
    int   GetHScrollRange();
    int   GetVScrollRange();

	bool  IsVScrollBarAtBottom();
    bool  DoPageDown();
    bool  DoPageUp();
    bool  DoLineDown();
    bool  DoLineUp();
    bool  DoHome();
    bool  DoEnd();

    bool  IsSmoothScrolling();

private:
    ScrollBarManager*  m_pImpl;
};

}

#endif // _UI_ISCROLLBARMANAGER_H_