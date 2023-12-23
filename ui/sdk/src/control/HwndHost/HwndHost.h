#pragma  once
#include "Src\UIObject\Control\control.h"
#include "include/interface/ihwndhost.h"

namespace ui
{

//
//  ��װһ��ԭʼ��HWND����
//
// [2013.12.16] HwndHostҲ���ܰ�װһ��panel�µ��Ӷ��������װһ��IEWrap
//
//
class HwndHost : public Control
{
public:
	HwndHost(IHwndHost* p);
	~HwndHost();

	UI_BEGIN_MSG_MAP()
		UIMSG_VISIBLE_CHANGED(OnVisibleChanged)
		UIMSG_HITTEST(OnHitTest)
        UIMSG_QUERYINTERFACE(HwndHost)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
		UIMSG_SERIALIZE(OnSerialize)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

	void  Attach(HWND hWnd);
	HWND  Detach();

protected:
	LONG_PTR  OnHitTest(POINT* pt, POINT*  ptInChild);
	void  OnVisibleChanged(BOOL bVisible, IObject* pObjChanged);
	void  OnSerialize(SerializeParam* pData);
    void  GetDesiredSize(SIZE* pSize);

	virtual void  SetObjectPos( int x, int y, int cx, int cy, int nFlag=0) override;
	virtual void  virtualOnMove() override;
	virtual void  virtualSetVisibleEx(VISIBILITY_TYPE eType) override;
	virtual void  virtualSetEnable(bool b) override;
	virtual bool  IsVisible() override;
	virtual bool  IsEnable() override;

public:
	bool  SubclassWindow();
	bool  UnSubclassWindow();
	void  OnSubclassWindow();
	void  OnUnsubclassWindow();

	void  SyncPosition();

protected:
	static long CALLBACK  _WndProc( HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam );
	long  WndProc( unsigned int uMsg, WPARAM wParam, LPARAM lParam );
	long  DefWindowProc( unsigned int uMsg, WPARAM wParam, LPARAM lParam );

    void  UpdateTabStopStyle();
public:
    IHwndHost*  m_pIHwndHost;
	HWND  m_hWnd;

protected:
	CWndProcThunk  m_thunk;   // ATL�е�THUNK�����ڽ�һ�����ڹ��������Լ��ĳ�Ա����
	WNDPROC  m_oldWndProc;    // �ô��ڵ�ԭʼ���ڹ���
};

}