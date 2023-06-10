#pragma once
#include "Src/Base/Message/message.h"
#include "Inc/Interface/ianimate.h"

//////////////////////////////////////////////////////////////////////////
//              惯性滚动
//
//  旧的滚动逻辑，已废弃：
//   1. 以m_nScroll_V0作为初速度做减速运动，最终速度到达0停止
//   2. 每一次MOUSEWHEEL时，将速度重置为m_nScroll_V0
//   3. 将滚动能量作为路程
//   4. 每一次MOUSEWHEEL时，累加所需要的路程
//   5. 时间不需要，2as=v^2，没有时间参数
//
//  新的滚动逻辑：
//   1. 采用减速的缓动算法
//   2. 每次滚动时间固定
//   3. 滚动过程中如果再次触发mousewheel，则更新动画的from/to/duration参数
//     （不是销毁再创建）
//
//////////////////////////////////////////////////////////////////////////

//  1. 回弹时，如何记录当前已回弹的距离？要不然在回弹过程中再触发一次
//     mouse wheel怎么设置动画参数？
//     ：给callback再加一个接口，用于获取超出的滚动值
//       再次触发时，使用sin三角函数重新计算从当前位置到最高点所需要的动画
//       时长
//
//
//
// TODO: 仿ios边界弹性滚动动画 m_bAnimateOverflow
//  2014/8/27 
//  . 在动画过程中点出鼠标左键，会停止当前滚动。如何处理
//
//  2015/9/2
//  . 在触摸屏上利用UpdatePanningFeedback API 实现了整个窗口的边缘回弹，
//    但是类ios的动画效果还是没想到要怎么实现
//
// bouncy edge
// 
// 橡皮筋动画
// Rubber band animation
// 弹跳动画
// Bounce animation
//
namespace UI
{

interface  IStoryboard;

enum SmoothScrollResult
{
    INERTIA_SCROLL_STOP = 0,      // 立即结束
    INERTIA_SCROLL_CONTINUE = 1,  // 继续滚动
    INERTIA_SCROLL_BOUNCE_EDGE = 2,  // 溢出了
};

enum MOUSEWHEEL_DIR 
{
	MOUSEWHEEL_DOWN, 
	MOUSEWHEEL_UP
};
interface ISmoothScrollCallback
{
    virtual  void SmoothScroll_Start() = 0;
    virtual  void SmoothScroll_Stop() = 0;

	// 平滑滚动阶段，nDeltaPos为偏移量
    virtual  SmoothScrollResult SmoothScroll_Scroll(
				MOUSEWHEEL_DIR eDir, uint nDeltaPos) = 0;

	// 边缘回弹阶段，nBounceHeight为相对于边缘的绝对值
	virtual  SmoothScrollResult SmoothScroll_BounceEdge(
				MOUSEWHEEL_DIR eDir, uint nBounceHeight) = 0;

	// 获取当前滚动条滚动的弹性高度。
	// 目的：
	// 1. 用于连续回弹中获取新的开始位置 
	// 2. 用于回弹返回阶段获取from值
	// 如果返回值小于0表示向上滚动了。大于0表示向下滚动了
	virtual  int  SmoothScroll_GetScrolledBounceHeight() = 0;
};

class SmoothScroll : public UIA::IAnimateEventCallback
{
public:
    SmoothScroll();
    ~SmoothScroll();


public:
    void  SetUIApplication(UIApplication* p);
    void  SetCallback(ISmoothScrollCallback* pCallback);
    bool  IsEnable();
    void  SetEnable(bool b);
    bool  IsScrolling();
    void  AddPower(int zDelta, int nViewPage);
    void  StopScroll();

protected:
	enum AnimateType
	{
		ScrollAnimate,
		BouncyEdgeAnimate_Out,  // 弹出去
		BouncyEdgeAnimate_Back, // 弹回来
	};

    virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(UIA::IStoryboard*) override;
	void  OnTick_ScrollAnimate();
	void  OnTick_BounceEdgeAnimate();

	void  OnScrollStop();
    void  CreateScrollAnimate(uint nPower);
	void  CreateBouncyEdgeOutAnimate(uint nPower);
	void  CreateBouncyEdgeBackAnimate();
    void  DestroyAnimate();

private:
    UIApplication*  m_pUIApplication;
    ISmoothScrollCallback*  m_pCallback;

	// 从下至下滚动，还是从下至上
	MOUSEWHEEL_DIR  m_eMouseWheelDir;

    bool  m_bEnable;
    UIA::IStoryboard*  m_pAnimateStoryboard;

	// 每个mousewheel滚动距离
	int  m_nStep;         

	// 边缘回弹大小
	int  m_nMaxBounceEdge;
};

}