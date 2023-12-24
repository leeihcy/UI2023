// #pragma once
// #include "include/interface/ianimate.h"
// #include "include/interface/iuiinterface.h"

// namespace ui {
// class WaitForHandlesMgr;
// class Application;

// class AnimateWrap : public uia::IAnimateTimerCallback
// #if 0
// 		, public IWaitForHandleCallback
// #endif
// {
// public:
//   AnimateWrap(Application &app);
//   ~AnimateWrap();

//   uia::IAnimate *GetAnimate();

// protected:
//   // uia::IAnimateTimerCallback
//   virtual void OnSetTimer(int) override;
//   virtual void OnKillTimer(int) override;
// #if 0
// 		// IWaitForHandleCallback
// 		virtual void  OnWaitForHandleObjectCallback(long, long) override;
// #endif
// private:
//   uia::IAnimatePtr m_animate;
//   Application &m_uiApplication;
// };
// } // namespace ui