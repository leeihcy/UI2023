#ifndef _INCLUDED_UIAUTOTEST_INC_H_
#define _INCLUDED_UIAUTOTEST_INC_H_

#if 0 // defined(OS_WIN)
    #ifdef UIAUTOMATOR_EXPORTS
    #define UIAUTOMATOR_API __declspec(dllexport)
    #else
    #define UIAUTOMATOR_API __declspec(dllimport)
    #endif
#else
    #define UIAUTOMATOR_API
#endif

namespace ui
{
	struct IWindow;

	struct IUIAutoTest
	{
		virtual void Init() = 0;
		virtual void Release() = 0;

		virtual void OnWindowInit(ui::IWindow*) = 0;
	};


	extern "C" {
		UIAUTOMATOR_API IUIAutoTest*  GetUIAutoTest();
	}

}

#endif