#pragma once
#include "..\wndtransmodebase.h"
#include "Src\Atl\image.h"
#include "..\anti\antiwrap.h"

// TODO: 未完事项
// 1. 阴影的过渡不像vs2013那么自然
// 2. 窗口的圆角未处理
// 3. 窗口的显隐未处理
// 4. 窗口的最大化最小化未处理

namespace UI
{
	enum GLOW_WINDOW_ALIGN
	{
		GLOW_WINDOW_LEFT,
		GLOW_WINDOW_TOP,
		GLOW_WINDOW_RIGHT,
		GLOW_WINDOW_BOTTOM,
		GLOW_WINDOW_LTRB,
	};

class GlowWindow : public CWindowImpl < GlowWindow >
{
public:
	GlowWindow();
	~GlowWindow();

	BEGIN_MSG_MAP(GlowWindow)
		MSG_WM_SIZE(OnSize)
	END_MSG_MAP()

	void  Create(HWND hWnd, GLOW_WINDOW_ALIGN eAlign);
	void  Destroy();
    void  render();
	void  commit();

private:
	void OnSize(UINT nType, CSize size);

private:
	Image  m_buffer;
	GLOW_WINDOW_ALIGN  m_eAlign;
	int  m_nGlowSize;
	COLORREF  m_color;
};

class GlowWindowWrap : public AntiWindowWrap
{
public:
    GlowWindowWrap();
    ~GlowWindowWrap();

    virtual WINDOW_TRANSPARENT_TYPE  GetType()
    {
        return WINDOW_TRANSPARENT_TYPE_GLOW;
    }

    virtual BOOL  ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);
	virtual void  Enable(bool b);

public:
    void  Show();
    void  Hide();
private:
	GlowWindow  m_topGW;
	GlowWindow  m_leftGW;
	GlowWindow  m_rightGW;
	GlowWindow  m_bottomGW;

};

}