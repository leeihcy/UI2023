#ifndef _INCLUDED_UIAUTOTEST_INC_H_
#define _INCLUDED_UIAUTOTEST_INC_H_

#ifdef UIAUTOMATOR_EXPORTS
#define UIAUTOMATOR_API __declspec(dllexport)
#else
#define UIAUTOMATOR_API __declspec(dllimport)
#endif

namespace ui
{
	struct IWindow;

	struct IUIAutoTest
	{
		virtual void Init() = 0;
		virtual void Release() = 0;

		virtual void OnWindowInit(UI::IWindow*) = 0;
	};


	extern "C" {
		UIAUTOMATOR_API IUIAutoTest*  GetUIAutoTest();
	}

}

#endif