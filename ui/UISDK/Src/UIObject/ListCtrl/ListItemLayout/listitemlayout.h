#pragma once
#include "Inc/Interface/ilistctrlbase.h"

namespace UI
{
interface IListItemBase;
interface IListCtrlBase;
class ListCtrlBase;
class ListItemBase;

enum LISTCTRL_ITEM_LAYOUT_TYPE
{
    // 1. 普通系统listbox. [固定行高]
    //    滚动条：  横向NONE，纵向AUTO
    //    布  局：  横向占满client width，纵向依次排列
    //    AUTOSIZE: 横向0(不支持AUTO), 纵向total height
    LISTCTRL_ITEM_LAYOUT_TYPE_1,

    // 2. 普通系统listbox，支持横向滚动. [固定行高]
    //    滚动条：  横向AUTO，纵向AUTO
    //    布  局：  横向取max width，或者client width，纵向依次排列
    //    AUTOSIZE: 横向max width, 纵向total height
    LISTCTRL_ITEM_LAYOUT_TYPE_2,
};


class  ListCtrlLayoutMgr
{
public:
    ListCtrlLayoutMgr();
    ~ListCtrlLayoutMgr();
    
    void  SetListCtrlBase(ListCtrlBase* pListCtrlBase);

    IListCtrlLayout*  GetLayout();
    void  SetLayout(IListCtrlLayout* p);

    bool  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    void  Measure(SIZE* pSize);

private:
    ListCtrlBase*  m_pListCtrl;
    IListCtrlLayout*  m_pLayout;
};


class ListCtrlItemSimpleLayout : public IListCtrlLayout
{
public:
    ListCtrlItemSimpleLayout();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};

class ListCtrlItemVariableHeightLayout : public IListCtrlLayout
{
public:
    ListCtrlItemVariableHeightLayout();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


// TAB布局，横向平均排布
class ListCtrlItemHorzAverageLayout : public IListCtrlLayout
{
    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize){};
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


// 等高，流式布局，一行铺满则换行 （只有纵向滚动条）
class ListCtrlItemFixHeightFlowLayout : public IListCtrlItemFixHeightFlowLayout
{
public:
	ListCtrlItemFixHeightFlowLayout();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

	virtual void  SetLineSpace(int) override;
	virtual void  SetItemSpace(int) override;

	int  GetLineSpace();
	int  GetItemSpace();

protected:
    IListCtrlBase*  m_pIListCtrlBase;

	int  m_nItemSpace;
	int  m_nLineSpace;
};



// 横向与纵向同时布局，横向无滚动条，纵向有滚动条
// 如listview的icon视图模式
class ListCtrlItemVariableHeightFlowLayout : public IListCtrlLayout
{
public:
    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }      
protected:
    IListCtrlBase*  m_pIListCtrlBase;

private:
    // 当前行的列表项集合。在换行时，才能计算出这个列表项的实现位置。
    // 例如每个item的高度可能不一样，要取到最大高度才行。
    class CurrentLineItems
    {
    public:
        CurrentLineItems() 
        {
            m_nMaxHeight = 0; m_nContentWidth = 0; m_nContentHeight = 0;
            m_hSpace = m_vSpace = 0;
            m_nCtrlWidth = 0;
            m_nxCursor = 0;
            m_nyCursor = 0;
        }
        ~CurrentLineItems() 
        {
            UIASSERT(m_vecItems.empty());
        }
        void  AddItem(ListItemBase*, int w, int h);
        void  AddSingleLineItem(ListItemBase*, int h);
        void  CommitLine();
        void  GetContentSize(SIZE* p)
        {
            if (p) { p->cx = m_nContentWidth; p->cy = m_nContentHeight; }
        }
        void  SetHSpace(int n) { m_hSpace = n; }
        void  SetVSpace(int n) { m_vSpace = n; }
        void  SetCtrlWidth(int n) { m_nCtrlWidth = m_nContentWidth = n; }
        void  SetXYCursor(int x, int y) { m_nxCursor = x; m_nyCursor = y; }
    private:
        struct ListItemData
        {
            ListItemBase* pItem;
            int x;
            int w;
        };
        vector<ListItemData>  m_vecItems;
        int  m_nMaxHeight;
        int  m_nxCursor;  // 当前x的位置游标
        int  m_nyCursor;  // 当前y的位置游标

        int  m_hSpace;
        int  m_vSpace;
        int  m_nCtrlWidth;

        // 最终内容的大小与高度
        int  m_nContentWidth;
        int  m_nContentHeight;
    };

};

IListCtrlLayout*  CreateListCtrlLayout(int nType, IListCtrlBase* p);
}
