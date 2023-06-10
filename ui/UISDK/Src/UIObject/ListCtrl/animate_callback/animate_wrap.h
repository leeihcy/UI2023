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
    // ���ڶ�����ʼǰ��׼��
    virtual bool  PreStart();

    // ���ڶ�������
    virtual bool  IsEqual(AnimateImpl* p);

    // ��ʼ����
    virtual bool  StartReq(UIA::IStoryboard* p) = 0;

	// ���ڶ����������֪ͨ���ص�
	virtual void  PostEnd() {};

    // ������Ӧ
    virtual void  OnEnd(UIA::E_ANIMATE_END_REASON) = 0;
    virtual UIA::E_ANIMATE_TICK_RESULT  OnTick(UIA::IStoryboard*) = 0;

    // ���������ȡapp�ӿ�
    virtual UI::IUIApplication*  GetUIApplication() = 0;

protected:
    virtual void  OnAnimateEnd(UIA::IStoryboard*, UIA::E_ANIMATE_END_REASON) override;
    virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(UIA::IStoryboard*) override;

protected:
    // �������������ֳ�����
    // 1. ������m_lUniqueId����ֱ�ӱȽ�unique id 
    // 2. û������m_lUniqueId������������ȥ����
    long  m_lUniqueId;  
    // ���ڻ�������ת��
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
