#include "include/interface/irect.h"
#include "src/element/rect/rect.h"
#include "src/element/rect/rect_meta.h"

namespace ui {
namespace svg {

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(Rect, Object)

// IRect::~IRect() {
//   if (m_pImpl) {
//     delete m_pImpl;
//     m_pImpl = nullptr;
//   }
// }
// IRect::IRect(ui::E_BOOL_CREATE_IMPL b) : I##SUPER(ui::CREATE_IMPL_TRUE) {
//   if (b) {
//     m_pImpl = (Rect *)operator new(sizeof(Rect));
//     new (m_pImpl) Rect(this);
//   } else {
//     m_pImpl = nullptr;
//   }
// }
// Rect *IRect::GetImpl() { return static_cast<Rect *>(m_pImpl); }
// IRect *IRect::CreateInstance(ui::IResource *p) {
//   return ui::ObjectCreator<IRect>::CreateInstance(p);
// }
// bool IRect::nvProcessMessage(ui::UIMSG *pMsg, int nMsgMapID, bool bDoHook) {
//   return __pImpl->nvProcessMessage(pMsg, nMsgMapID, bDoHook);
// }


} // namespace svg
} // namespace ui