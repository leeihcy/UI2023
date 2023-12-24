#ifndef _UI_IANIMATE_H_
#define _UI_IANIMATE_H_
#include "sdk/include/interface.h"

namespace uia {
enum TIMELINE_TIME_TYPE {
  TT_BY_MS,
  TT_BY_SECOND,
  TT_BY_FRAME,
};

enum EaseType {
  ease_unknown = -1,

  // cubic
  ease = 0,    // default, 逐渐变慢
  linear,      // 匀速
  ease_in,     // 加速
  ease_out,    // 减速
  ease_in_out, // 加速然后减速

  // sine
  ease_in_sine,
  ease_out_sine,
  ease_in_out_sine,

  // quad
  ease_in_quad,
  ease_out_quad,
  ease_in_out_quad,

  // quart
  ease_in_quart,
  ease_out_quart,
  ease_in_out_quart,

  // quint
  ease_in_quint,
  ease_out_quint,
  ease_in_out_quint,

  // expo
  ease_in_expo,
  ease_out_expo,
  ease_in_out_expo,

  // circ
  ease_in_circ,
  ease_out_circ,
  ease_in_out_circ,

  // back
  ease_in_back,     // 先回退，再加速
  ease_out_back,    // 先加速超出，再回来
  ease_in_out_back, // 下超出 + 上超出

  ease_bezier_support =
      ease_in_out_back, // 目前bezier只能支持到这里，后面的采用easing函数模拟

  // elastic
  ease_in_elastic,
  ease_out_elastic,
  ease_in_out_elastic,

  // bounce
  ease_in_bounce,
  ease_out_bounce,
  ease_in_out_bounce,

  // bezier_custom
  ease_bezier_custom,

  // function custom
  ease_function_custom,

  ease_end,
};

// enum SLIDE_ANIMATE_FLAG
// {
// 	SLIDE_LEFT2RIGHT,
// 	SLIDE_TOP2BOTTOM,
// 	SLIDE_RIGHT2LEFT,
// 	SLIDE_BOTTOM2TOP,
// };
// enum E_WINDOW_ANIMATE_TYPE
// {
// 	E_WINDOW_ANIMATE_3D_ROTATE,
// 	E_WINDOW_ANIMATE_UPDOWNALPHASHOW,
// 	E_WINDOW_ANIMATE_ALPHASHOW,
// };
// enum E_CONTROL_ANIMATE_TYPE
// {
// 	E_CONTROL_ANIMATE_SLIDE,
// };

//////////////////////////////////////////////////////////////////////////
//
//     动画接口
//
//////////////////////////////////////////////////////////////////////////

class Timeline;
struct UIAPI ITimeline {
public:
  ITimeline(Timeline *p);

  void SetRepeateCount(int n);
  void SetTimeType(TIMELINE_TIME_TYPE eType);
  void SetAutoReverse(bool b);
  bool IsFinish();
  void SetFinish();
  float GetCurrentValue();
  int GetCurrentIntValue();
  void SetId(int nID);
  int GetId();
  void SetUserData(long lParam);
  long GetUserData();
  void SetCustomBezierArgs(double, double, double, double);

private:
  Timeline *m_pTimelineImpl;
};

struct ITimingFunction {
  virtual ~ITimingFunction() {}

  // in: 当前时间进度
  // out: 当前位移进度
  virtual float OnTick(float fTimePercent) = 0;
};

typedef float(/*__stdcall*/ *pfnTimingFunction)(float fTimePercent);

class FromToTimeline;
struct UIAPI IFromToTimeline : public ITimeline {
  IFromToTimeline(FromToTimeline *p);

  IFromToTimeline *SetParam(float from, float to, float t);
  void ModifyTo(float to);

  IFromToTimeline *SetEaseType(EaseType eType);
  IFromToTimeline *SetCustomBezierEase(double p1x, double p1y, double p2x,
                                       double p2y);
  IFromToTimeline *SetCustomTimingFuction(pfnTimingFunction f);

private:
  FromToTimeline *m_pFromToTimelineImpl;
};

class KeyFrameTimeline;
struct UIAPI IKeyFrameTimeline : public ITimeline {
  IKeyFrameTimeline(KeyFrameTimeline *p);

  IKeyFrameTimeline *SetParam(float from, float t);
  IKeyFrameTimeline *AddKeyFrame(float percent, float to, EaseType eType);

private:
  KeyFrameTimeline *m_pKeyFrameTimelineImpl;
};

struct IAnimateEventCallback;
class Storyboard;
struct UIAPI IStoryboard {
public:
  IStoryboard(Storyboard *p);
  Storyboard *GetImpl();

  void SetEventListener(IAnimateEventCallback *);
  void SetId(int nID);
  int GetId();
  void SetWParam(ui::llong wParam);
  ui::llong GetWParam();
  void SetLParam(ui::llong lParam);
  ui::llong GetLParam();

  ITimeline *CreateIdleTimeline(int nTimelineId);
  IFromToTimeline *CreateTimeline(int nId);
  IKeyFrameTimeline *CreateKeyFrameTimeline(int nId);

  ITimeline *GetTimeline(unsigned int nIndex);
  ITimeline *FindTimeline(int nTimelineId);

  bool IsFinish();
  void Cancel();

  void Begin();
  void BeginBlock();
  void BeginFullCpu();
  void BeginDelay(long lElapse);

private:
  Storyboard *m_pStoryboardImpl;
};

struct IAnimateEventCallback;
class Animate;

struct IAnimate;
using IAnimatePtr =
    std::unique_ptr<IAnimate, void (*)(IAnimate*)>;

struct UIAPI IAnimate {
  IAnimate(Animate *p);
  Animate *GetImpl();

  void OnTick();
  int SetFps(int n);

  void ClearStoryboardByNotify(IAnimateEventCallback *);
  void RemoveStoryboard(IStoryboard *p);
  void RemoveStoryboardByNotityAndId(IAnimateEventCallback *, int nId);

  IStoryboard *CreateStoryboard(IAnimateEventCallback *pNotify, int nId = 0,
                                long wParam = 0, long lParam = 0);

private:
  Animate *m_pImpl;
};

enum E_ANIMATE_END_REASON {
  ANIMATE_END_NORMAL,  // 正常结束
  ANIMATE_END_CANCEL,  // 被新动画取消
  ANIMATE_END_DISCARD, // 进程退出，强制结束
};
enum E_ANIMATE_TICK_RESULT {
  ANIMATE_TICK_RESULT_CONTINUE, // 动画继续
  ANIMATE_TICK_RESULT_CANCEL,   // 动画取消，立即结束
};
struct IAnimateEventCallback {
  virtual void OnAnimateStart(IStoryboard *){};
  virtual void OnAnimateEnd(IStoryboard *, E_ANIMATE_END_REASON e){};
  virtual E_ANIMATE_TICK_RESULT OnAnimateTick(IStoryboard *) {
    return ANIMATE_TICK_RESULT_CONTINUE;
  };
  virtual void OnAnimateRepeat(IStoryboard *){};
  virtual void OnAnimateReverse(IStoryboard *){};
};

} // namespace uia

#endif // _UI_IANIMATE_H_