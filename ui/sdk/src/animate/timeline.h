#pragma once
#include "animatemgr.h"
#include "movealgo.h"
// #include "src/util/Stopwatch/stopwatch.h"

// 2014.2.20  TODO: 增加repeat delay功能。每一次循环时可以延时一段时间
// 2015.10.13 
//  Reverse反转的实现原理：
//  反转时，from/to不用对调，只需要将当前的时间进度修改为 1-time progress
//  即可。

namespace uia
{
#define TIMELINE_FLAG_FINISH       0x0004   // 指示一个timeline是否已经到期结束了
#define TIMELINE_FLAG_NEED_RESET   0x0008   // 表示一个timeline一次循环结束，下次开始前需要重置
#define TIMELINE_FLAG_REVERSING    0x0010   // 表示正在反向进行动画
#define TIMELINE_FLAG_AUTO_REVERSE  0x0020  // 该动画自动反转

class Timeline
{
public:
    Timeline();
    virtual ~Timeline();

public:
    ITimeline*  GetITimeline();
    void  SetITimeline(ITimeline* p);
    void  SetAnimateMgr(AnimateManager* p) { m_pAnimateMgr = p; }

    void  SetId(int nId) { m_nId = nId; }
    int   GetId() { return m_nId; }

	void  SetUserData(long lParam) { m_pParam = lParam;}
	long  GetUserData() { return m_pParam; }

	void  SetRepeateCount(int n) { m_nRepeatTimes = n; }
    void  SetTimeType(TIMELINE_TIME_TYPE eType) { m_eTimeType = eType; }
    void  SetAutoReverse(bool b);
	bool  IsAutoReverse();

    bool  IsFinish();
    void  SetFinish();
    bool  IsReversing();
    void  SetReversing(bool);

	float  GetCurrentValue() { return m_fCurrentValue; }

    void  OnAnimateStart();

protected:
	void  reset_time();

public:
	virtual bool  OnTick();  // 注：作成虚函数，是为了让idle timeline 直接返回false;
	
	// 给子类的虚函数
	// virtual void  Init(){};
	// 子类使用当前时间进度，计算出当前值
    virtual float  OnTick(float fTimePercent) = 0;

protected:
	AnimateManager*  m_pAnimateMgr;

    ITimeline*  m_pITimeline;
    bool  m_bCreateITimeline;

    float   m_duration;
	float   m_fCurrentValue;

	// TODO: 要加上一个delay参数，允许timeline延迟进行，（实现非对称动画）
	TIMELINE_TIME_TYPE  m_eTimeType;
    // StopWatch  m_stopWatch; // 计时 记录动画开始时的time tick，当时间到达m_nBeginTime+m_nDuretion时，动画结束。
    int   m_nFrameElapse; // 经历的帧数

	int   m_nId;
    int   m_nRepeatTimes; // 1表示播放一次，小于等于0表示一直播放，当--m_nRepeatTimes==0时停止. 
    int   m_nFlags;
	long  m_pParam;
};
	
class  FromToTimeline : public Timeline
{
public:
	FromToTimeline();
	~FromToTimeline();
	IFromToTimeline*  GetIFromToTimeline();

public:
	void  SetParam(float from, float to, float t);
    void  SetEaseType(EaseType eType);
    void  SetCustomBezierEase(double p1x, double p1y, double p2x, double p2y);
    void  SetCustomTimingFuction(pfnTimingFunction f);
	void  ModifyTo(float to);

	virtual float OnTick(float fTimePercent) override;

private:
	IFromToTimeline*  m_pIFromToTimeline;
	
	float  m_from;
	float  m_to;

    // 如果未指定该function，则默认创建ease_out类型
	ITimingFunction*  m_pTimingFunction;
};

class KeyFrame
{
public:
	KeyFrame();
	~KeyFrame();

	float OnTick(float from, float fThisFrameTimePercent);

public:
	float  percent;
	float  to; 
	ITimingFunction*  pTimingFunction;
};

class KeyFrameTimeline : public Timeline
{
public:
	KeyFrameTimeline();
	~KeyFrameTimeline();

    IKeyFrameTimeline*  GetIKeyFrameTimeline();

	void  SetParam(float from, float t);
	void  AddKeyFrame(float percent, float to, EaseType eType);

    virtual float  OnTick(float fTimePercent) override;

private:
	IKeyFrameTimeline*  m_pIKeyFrameTimeline;

	float  m_from;
	std::vector<KeyFrame*>  m_keyFrames;  // 这里没有去考虑排序
};


class StepTimeline : public Timeline
{
public:

};

// 什么也不做，只是为了维持动画计时器
class IdleTimeline : public Timeline
{
public:
	virtual void  Init() {};
	virtual bool  OnTick() { return false; }
	virtual float  OnTick(float fTimePercent) { return 0; }
};

}
