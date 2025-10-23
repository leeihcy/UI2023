#pragma once
#include <list>

// 用于管理顶层窗口，例如换肤

namespace ui
{
class Object;
class Window;
class ResourceBundle;
class Application;
struct ITopWindowManager;
struct IWindow;
struct UIMSG;

class TopWindowManager
{
public:
    TopWindowManager(Application* p);
    ~TopWindowManager();
    ITopWindowManager*  GetITopWindowManager();

    typedef std::list<Window*>  _MyList;
    typedef std::list<Window*>::iterator _MyIter;

	long  AddTopWindowObject(Window*);
	long  RemoveTopWindowObject(Window*);
	
	void  ClearTopWindowObject( );
	int   GetTopWindowCount() { return (int)m_lTopWindowObject.size(); }
#if 0 // defined(OS_WIN)
	IWindow* GetWindow(HWND hWnd); 
#endif
	// void  ChangeSkin(ResourceBundle* pNewSkinRes);
	bool  UpdateAllWindow();

    void  SendMessage2AllWnd(UIMSG* pMsg);
    //void  PostMessage2AllWnd(UIMSG* pMsg);
    void  ForwardMessage2AllObj(UIMSG*  pMsg);

protected:
	void  GetAllChildIntoList(Object* pParent, std::list<Object*>& listObjects);

private:
    ITopWindowManager*  m_pITopWindowManager;
	std::list<Window*>  m_lTopWindowObject;
    Application*  m_pUIApplication;
};
}

