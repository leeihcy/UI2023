#pragma once
#include "include/interface/iuiinterface.h"
#include "include/interface/ianimate.h"

namespace ui
{
	class WaitForHandlesMgr;
    class UIApplication;

	class AnimateHelper :
		public uia::IAnimateTimerCallback,
		public IWaitForHandleCallback
	{
	public:
		AnimateHelper(UIApplication& app);
		~AnimateHelper();

		void  Init(WaitForHandlesMgr* p);
		void  UnInit();

		uia::IAnimateManager*  GetAnimateManager();

	protected:
		// uia::IAnimateTimerCallback
		virtual void  OnSetTimer(long) override;
		virtual void  OnKillTimer(long) override;

		// IWaitForHandleCallback
		virtual void  OnWaitForHandleObjectCallback(long, long) override;

	private:
		WaitForHandlesMgr*  m_pWaitforHandle;
		uia::IAnimateManager*  m_pAnimateMgr;
		UIApplication&  m_uiApplication;
	};
}