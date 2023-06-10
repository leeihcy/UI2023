#pragma once

namespace UIA
{
// http://msdn.microsoft.com/zh-cn/magazine/cc163397.aspx  ��չ WPF ������
// http://www.cnblogs.com/Jax/archive/2008/03/26/1123778.html ��Programming WPF������ ��8�� ����

// http://msdn.microsoft.com/en-us/library/system.windows.media.animation.doubleanimationbase_members(v=vs.85).aspx  DoubleAnimationBase Members

// http://gamebabyrocksun.blog.163.com/blog/static/571534632010219544464/  ��Ϸ����ѭ������

/*  motion blur<<�˶�ģ��>>

�� ��Ϸ��Ϊʲô��Ҫģ����������һ�㲻�Ǹ�����

	Ϊʲôÿ��24֡���ٶȶ��ڵ�Ӱ��˵�Ѿ����Ի�ú��������Ӿ�Ч������
	���ڵ�����Ϸ��˵ȴ���ԵÿĿ������أ�ԭ��ܼ򵥣�������ڹ�����ʱ
	�򲢷�һ֡һ֡��ֹ�����㣬�������µ�ÿһ֡�Ѿ�������1/24�����ڵ�
	�����Ӿ���Ϣ�����������λ�ơ�����ڿ�¼���ʱ������ͣ��������
	�������Ĳ�����һ�������ľ�ֹ���棬����һ��ģ����ͼ��ԭ�������
	�ˡ�������������һ�㣬��Ϸ���ÿһ֡����һ����ֹ���棬���������
	���Ĺ�����ץһ��ͼƬ�������õ��Ŀ϶���һ�������ľ�̬ͼ���˶�ģ��
	������Ŀ�ľ�������ǿ�����ƶ���������ʵ�У���һ��������������֮֡
	���������λ����Ϣ�����ǽ���ǰ֡ͬǰһ֡�����һ������õ�һ��
	Ч����
*/

/*
Google Material Design �� Motion

https://material.io/guidelines/motion/movement.html#movement-movement-within-screen-bounds
https://material.google.com/motion/material-motion.html


�˶��е������ܵ���������ʵ�����������������������

���ϻ��ߣ�
����ʵ�����з���������ҪŬ���� ����Ļ�������ƶ���Ԫ��Ӧ�����Ƶ����ͨ�����������ƶ��ļ����ڼ��Ŭ����
����б�Ϸ��ƶ�ʱ����ǳ������ʼ���Զ��͵�����������
��Ҫʹ�ö��͵�������ʼԪ�ص������ƶ���

���»��ߣ�
��ʵ������½�Ԫ�ر��������١� ����Ļ�������ƶ���Ԫ��Ӧ��ͨ������������ƶ��������ٵ�Ŭ����
���Խ��������ƶ�ʱ���½�Ӧ��ʼ���ͣ�����ǳǳ�ؽ�����

��Ҫ����΢���½���ʼԪ�ص������˶���

ֱ�ߣ�
���ŵ����ᣨˮƽ��ֱ�����������ߣ��ƶ���Ԫ�ز���ѭ���ߡ� ��Щ�������򵥣����ܻ��Ը�����ٶ��ƶ���
���ֵ����˶���·��ֱ��
��Ҫ�Ե����˶�Ӧ�÷���Ȼ����

������뿪��Ļ��Ԫ��Ҳ�ص������ƶ���
���Ԫ������Ļ���ƶ�����ҪʹԪ�ص�·����������Ϊ��ʹ��ڵ��ø��ӡ�


duration:
����Ϊ���ж���ʹ�õ�������ʱ�䣬����ÿ������ʱ������Ӧ�н��ľ��룬Ԫ�ص��ٶȺͱ���仯��
�뿪��Ļ��������ܾ��н϶̵ĳ���ʱ�䣬��Ϊ������Ҫ���ٵ�ע�⡣

�ƶ�ͨ���ڳ���300ms�ķ�Χ�ڷ����������ֲ��췶Χ�ڣ� ���ͣ����ӵ�ȫ�����ɿ��ܾ��и����ĳ���ʱ�䣬
��������375���� ������Ļ��Ԫ�س���225ms �뿪��Ļ��Ԫ�س���195ms ����400�����ת�����ܸо�̫����

���涯��Ӧ�ñ��ƶ����ָ�����򵥡���Щ����Ӧ�ó���150ms��200ms�� ��������ת�����ܲ�̫���ԣ�
�������Ӧ��������Ӧ�ٶȱ��ƶ����ָ��졣 ���ӵ���ҳת��ͨ���ᵼ��֡��֡����������������GPU���ٵģ�
���϶̵ĳ���ʱ���ʹ��Щ��̫���ԣ���Ϊ�������졣

ease:
�ڶ����ĳ���ʱ���ڣ����ٺͼ��ٵı仯Ӧ����ƽ���ģ������˶��Ͳ�����ֻ�е�ġ�
�����ٺͼ��ٲ��ԳƵط���ʱ���˶����ֵø�����Ȼ����졣

������ʹ�õ�ƽ̨��������������ߵ��������ܲ�ͬ����Щ׼�����Ϊ��׼�����٣����ٺ����ߡ�

��׼���ߣ�Ҳ��Ϊ��ease in out����������Ļ������ߡ�Ԫ������Ļ�ϵ�λ��֮����ټ��ٲ��������١�
Platform   Protocol
-----------------------
Android   FastOutSlowInInterpolator
iOS       [[CAMediaTimingFunction alloc] initWithControlPoints:0.4f:0.0f:0.2f:1.0f]
CSS       cubic-bezier(0.4, 0.0, 0.2, 1);
After Effects  Outgoing Velocity: 40%
               Incoming Velocity: 80%
�����ڣ� 1. ��׼���������ƶ���Ļ�����Ԫ�أ���һ��Ԫ��������Ԫ�ض�����ѹ�Ķ������Ա���ƽ���Ŀ�ʼ��ֹͣ��
�����Ԫ����ȣ������ߵĳ���ʱ���Գ���300ms����
       2. ��Ļ�ϳ��ֵ����б���任��ʹ�ñ�׼���ߡ�

��������
ʹ�ü������ߣ�Ҳ��Ϊ��ease out����Ԫ����ȫ�ٽ�����Ļ�����������ٵ���ֹ�㡣 �ڼ����ڼ䣬
Ԫ�������ڳߴ磨��100������͸���ȣ���100����֮����зŴ���ĳЩ����£���Ԫ����0����͸����
������Ļʱ�������ڽ���ʱ���ܻ�ӽϴ�ĳߴ���΢��С��
Platform   Protocol
------------------------
Android    LinearOutSlowInInterpolator
iOS        [[CAMediaTimingFunction alloc] initWithControlPoints:0.0f:0.0f:0.2f:1.0f]
CSS        cubic-bezier(0.0, 0.0, 0.2, 1);
After Effects  Outgoing Velocity: 0%
               Incoming Velocity: 80%
������: ��������(225ms)

��������	Acceleration curve
ʹ�ü��ٶ����ߣ�Ҳ��Ϊ��ease in����Ԫ�ؽ���Ļȫ���뿪������������ʱ�����١� �����ڶ�����ʼʱ���٣�
���ܻ��Դ�С��0������͸���ȣ���0������С����ĳЩ����£���Ԫ����0����͸�����뿪��Ļʱ��
����Ҳ�����ڳߴ�����΢���ϻ�������С��
Platform   Protocol
-------------------------
Android    FastOutLinearInInterpolator
iOS        [CAMediaTimingFunction alloc] initWithControlPoints:0.4f:0.0f:1.0f:1.0f]
CSS        cubic-bezier(0.4, 0.0, 1, 1);
After Effects   Outgoing Velocity: 40%
                Incoming Velocity: 0%
������: �����볡����(195ms)

�������� Sharp curve
ʹ�ü�������ߣ�Ҳ��Ϊ��ease in out����Ԫ�ؿ��ټ��ٺͼ��١���ͨ���˳�������ʱ���ص���Ļ��Ԫ��ʹ
�á� Ԫ�ؿ��Դ���Ļ�ϵ���ʼ����ټ��٣�Ȼ����ٵؽ��Գ����߼��ٵ������뿪��Ļ�ľ�ֹ�㡣���ٱ�
��׼���߸��죬��Ϊ���������������ȷ��·����Ԫ�ؿ�����ʱ�Ӹõ㷵�ء�
Platform   Protocol
------------------------
Android    -
iOS        -
CSS        cubic-bezier(0.4, 0.0, 0.6, 1);
After Effects   Outgoing Velocity: 40%
			    Incoming Velocity: 40%
�����ڣ���ʱ�볡(300ms)������ʱʹ�ü������ߣ�
*/

// 2015.10.18
// WaitableTimer + SleepEx/WaitForSingleObject + while(1)���ʱ������
// FPS��ʱ���ֻ��60֡����,����ʱ���ܹ�������
// ����Է��֣���chrome�����ʱ��fps����ȥ���ر�chromeʱ��fpsֻ����60
// 2016.3.10
// ʹ��timeBeginPeriod�ɻָ����ȡ�

class Storyboard;
interface IAnimateManager;
interface IAnimateEventCallback;
interface IAnimateTimerCallback;

class AnimateManager
{
public:
	AnimateManager();
	~AnimateManager();

	void  AddStoryboard(Storyboard* p);
	void  AddStoryboardBlock(Storyboard* p);
    void  AddStoryboardFullCpu(Storyboard* p);
	void  ClearStoryboardOfNotify(IAnimateEventCallback*);
    void  RemoveStoryboardByNotityAndId(IAnimateEventCallback* pCallback, int nId);
	void  RemoveStoryboard(Storyboard* p);
    void  CancelStoryboard(Storyboard* p);
	
    Storyboard*  CreateStoryboard(
            IAnimateEventCallback* pNotify, 
            int nId = 0, WPARAM wParam = 0, LPARAM lParam = 0);

    IAnimateManager*  GetIAnimateManager();
    void  SetTimerCallback(IAnimateTimerCallback*);
	void  SetUIApplication(UIApplication*);

public:
	int     GetFps() { return m_nFps; }
	int     SetFps(int n);

	void    OnTick();

protected:
    void  Destroy();

	void  SetTimer();
	void  KillTimer();

protected:
    IAnimateManager*  m_pIAnimateManager;
    IAnimateTimerCallback*  m_pITimerCallback;
	UIApplication*  m_pUIApplication;

	LARGE_INTEGER     m_liPerFreq;     // ����֡������

	HANDLE m_hTimer;
	int    m_nFps;
    bool   m_bTimerStart;
	bool   m_bHandlingTimerCallback;   // �����ڱ���stl�������ⲿ��ɾ��һ�����󣬵��±���

	list<Storyboard*>  m_listStoryboard;
    HMODULE  m_hModuleWinmm;
};
}

