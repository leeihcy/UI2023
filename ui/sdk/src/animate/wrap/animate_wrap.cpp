// #include "animate_wrap.h"
// #include "include/inc.h"
// #include "include/interface/ianimate.h"
// #include "src/animate/animate.h"

// namespace ui {

// AnimateWrap::AnimateWrap(Application &app)
//     : m_uiApplication(app), m_animate(uia::IAnimate::create(this)) {
//   m_animate->GetImpl()->SetUIApplication(&m_uiApplication);
// }

// AnimateWrap::~AnimateWrap() { UIASSERT(!m_animate); }

// void AnimateWrap::OnSetTimer(int hHandle) {
// #if 0
//     if (m_pWaitforHandle)
//     {
//         m_pWaitforHandle->AddHandle(
//             hHandle, 
//             static_cast<IWaitForHandleCallback*>(this), 
//             0);
//     }
// #endif
// }

// void AnimateWrap::OnKillTimer(int hHandle) {
// #if 0
//     if (m_pWaitforHandle)
//     {
//         m_pWaitforHandle->RemoveHandle(hHandle);
//     }
// #endif
// }

// // void AnimateWrap::OnWaitForHandleObjectCallback(long, long)
// // {
// //     if (m_animate)
// //         m_animate->OnTick();
// // }

// uia::IAnimate *AnimateWrap::GetAnimate() { return m_animate.get(); }

// } // namespace ui