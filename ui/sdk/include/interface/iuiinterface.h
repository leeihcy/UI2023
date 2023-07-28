#ifndef _IUIINTERFACE_H_
#define _IUIINTERFACE_H_
#include "include/interface/imessage.h"

// IApplication接口
namespace ui
{
	struct  IToolTipUI;
	struct  IWindow;
	struct  IResourceManager;
	struct  IResBundle;
	struct     TOOLTIPITEM;
	struct  IRenderBitmap;
	struct  IRenderFont;
	struct  ISkinConfigRes;
	struct  ISkinConfigManager;
	struct  ITopWindowManager;
	struct  IWaitForHandlesMgr;
	struct  IRenderTarget;
	struct  IMessageFilterMgr;
    struct  IMapAttribute;
    struct  IObject;

	// 句柄定义
	struct IApplication;

    class TopWindowManager;
    struct UIAPI ITopWindowManager : public IRootInterface
    {
        ITopWindowManager(TopWindowManager* p);
        TopWindowManager* GetImpl();

        long  AddTopWindowObject(IWindow*);
        long  RemoveTopWindowObject(IWindow*);
        //void     ChangeSkin(IResBundle* pNewSkinRes);

        bool  UpdateAllWindow();
        void  SendMessage2AllWnd(UIMSG*  pMsg);
        //void  PostMessage2AllWnd(UIMSG* pMsg);
        void  ForwardMessage2AllObj(UIMSG*  pMsg);

    private:
        TopWindowManager*  m_pImpl;
    };

    
    //////////////////////////////////////////////////////////////////////////

	//
	// 各种提示条绘制的抽象类	
	//
	struct IToolTipUI
	{
	public:
		virtual ~IToolTipUI() {};
		virtual bool  Create()  = 0;
		virtual bool  Destroy() = 0;
		virtual bool  SetText(const wchar_t* szText) = 0;
		virtual bool  SetTitle(const wchar_t* szText) = 0;
		virtual bool  Show(HWND hWndParent) = 0;
		virtual bool  Hide() = 0;
		virtual bool  SetUIApplication(IApplication* p) = 0;
		virtual void  OnSerialize(SERIALIZEDATA* pData) = 0;
	};

	struct IWaitForHandleCallback
	{
		virtual void OnWaitForHandleObjectCallback(/*HANDLE*/long, long) = 0;
	};
	struct IWaitForHandlesMgr : public IRootInterface
	{
		virtual bool  AddHandle(/*HANDLE*/long h, IWaitForHandleCallback* pCB, long l) = 0;
		virtual bool  RemoveHandle(/*HANDLE*/long h) = 0;
	};

	struct IFlashInvalidateListener
	{
		virtual void OnInvalidateRect(const RECT*, bool) = 0;
	};
	struct IPreTranslateMessage  // IMessageFilter 被系统其它地方已经定义过了
	{
	public:
		virtual bool PreTranslateMessage(ui::MSG* pMsg) = 0;
	};

	struct IMessageFilterMgr : public IRootInterface
	{
		virtual void  AddMessageFilter(IPreTranslateMessage* p) = 0;
		virtual void  RemoveMessageFilter(IPreTranslateMessage* p) = 0;
	};

	
}

#endif // _IUIINTERFACE_H_