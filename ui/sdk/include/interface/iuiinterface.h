#ifndef _IUIINTERFACE_H_
#define _IUIINTERFACE_H_

// IUIApplication�ӿ�
namespace ui
{
	interface  IToolTipUI;
	interface  IWindowBase;
	interface  ISkinManager;
	interface  ISkinRes;
	struct     TOOLTIPITEM;
	interface  IRenderBitmap;
	interface  IRenderFont;
	interface  ISkinConfigRes;
	interface  ISkinConfigManager;
	interface  ITopWindowManager;
	interface  IWaitForHandlesMgr;
	interface  IRenderTarget;
	interface  IMessageFilterMgr;
    interface  IMapAttribute;
    interface  IObject;

	// �������
	interface IUIApplication;

    class TopWindowManager;
    interface UIAPI ITopWindowManager : public IRootInterface
    {
        ITopWindowManager(TopWindowManager* p);
        TopWindowManager* GetImpl();

        long  AddTopWindowObject(IWindowBase*);
        long  RemoveTopWindowObject(IWindowBase*);
        //void     ChangeSkin(ISkinRes* pNewSkinRes);

        bool  UpdateAllWindow();
        void  SendMessage2AllWnd(UIMSG*  pMsg);
        //void  PostMessage2AllWnd(UIMSG* pMsg);
        void  ForwardMessage2AllObj(UIMSG*  pMsg);

    private:
        TopWindowManager*  m_pImpl;
    };

    
    //////////////////////////////////////////////////////////////////////////

	//
	// ������ʾ�����Ƶĳ�����	
	//
	interface IToolTipUI
	{
	public:
		virtual ~IToolTipUI() = 0 {};
		virtual bool  Create()  = 0;
		virtual bool  Destroy() = 0;
		virtual bool  SetText(const wchar_t* szText) = 0;
		virtual bool  SetTitle(const wchar_t* szText) = 0;
		virtual bool  Show(HWND hWndParent) = 0;
		virtual bool  Hide() = 0;
		virtual bool  SetUIApplication(IUIApplication* p) = 0;
		virtual void  OnSerialize(SERIALIZEDATA* pData) = 0;
	};

	interface IWaitForHandleCallback
	{
		virtual void OnWaitForHandleObjectCallback(HANDLE, long) = 0;
	};
	interface IWaitForHandlesMgr : public IRootInterface
	{
		virtual bool  AddHandle(HANDLE h, IWaitForHandleCallback* pCB, long l) = 0;
		virtual bool  RemoveHandle(HANDLE h) = 0;
	};

	interface IFlashInvalidateListener
	{
		virtual void OnInvalidateRect(const RECT*, bool) = 0;
	};

	interface IPreTranslateMessage  // IMessageFilter ��ϵͳ�����ط��Ѿ��������
	{
	public:
		virtual bool PreTranslateMessage(MSG* pMsg) = 0;
	};

	interface IMessageFilterMgr : public IRootInterface
	{
		virtual void  AddMessageFilter(IPreTranslateMessage* p) = 0;
		virtual void  RemoveMessageFilter(IPreTranslateMessage* p) = 0;
	};

	
}

#endif // _IUIINTERFACE_H_