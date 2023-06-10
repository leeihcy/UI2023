#pragma once
#include <list>

namespace UI
{
    interface IUIApplication;
}

class AnimateImpl : public UIA::IAnimateEventCallback
{
public:
    AnimateImpl();
    virtual ~AnimateImpl();

    void  SetUniqueId(long);
    long  GetUniquieId();
    long  GetClassType();

    void  SetDuration(long);

    void  CancelAnimate();
    bool  Start(bool bBlock);
    bool  CheckEqual(AnimateImpl* p);

protected:
    // 用于动画开始前的准备
    virtual bool  PreStart();

    // 用于动画互斥
    virtual bool  IsEqual(AnimateImpl* p);

    // 开始动画
    virtual bool  StartReq(UIA::IStoryboard* p) = 0;

	// 用于动画结束后的通知、回调
	virtual void  PostEnd() {};

    // 动画响应
    virtual void  OnEnd(UIA::E_ANIMATE_END_REASON) = 0;
    virtual UIA::E_ANIMATE_TICK_RESULT  OnTick(UIA::IStoryboard*) = 0;

    // 向派生类获取app接口
    virtual UI::IUIApplication*  GetUIApplication() = 0;

protected:
    virtual void  OnAnimateEnd(UIA::IStoryboard*, UIA::E_ANIMATE_END_REASON) override;
    virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(UIA::IStoryboard*) override;

protected:
    // 动画互斥有两种场景：
    // 1. 设置了m_lUniqueId，则直接比较unique id 
    // 2. 没有设置m_lUniqueId，交由派生类去处理
    long  m_lUniqueId;  
    // 用于基类类型转换
    long  m_lClassType;

    long  m_lDuration;

private:
    UIA::IStoryboard*  m_pStoryboard;
};


class AnimateWrapManager
{
public:
    AnimateWrapManager();
    static  AnimateWrapManager& Get();

    long  GetDefaultDuration();
    UIA::EaseType  GetDefaultEaseType();

    void  AddAnimate(AnimateImpl*);
    void  RemoveAnimate(AnimateImpl*);

    void  MakesureUnique(AnimateImpl*);

private:
    long  m_lDefaultDuration;
    UIA::EaseType  m_eDefaultEaseType;

    list<AnimateImpl*>  m_listAnimate;
};
