#pragma once
#include <list>

// 用于管理顶层窗口，例如换肤

namespace ui
{
class Object;
class Window;
class SkinRes;
class UIApplication;
struct ITopWindowManager;
struct IWindowBase;

class TopWindowManager
{
public:
    TopWindowManager(UIApplication* p);
    ~TopWindowManager();
    ITopWindowManager*  GetITopWindowManager();

    typedef std::list<Window*>  _MyList;
    typedef std::list<Window*>::iterator _MyIter;

	long  AddTopWindowObject(Window*);
	long  RemoveTopWindowObject(Window*);
	
	void  ClearTopWindowObject( );
	int   GetTopWindowCount() { return (int)m_lTopWindowObject.size(); }
	IWindowBase* GetWindowBase(HWND hWnd); 
				
	// void  ChangeSkin(SkinRes* pNewSkinRes);
	bool  UpdateAllWindow();

    void  SendMessage2AllWnd(UIMSG* pMsg);
    //void  PostMessage2AllWnd(UIMSG* pMsg);
    void  ForwardMessage2AllObj(UIMSG*  pMsg);

protected:
	void  GetAllChildIntoList(Object* pParent, std::list<Object*>& listObjects);

private:
    ITopWindowManager*  m_pITopWindowManager;
	std::list<Window*>  m_lTopWindowObject;
    UIApplication*  m_pUIApplication;
};
}

