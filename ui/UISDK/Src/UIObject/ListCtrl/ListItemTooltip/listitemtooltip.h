#pragma once

namespace UI
{
class  ListItemBase;

enum LISTITEMTOOLTIPMODE
{
    LITM_NONE,    // 不启用
    LITM_SINGLE,  // 单行提示
    LITM_MULTI,   // 多区域提示
    LITM_CUSTOM,  // 自定义。自己处理tooltip消息
};

struct ListITemToolTipInfo
{
    long  lId;
    RECT  region;

    String*  pTextRef;
};

class ListITemToolTipMgr
{
public:
    ListITemToolTipMgr();
    ~ListITemToolTipMgr();
    void  SetListItem(ListItemBase* pItem);

    BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapId, bool bDoHook);

    bool  AddToolTip(ListITemToolTipInfo* pInfo);
    bool  FindToolTip(long lId, __out_opt int* pIndex = nullptr);
    bool  RemoveToolTip(long lId);
    bool  ModifyToolTip(ListITemToolTipInfo* pInfo);

    void  SetToolTip(LPCTSTR szText);
    LPCTSTR  GetToolTip();
    void  ShowSingleToolTip();
    void  SetToolTipMode(LISTITEMTOOLTIPMODE eMode);

protected:
    LRESULT  OnGetToolTipInfo(WPARAM wParam, LPARAM lParam);
    
    void  OnMouseMove(WPARAM wParam, LPARAM lParam);
    void  OnMouseLeave();

protected:
    int  HitTest(POINT ptClient);

private:
    ListItemBase*  m_pListItem;
    IUIApplication*  m_pUIApp;

    int   m_nActiveIndex;

    LISTITEMTOOLTIPMODE  m_eMode;

    // 简单模式
    String  m_strToolTip;

    // 复杂模式
    typedef vector<ListITemToolTipInfo>::iterator _MyIter;
    vector<ListITemToolTipInfo>  m_vecToolTips;
};

}