#pragma once
#include <list>
#include "sdk/include/interface/ianimate.h"

namespace ui {
class Application;
}

namespace uia {
// http://msdn.microsoft.com/zh-cn/magazine/cc163397.aspx  扩展 WPF 动画类
// http://www.cnblogs.com/Jax/archive/2008/03/26/1123778.html 《Programming
// WPF》翻译 第8章 动画

// http://msdn.microsoft.com/en-us/library/system.windows.media.animation.doubleanimationbase_members(v=vs.85).aspx
// DoubleAnimationBase Members

// http://gamebabyrocksun.blog.163.com/blog/static/571534632010219544464/
// 游戏引擎循环机制

/*  motion blur<<运动模糊>>

● 游戏中为什么需要模糊处理？清晰一点不是更好吗？

        为什么每秒24帧的速度对于电影来说已经足以获得很流畅的视觉效果，而
        对于电脑游戏来说却会显得磕磕碰碰呢？原因很简单，摄像机在工作的时
        候并非一帧一帧静止的拍摄，它所摄下的每一帧已经包含了1/24秒以内的
        所有视觉信息，包括物体的位移。如果在看录像的时候按下暂停键，我们
        得所到的并不是一幅清晰的静止画面，而是一张模糊的图像，原因就在于
        此。电脑做不到这一点，游戏里的每一帧就是一幅静止画面，如果你在运
        动的过程中抓一张图片下来，得到的肯定是一幅清晰的静态图。运动模糊
        技术的目的就在于增强快速移动场景的真实感，这一技术并不是在两帧之
        间插入更多的位移信息，而是将当前帧同前一帧混合在一起所获得的一种
        效果。
*/

/*
Google Material Design · Motion

https://material.io/guidelines/motion/movement.html#movement-movement-within-screen-bounds
https://material.google.com/motion/material-motion.html


运动中的物质受到类似于真实世界的力的力，例如重力。

向上弧线：
在现实世界中反对重力需要努力。
在屏幕上向上移动的元素应该类似地描绘通过较慢向上移动的加速期间的努力。
当向斜上方移动时，从浅上升开始，以陡峭的上升结束。
不要使用陡峭的上升开始元素的向上移动。

向下弧线：
现实世界的下降元素被重力加速。
在屏幕上向下移动的元素应该通过更快的向下移动来描绘更少的努力。
当对角线向下移动时，下降应开始陡峭，并且浅浅地结束。

不要以轻微的下降开始元素的向下运动。

直线：
沿着单个轴（水平或垂直，但不是两者）移动的元素不遵循弧线。
这些动作更简单，可能会以更快的速度移动。 保持单轴运动的路径直线
不要对单轴运动应用非自然弧。

进入和离开屏幕的元素也沿单个轴移动。
如果元素在屏幕外移动，则不要使元素的路径弧化，因为它使入口点变得复杂。


duration:
不是为所有动画使用单个持续时间，调整每个持续时间以适应行进的距离，元素的速度和表面变化。
离开屏幕的物体可能具有较短的持续时间，因为它们需要较少的注意。

移动通常在超过300ms的范围内发生，在这种差异范围内：
大型，复杂的全屏过渡可能具有更长的持续时间， 发生超过375毫秒
进入屏幕的元素超过225ms 离开屏幕的元素超过195ms 超过400毫秒的转场可能感觉太慢。

桌面动画应该比移动对手更快更简单。这些动画应该持续150ms到200ms。
由于桌面转换可能不太明显， 因此他们应该立即响应速度比移动对手更快。
复杂的网页转换通常会导致帧丢帧（除非它们是用于GPU加速的）
。较短的持续时间会使这些不太明显，因为动画更快。

ease:
在动画的持续时间内，加速和减速的变化应该是平滑的，这样运动就不会出现机械的。
当加速和减速不对称地发生时，运动表现得更加自然和愉快。

根据所使用的平台或软件，缓和曲线的命名可能不同。这些准则将其称为标准，减速，加速和曲线。

标准曲线（也称为“ease in
out”）是最常见的缓和曲线。元素在屏幕上的位置之间快速加速并缓慢减速。 Platform
Protocol
-----------------------
Android   FastOutSlowInInterpolator
iOS       [[CAMediaTimingFunction alloc]
initWithControlPoints:0.4f:0.0f:0.2f:1.0f] CSS       cubic-bezier(0.4, 0.0, 0.2,
1); After Effects  Outgoing Velocity: 40% Incoming Velocity: 80% 可用于： 1.
标准曲线用于移动屏幕内外的元素，如一个元素受其它元素动画挤压的动画，以保持平滑的开始和停止。
与独立元素相比，该曲线的持续时间稍长（300ms）。
       2. 屏幕上出现的所有表面变换都使用标准曲线。

减速曲线
使用减速曲线（也称为“ease out”）元素以全速进入屏幕，并缓慢减速到静止点。
在减速期间，
元件可以在尺寸（至100％）或不透明度（至100％）之间进行放大。在某些情况下，当元素以0％不透明度
进入屏幕时，它们在进入时可能会从较大的尺寸略微缩小。
Platform   Protocol
------------------------
Android    LinearOutSlowInInterpolator
iOS        [[CAMediaTimingFunction alloc]
initWithControlPoints:0.0f:0.0f:0.2f:1.0f] CSS        cubic-bezier(0.0, 0.0,
0.2, 1); After Effects  Outgoing Velocity: 0% Incoming Velocity: 80% 可用于:
进场动画(225ms)

加速曲线	Acceleration curve
使用加速度曲线（也称为“ease in”）元素将屏幕全速离开。它们在离屏时不减速。
它们在动画开始时加速，
可能会以大小（0％）或不透明度（至0％）缩小。在某些情况下，当元素以0％不透明度离开屏幕时，
它们也可能在尺寸上略微向上或向下缩小。
Platform   Protocol
-------------------------
Android    FastOutLinearInInterpolator
iOS        [CAMediaTimingFunction alloc]
initWithControlPoints:0.4f:0.0f:1.0f:1.0f] CSS        cubic-bezier(0.4, 0.0, 1,
1); After Effects   Outgoing Velocity: 40% Incoming Velocity: 0% 可用于:
永久离场动画(195ms)

夏普曲线 Sharp curve
使用尖锐的曲线（也称为“ease in
out”）元素快速加速和减速。它通过退出可以随时返回到屏幕的元素使 用。
元素可以从屏幕上的起始点快速加速，然后快速地将对称曲线减速到立即离开屏幕的静止点。减速比
标准曲线更快，因为它不符合离屏点的确切路径。元素可以随时从该点返回。
Platform   Protocol
------------------------
Android    -
iOS        -
CSS        cubic-bezier(0.4, 0.0, 0.6, 1);
After Effects   Outgoing Velocity: 40%
                            Incoming Velocity: 40%
可用于：临时离场(300ms)（返场时使用减速曲线）
*/

// 2015.10.18
// WaitableTimer + SleepEx/WaitForSingleObject + while(1)结合时，发现
// FPS有时最大只在60帧左右,但有时有能够正常。
// 后测试发现，打开chrome浏览器时，fps能上去。关闭chrome时，fps只能是60
// 2016.3.10
// 使用timeBeginPeriod可恢复精度。

class Storyboard;
struct IAnimateEventCallback;
struct IAnimateTimerCallback;

class Animate {
public:
  Animate(ui::Application*);
  ~Animate();

  void AddStoryboard(Storyboard *p);
  void AddStoryboardBlock(Storyboard *p);
  void AddStoryboardFullCpu(Storyboard *p);
  void ClearStoryboardOfNotify(IAnimateEventCallback *);
  void RemoveStoryboardByNotityAndId(IAnimateEventCallback *pCallback, int nId);
  void RemoveStoryboard(Storyboard *p);
  void CancelStoryboard(Storyboard *p);

  Storyboard *CreateStoryboard(IAnimateEventCallback *pNotify, int nId = 0,
                               long wParam = 0, long lParam = 0);

  IAnimate *GetIAnimate();
  void SetTimerCallback(IAnimateTimerCallback *);

public:
  int GetFps() { return m_nFps; }
  int SetFps(int n);

  void OnTick();

protected:
  void Destroy();

  void SetTimer();
  void KillTimer();

protected:
  IAnimate m_ianimate;
  IAnimateTimerCallback *m_pITimerCallback;
  ui::Application *m_pUIApplication;

  int m_nFps;
  bool m_bTimerStart;

  // 避免在遍历stl过程中外部又删除一个对象，导致崩溃
  bool m_bHandlingTimerCallback; 

  std::list<Storyboard *> m_listStoryboard;
};
} // namespace uia
