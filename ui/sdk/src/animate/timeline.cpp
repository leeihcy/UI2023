#include "timeline.h"
#include "include/inc.h"

namespace uia {

Timeline::Timeline() {
  m_pITimeline = nullptr;
  m_bCreateITimeline = false;

  m_nFrameElapse = 0;
  m_eTimeType = TT_BY_MS; // 默认以毫秒为单位
  m_duration = 300;

  m_nRepeatTimes = 1; // 默认只播放一次

  m_nFlags = 0;
  m_nId = 0;
  m_pParam = 0;
}

Timeline::~Timeline() {
  if (m_bCreateITimeline)
    SAFE_DELETE(m_pITimeline);
}

ITimeline *Timeline::GetITimeline() {
  if (m_pITimeline)
    return m_pITimeline;

  m_bCreateITimeline = true;
  m_pITimeline = new ITimeline(this);
  return m_pITimeline;
}
void Timeline::SetITimeline(ITimeline *p) {
  if (m_bCreateITimeline)
    delete m_pITimeline;

  m_pITimeline = p;
  m_bCreateITimeline = false;
}

void Timeline::OnAnimateStart() { m_stopWatch.Start(); }

// 返回是否结束
bool Timeline::OnTick() {
  if (m_nFlags & TIMELINE_FLAG_NEED_RESET) {
    m_nFlags &= ~TIMELINE_FLAG_NEED_RESET;

    m_nFrameElapse = 0;
    m_stopWatch.Start();
  } else if (IsFinish()) {
    return true;
  }

  m_nFrameElapse++;

  int nTimeElapse = 0;
  switch (m_eTimeType) {
  case TT_BY_FRAME:
    nTimeElapse = m_nFrameElapse;
    break;

  case TT_BY_MS:
    nTimeElapse = m_stopWatch.ElapseMillisecondsSinceStart();
    // printf("nTimeElapse = %d\n", nTimeElapse);
    break;

  case TT_BY_SECOND:
    nTimeElapse = m_stopWatch.ElapseSecondsSinceStart();
  }

  float fTimePercent = (float)nTimeElapse / m_duration;

  bool bTimelineFinish = false;
  if (fTimePercent >= 1) {
    bTimelineFinish = 1;
    fTimePercent = 1;
  }

  if (IsReversing())
    fTimePercent = 1 - fTimePercent;
  m_fCurrentValue = this->OnTick(fTimePercent);

  if (bTimelineFinish) {
    if (IsAutoReverse()) {
      m_nFlags |= TIMELINE_FLAG_NEED_RESET; // 下次开始前先重置动画参数

      // 将数据进行反向
      // OnReverse();

      // 反向的结束，代表一次播放结束
      if (IsReversing()) {
        if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes) {
          SetFinish();
        }
        SetReversing(false);
      }
      // 正向的结束，进入反向
      else {
        SetReversing(true);
      }
    } else {
      // 一次播放结束
      if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes) {
        SetFinish();
      } else {
        m_nFlags |= TIMELINE_FLAG_NEED_RESET; // 下次开始前先重置动画参数
      }
    }
  }

  return IsFinish();
}

bool Timeline::IsFinish() {
  return (m_nFlags & TIMELINE_FLAG_FINISH) ? true : false;
}
void Timeline::SetFinish() { m_nFlags |= TIMELINE_FLAG_FINISH; }
bool Timeline::IsReversing() {
  return (m_nFlags & TIMELINE_FLAG_REVERSING) ? true : false;
}
void Timeline::SetReversing(bool b) {
  if (b)
    m_nFlags |= TIMELINE_FLAG_REVERSING;
  else
    m_nFlags &= ~TIMELINE_FLAG_REVERSING;
}

void Timeline::reset_time() {
  m_nFrameElapse = 0;
  m_stopWatch.Start();
  m_nFlags &= ~(TIMELINE_FLAG_REVERSING | TIMELINE_FLAG_NEED_RESET |
                TIMELINE_FLAG_FINISH);
}

void Timeline::SetAutoReverse(bool b) {
  if (b)
    m_nFlags |= TIMELINE_FLAG_AUTO_REVERSE;
  else
    m_nFlags &= ~TIMELINE_FLAG_AUTO_REVERSE;
}

bool Timeline::IsAutoReverse() {
  return (m_nFlags & TIMELINE_FLAG_AUTO_REVERSE) ? true : false;
}

#if 0
virtual void  SetCustomBezierArgs(double d1, double d2, double d3, double d4)
{
    if (!m_pMoveAlgorithm)
        return;

    // TODO: 这里没有确保m_pMoveAlgorithm就是EasingMove类型。
    static_cast<EasingMove*>(m_pMoveAlgorithm)->SetCustomBezierArgs(d1, d2, d3,d4);
}
void  _SetEaseParam(float from, float to, float t, EaseType eType)
{
    if (m_nFrameElapse > 0)
        _ResetTime();

    SAFE_DELETE(m_pMoveAlgorithm);
    EasingMove* p = new EasingMove;
    p->SetParam(from, to, t, eType);

    m_pMoveAlgorithm = static_cast<IMoveAlgorithm*>(p);
}

void  _SetLinerParam1(float from, float to, float t)
{
    if (m_nFrameElapse > 0)
        _ResetTime();

    SAFE_DELETE(m_pMoveAlgorithm);
    LinearMove* p = new LinearMove;
    p->SetParam1(from, to, t);

    m_pMoveAlgorithm = static_cast<IMoveAlgorithm*>(p);
}

void  _SetAccelerateParam1(float from, float to, float t, float Vo)
{
    if (m_nFrameElapse > 0)
        _ResetTime();

    SAFE_DELETE(m_pMoveAlgorithm);
    AccelerateMove* p = new AccelerateMove;
    p->SetParam1(from, to, t, Vo);

    m_pMoveAlgorithm = static_cast<IMoveAlgorithm*>(p);
}
#endif

#pragma region // Timing Fuction Creator

ITimingFunction *CreateEaseTimingFuction(EaseType e) {
  EasingMove *p = new EasingMove(e);
  return static_cast<ITimingFunction *>(p);
}

ITimingFunction *CreateDefaultTimingFuction() {
  return CreateEaseTimingFuction(ease);
}

ITimingFunction *CreateCustomBezierEaseTimingFuction(double p1x, double p1y,
                                                     double p2x, double p2y) {
  EasingMove *p = new EasingMove(ease_bezier_custom);
  p->SetCustomBezierArgs(p1x, p1y, p2x, p2y);

  return static_cast<ITimingFunction *>(p);
}

ITimingFunction *CreateCustomTimingFuction(pfnTimingFunction f) {
  CustomTimingFuction *p = new CustomTimingFuction;
  p->SetFunction(f);

  return static_cast<ITimingFunction *>(p);
}

#pragma endregion

FromToTimeline::FromToTimeline() {
  m_from = 0;
  m_to = 0;

  m_pTimingFunction = nullptr;
  m_pIFromToTimeline = new IFromToTimeline(this);
  Timeline::SetITimeline(m_pIFromToTimeline);
}
FromToTimeline::~FromToTimeline() {
  SAFE_DELETE(m_pIFromToTimeline);
  SAFE_DELETE(m_pTimingFunction);
}

IFromToTimeline *FromToTimeline::GetIFromToTimeline() {
  return m_pIFromToTimeline;
}

float FromToTimeline::OnTick(float fTimePercent) {
  if (!m_pTimingFunction)
    m_pTimingFunction = CreateDefaultTimingFuction();

  float fProgress = m_pTimingFunction->OnTick(fTimePercent);
  return m_from + (m_to - m_from) * fProgress;
}

void FromToTimeline::SetParam(float from, float to, float t) {
  if (m_nFrameElapse > 0)
    reset_time();

  m_from = from;
  m_to = to;
  m_duration = t;
}
void FromToTimeline::ModifyTo(float to) { m_to = to; }

void FromToTimeline::SetEaseType(EaseType eType) {
  SAFE_DELETE(m_pTimingFunction);
  m_pTimingFunction = CreateEaseTimingFuction(eType);
}

void FromToTimeline::SetCustomBezierEase(double p1x, double p1y, double p2x,
                                         double p2y) {
  SAFE_DELETE(m_pTimingFunction);
  m_pTimingFunction = CreateCustomBezierEaseTimingFuction(p1x, p1y, p2x, p2y);
}
void FromToTimeline::SetCustomTimingFuction(pfnTimingFunction f) {
  SAFE_DELETE(m_pTimingFunction);
  m_pTimingFunction = CreateCustomTimingFuction(f);
}

#if 0
void  FromToTimeline::SetLinerParam1(float from, float to, float t)
{
// 	if (m_nFrameElapse > 0)
// 		reset_time();

	SetEaseParam(from, to, t);

// 	m_from = from;
// 	m_to = to;
// 	m_duration = t;
// 
// 	SAFE_DELETE(m_pTimingFunction);
// 	LinearMove* p = new LinearMove;
// 	p->SetParam1(from, to, t);
// 
// 	m_pTimingFunction = static_cast<ITimingFunction*>(p);
}
#endif

KeyFrameTimeline::KeyFrameTimeline() {
  m_pIKeyFrameTimeline = new IKeyFrameTimeline(this);
  Timeline::SetITimeline(m_pIKeyFrameTimeline);

  m_from = 0;
}

KeyFrameTimeline::~KeyFrameTimeline() {
  SAFE_DELETE(m_pIKeyFrameTimeline);

  std::vector<KeyFrame *>::iterator iter = m_keyFrames.begin();
  for (; iter != m_keyFrames.end(); ++iter) {
    delete (*iter);
  }
  m_keyFrames.clear();
}

void KeyFrameTimeline::SetParam(float from, float t) {
  m_from = from;
  m_duration = t;
}
void KeyFrameTimeline::AddKeyFrame(float percent, float to, EaseType eType) {
  KeyFrame *pKeyFrame = new KeyFrame;
  pKeyFrame->percent = percent;
  pKeyFrame->to = to;
  pKeyFrame->pTimingFunction = CreateEaseTimingFuction(eType);
  m_keyFrames.push_back(pKeyFrame);
}

IKeyFrameTimeline *KeyFrameTimeline::GetIKeyFrameTimeline() {
  return m_pIKeyFrameTimeline;
}

KeyFrame::KeyFrame() {
  percent = 0;
  to = 0;
  pTimingFunction = nullptr;
}

KeyFrame::~KeyFrame() { SAFE_DELETE(pTimingFunction); }

float KeyFrame::OnTick(float from, float fThisFrameTimePercent) {
  if (!pTimingFunction) {
    pTimingFunction = CreateDefaultTimingFuction();
  }

  float f = pTimingFunction->OnTick(fThisFrameTimePercent);

  float fCurrent = from + (to - from) * f;
  return fCurrent;
}

float KeyFrameTimeline::OnTick(float fTimePercent) {
  int nSize = (int)m_keyFrames.size();

  for (int i = 0; i < nSize; ++i) {
    if (fTimePercent > m_keyFrames[i]->percent)
      continue;

    // 关键帧，由总时间求出本帧的时候进度
    float prevPercent = 0;
    float prevFrom = m_from;
    if (i > 0) {
      prevPercent = m_keyFrames[i - 1]->percent;
      prevFrom = m_keyFrames[i - 1]->to;
    }
    float fPercentDuration = m_keyFrames[i]->percent - prevPercent;
    UIASSERT(fPercentDuration > 0);

    float fFrameTimePercent = (fTimePercent - prevPercent) / fPercentDuration;
    return m_keyFrames[i]->OnTick(prevFrom, fFrameTimePercent);
  }

  return m_from;
}

} // namespace uia