#pragma once
#include "include/interface/iuiinterface.h"
#include "include/interface/ianimate.h"

namespace ui
{
	class WaitForHandlesMgr;
    class Application;

	class AnimateHelper :
		public uia::IAnimateTimerCallback,
		public IWaitForHandleCallback
	{
	public:
		AnimateHelper(Application& app);
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
		Application&  m_uiApplication;
	};
}