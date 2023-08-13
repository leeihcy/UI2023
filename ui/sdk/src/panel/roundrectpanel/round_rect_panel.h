#pragma once
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

class PanelRoundRectHandler
{
public:
    PanelRoundRectHandler();
    ~PanelRoundRectHandler();

    void  PrePaint(HDC hDC, int width, int height);
    void  PostPaint(HDC hDC, int width, int height);

    void  SetRadius(
            ULONG lefttop, ULONG righttop,
            ULONG leftbottom, ULONG rightbottom);

private:
    ULONG  m_lefttop;
    ULONG  m_righttop;
    ULONG  m_leftbottom;
    ULONG  m_rightbottom;
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
    void  save(UI::CImage&, HDC hDC, ULONG size, int x, int y);
    void  update(UI::CImage&, HDC hDC, ULONG size, int x, int y);
    void  restore(UI::CImage&, HDC hDC, ULONG size, int x, int y);

private:
    UI::CImage  m_lefttop;
    UI::CImage  m_righttop;
    UI::CImage  m_leftbottom;
    UI::CImage  m_rightbottom;

    long  m_lRef;
};