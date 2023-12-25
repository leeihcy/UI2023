#include "animate.h"
#include "src/application/uiapplication.h"
#include "storyboard.h"

namespace uia {
Animate *g_pAnimate = nullptr;
Animate::Animate(ui::Application* app) : m_ianimate(this) {
  g_pAnimate = this;

  m_pITimerCallback = nullptr;
  m_pUIApplication = app;

  m_bTimerStart = false;
  m_bHandlingTimerCallback = false;
  m_nFps = 60;
}

Animate::~Animate() {
  // 注：close timer handle不能在UnCoInitialize后调用
  Destroy();
}

IAnimate *Animate::GetIAnimate() {
  return &m_ianimate;
}

void Animate::SetTimerCallback(IAnimateTimerCallback *p) {
  m_pITimerCallback = p;
}

int Animate::SetFps(int n) {
  int nOld = m_nFps;
  m_nFps = n;
  if (m_nFps > 1000)
    m_nFps = 1000;
  return nOld;
}
void Animate::Destroy() {
  KillTimer();
  std::list<Storyboard *>::iterator iter = m_listStoryboard.begin();
  for (; iter != m_listStoryboard.end(); ++iter) {
    Storyboard *p = *iter;
    if (!p)
      continue;

    p->NotifyDiscard();
    delete p;
  }
  m_listStoryboard.clear();
}

Storyboard *Animate::CreateStoryboard(IAnimateEventCallback *pNotify,
                                             int nId, long wParam,
                                             long lParam) {
  Storyboard *pStoryboard = new Storyboard;
  pStoryboard->SetAnimateMgr(this);
  pStoryboard->SetEventListener(pNotify);
  pStoryboard->SetId(nId);
  pStoryboard->SetWParam(wParam);
  pStoryboard->SetLParam(lParam);

  return pStoryboard;
}

void Animate::CancelStoryboard(Storyboard *p) { RemoveStoryboard(p); }
void Animate::RemoveStoryboard(Storyboard *p) {
  if (nullptr == p)
    return;

  // 如果正在对m_listObjTimeline进行遍历中，则不能在这里对list做erase操作

  std::list<Storyboard *>::iterator iter =
      std::find(m_listStoryboard.begin(), m_listStoryboard.end(), p);

  if (iter == m_listStoryboard.end()) {
    SAFE_DELETE(p);
    return;
  }

  p->NotifyCancel();

  if (m_bHandlingTimerCallback) {
    (*iter) = nullptr; // 其它地方都需要判断是否为空指针！！
    SAFE_DELETE(p);
  } else {
    m_listStoryboard.erase(iter);
    SAFE_DELETE(p);

    if (0 == m_listStoryboard.size()) {
      KillTimer();
    }
  }
}

void Animate::ClearStoryboardOfNotify(IAnimateEventCallback *pNotify) {
  if (nullptr == pNotify)
    return;

  std::list<Storyboard *>::iterator iter = m_listStoryboard.begin();
  for (; iter != m_listStoryboard.end();) {
    Storyboard *pStoryboard = *iter;
    if (!pStoryboard) {
      // 见：Animate::RemoveStoryboard
      iter = m_listStoryboard.erase(iter);
      continue;
    }

    if (pStoryboard->GetEventListener() != pNotify) {
      ++iter;
      continue;
    }

    if (m_bHandlingTimerCallback) {
      pStoryboard->SetFinish();
      pStoryboard->NotifyCancel();
      // 防止ontick在下一个遍历中继续回调，但对象可能已经销毁了。
      pStoryboard->MarkToBeDestroyed();
      ++iter;
    } else {
      iter = m_listStoryboard.erase(iter);
      pStoryboard->NotifyCancel();
      SAFE_DELETE(pStoryboard);
    }
  }

  if (0 == m_listStoryboard.size()) {
    KillTimer();
  }
}

void Animate::RemoveStoryboardByNotityAndId(
    IAnimateEventCallback *pCallback, int nId) {
  if (nullptr == pCallback)
    return;

  std::list<Storyboard *>::iterator iter = m_listStoryboard.begin();
  for (; iter != m_listStoryboard.end();) {
    Storyboard *pStoryboard = *iter;
    if (!pStoryboard) {
      // 见：Animate::RemoveStoryboard
      iter = m_listStoryboard.erase(iter);
      continue;
    }

    if (pStoryboard->GetEventListener() != pCallback) {
      ++iter;
      continue;
    }

    if (pStoryboard->GetId() != nId) {
      ++iter;
      continue;
    }

    if (m_bHandlingTimerCallback) {
      pStoryboard->SetFinish();
      pStoryboard->NotifyCancel();
      ++iter;
    } else {
      iter = m_listStoryboard.erase(iter);
      pStoryboard->NotifyCancel();
      SAFE_DELETE(pStoryboard);
    }
  }

  if (0 == m_listStoryboard.size()) {
    KillTimer();
  }
}

void Animate::AddStoryboard(Storyboard *p) {
  if (nullptr == p)
    return;

  std::list<Storyboard *>::iterator iter =
      std::find(m_listStoryboard.begin(), m_listStoryboard.end(), p);

  if (iter != m_listStoryboard.end())
    return;

  m_listStoryboard.push_back(p);

  SetTimer();

  if (!p->IsDelayWaiting())
    p->OnAnimateStart();
}

// 阻塞型动画
void Animate::AddStoryboardBlock(Storyboard *pStoryboard) {
#if 0 // defined(OS_WIN)
    if (!pStoryboard)
        return;

    SetTimer();

    pStoryboard->OnAnimateStart();

    //StopWatch watch;
    bool bFinish = false;
    while (1)
    {
        //watch.Start();
        bFinish = pStoryboard->OnTick();
        //long elapse = watch.Now();
        //assert(elapse < 5);

        if (bFinish)
        {
            pStoryboard->NotifyEnd();

            SAFE_DELETE(pStoryboard);
            if (0 == m_listStoryboard.size())
                KillTimer();

            return;
        }
        else
        {
            // sleep效率太低，换成使用Waitable Timer
            WaitForSingleObject(m_hTimer,INFINITE); 
            //UI::WaitForVerticalBlank();
        }
    }
#else
  UIASSERT(false);
#endif
}

// 尽量占满CPU来运行一个动画，如果涉及到位移的动画需很高的FPS才能不卡，涉及到透明的动画不需要。
void Animate::AddStoryboardFullCpu(Storyboard *pStoryboard) {
#if 0 // defined(OS_WIN)
	if (!pStoryboard)
		return;

	m_listStoryboard.push_back(pStoryboard);
	pStoryboard->OnAnimateStart();

	// 避免中途被外部清理掉，做一个引用 
	Storyboard* pShadow = pStoryboard;
	pStoryboard->SetRef(&pShadow);

	while (pShadow && !pShadow->IsFinish())
	{
		this->OnTick();
		m_pUIApplication->MsgHandleOnce();
	}

	if (pShadow)
	{
		pShadow->NotifyEnd();
		pShadow->SetRef(nullptr);

		SAFE_DELETE(pShadow);
		if (0 == m_listStoryboard.size())
			KillTimer();
	}
#else
  UIASSERT(false);
#endif
}

void Animate::OnTick() {
  //    UI::WaitForVerticalBlank();

  // 	static long lPrev = 0;
  // 	static LARGE_INTEGER liPrev = {0};
  // 	static LARGE_INTEGER liNow = {0};
  // 	::QueryPerformanceCounter(&liNow);
  // 	 int time = (int)((liNow.QuadPart -
  // liPrev.QuadPart)*1000/m_liPerFreq.QuadPart);
  //
  // 	char  szText[64] = {0};
  // 	sprintf(szText, "%d\n", time);
  // 	liPrev = liNow;
  // 	::OutputDebugStringA(szText);

  m_bHandlingTimerCallback = true;

  std::list<Storyboard *>::iterator iter = m_listStoryboard.begin();
  for (; iter != m_listStoryboard.end();) {
    Storyboard *pStoryboard = *iter;

    // 在遍历过程中被销毁了。这里清理掉
    if (!pStoryboard) {
      iter = m_listStoryboard.erase(iter);
      continue;
    }

    // 延时动画
    if (pStoryboard->IsDelayWaiting()) {
      pStoryboard->UpdateDelayWaiting();

      if (pStoryboard->IsDelayWaiting()) {
        ++iter;
        continue;
      } else {
        pStoryboard->OnAnimateStart();
      }
    }

    if (!pStoryboard->IsFinish()) {
      pStoryboard->OnTick();
    }
    pStoryboard = *iter; // 再更新。有可能中途被remove了

    if (pStoryboard && pStoryboard->IsFinish()) {
      iter = m_listStoryboard.erase(iter);
      pStoryboard->NotifyEnd();
      delete pStoryboard;
    } else {
      ++iter;
    }
  }

  m_bHandlingTimerCallback = false;

  if (0 == m_listStoryboard.size()) {
    KillTimer();
  }
}

void Animate::SetTimer() {
  m_pUIApplication->CreateAnimateTimer(m_nFps);
}
void Animate::KillTimer() {
  m_pUIApplication->DestroyAnimateTimer();
}
} // namespace uia