#pragma once
#include "panel.h"
#include "Src\Atl\image.h"

// 2016.4.1  libo 
// 实现圆角Panel，以尽量少的内存占用实现。
//
// 原理：
//   摒弃以前将内容画在一个单独的buffer上，然后对buffer做roundrect，最后将
//   该buffer提交到dc上的方法。
//
//   1. 在画panel内容之前，将四个圆角的父背景内容缓存起来；
//   2. 画panel内容；
//   3. 对panel的内容直接做roundrect，不启用新缓存；
//   4. 将panel的四个新圆角贴到之前缓存的父背景缓存上去，此时这个缓存上的
//      内容将是四个圆角的最终显示；
//   5. 将缓存上的四个圆角还原到dc上去；
//      完成
//

// 注：back.render的数据不会做圆角化，fore.render+child的数据会被圆角化

namespace UI
{

class RoundPanel : public Panel
{
public:
    RoundPanel(IRoundPanel*);
    ~RoundPanel();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
        UIMSG_POSTPAINT(OnPostPaint)
        UIMSG_QUERYINTERFACE(RoundPanel)
		UIMSG_SERIALIZE(OnSerialize)
	UI_END_MSG_MAP_CHAIN_PARENT(Panel)

public:
    void  PrePaint(HDC hDC, int width, int height);
    void  PostPaint(HDC hDC, int width, int height);

    void  SetRadius(
            ULONG lefttop, ULONG righttop,
            ULONG leftbottom, ULONG rightbottom);

protected:
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnPostPaint(IRenderTarget* pRenderTarget);

private:
    ULONG  m_lefttop;
    ULONG  m_righttop;
    ULONG  m_leftbottom;
    ULONG  m_rightbottom;

	IRoundPanel*  m_pIRoundPanel;
};

// 负责保存原始内容、更新panel圆角内容、将内容贴回到窗口上去
class BufferSwitcher
{
public:
    BufferSwitcher();
    ~BufferSwitcher();

    void  SaveLeftTop(HDC hDC, ULONG radius);
    void  SaveRightTop(HDC hDC, ULONG radius, int width);
    void  SaveLeftBottom(HDC hDC, ULONG radius, int height);
    void  SaveRightBottom(HDC hDC, ULONG radius, int width, int height);

    void  UpdateLeftTop(HDC hDC, ULONG radius);
    void  UpdateRightTop(HDC hDC, ULONG radius, int width);
    void  UpdateLeftBottom(HDC hDC, ULONG radius, int height);
    void  UpdateRightBottom(HDC hDC, ULONG radius, int width, int height);

    void  RestoreLeftTop(HDC hDC, ULONG radius);
    void  RestoreRightTop(HDC hDC, ULONG radius, int width);
    void  RestoreLeftBottom(HDC hDC, ULONG radius, int height);
    void  RestoreRightBottom(HDC hDC, ULONG radius, int width, int height);

    void  AddRef();
    void  Release();

private:
    void  save(UI::Image&, HDC hDC, ULONG size, int x, int y);
    void  update(UI::Image&, HDC hDC, ULONG size, int x, int y);
    void  restore(UI::Image&, HDC hDC, ULONG size, int x, int y);

private:
	UI::Image  m_lefttop;
	UI::Image  m_righttop;
	UI::Image  m_leftbottom;
	UI::Image  m_rightbottom;

    long  m_lRef;
};

}