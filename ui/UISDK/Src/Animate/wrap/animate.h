#pragma once
#include "Inc\Interface\iuiinterface.h"

namespace UI
{
	class WaitForHandlesMgr;

	class AnimateHelper :
		public UIA::IAnimateTimerCallback,
		public IWaitForHandleCallback
	{
	public:
		AnimateHelper(UIApplication& app);
		~AnimateHelper();

		void  Init(WaitForHandlesMgr* p);
		void  UnInit();

		UIA::IAnimateManager*  GetAnimateManager();

	protected:
		// UIA::IAnimateTimerCallback
		virtual void  OnSetTimer(HANDLE) override;
		virtual void  OnKillTimer(HANDLE) override;

		// IWaitForHandleCallback
		virtual void  OnWaitForHandleObjectCallback(HANDLE, LPARAM) override;

	private:
		WaitForHandlesMgr*  m_pWaitforHandle;
		UIA::IAnimateManager*  m_pAnimateMgr;
		UIApplication&  m_uiApplication;
	};
}