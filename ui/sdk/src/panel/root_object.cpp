#include "root_object.h"
#include "include/interface/iobject.h"
#include "src/resource/res_bundle.h"
#include "src/window/window.h"
#include "src/window/window_meta.h"

namespace ui {
RootObject::RootObject(IRootObject *p) : m_irootobject(p), Panel(p) {
  m_meta = &RootObjectMeta::Get();
}

void RootObject::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    Panel::onRouteMessage(msg);
    FinalConstruct();
    return;
  }
  if (msg->message == UI_MSG_ERASEBKGND) {
    onEraseBkgnd(static_cast<EraseBkgndMessage *>(msg)->rt);
    return;
  }
  Panel::onRouteMessage(msg);
}

void RootObject::FinalConstruct() {
  // TODO:
  // m_root->m_objLayer.CreateLayer();
}

bool RootObject::LoadLayout(const char *layout_id) {
  if (layout_id && strlen(layout_id) > 0) {
    LayoutManager &layoutmanager = m_resource->GetLayoutManager();

    //	加载子控件
    const char *szName = "Window";
    // if (GetMeta())
    //   szName = GetMeta()->Name();

    UIElementProxy pUIElement =
        layoutmanager.FindWindowElement(szName, layout_id);
    if (pUIElement) {
      // 自己的属性
      this->LoadAttributeFromXml(pUIElement.get(), false);

      // 遍历子对象
      layoutmanager.ParseChildElement(pUIElement.get(), this);
    } else {
      UI_LOG_FATAL("获取窗口对应的xml结点失败：name=%s, id=%s", szName,
                   layout_id);
      return false;
    }

    // m_strId = szId; // 提前给id赋值，便于日志输出
  } else {
    InitDefaultAttrib();
  }
  return true;
}

void RootObject::onWindowCreate() {
  // 窗口作为根结点，一定会创建一个缓存
  m_objStyle.layer = 1;
  this->virtualOnLoad();

  m_objStyle.initialized = 1;
  RouteMessage(UI_MSG_INITIALIZE);
  ForwardInitializeMessageToDecendant(this);
  RouteMessage(UI_MSG_INITIALIZE2);

  // 窗口加载即将完成，在内部调用所有
  // recursion_on_load_notify(this);
}
void RootObject::syncWindowSize() {
  Rect rc_client;
  m_window->m_platform->GetClientRect(&rc_client);
#if 0 // TODO:
  SetConfigWidth(rc_client.Width());
  SetConfigHeight(rc_client.Height());
#endif
  m_rcParent.CopyFrom(rc_client);
}
void RootObject::OnWindowSize(unsigned int new_client_width, unsigned int new_client_height) {
  m_rcParent.Set(0, 0, new_client_width, new_client_height);
  notify_WM_SIZE(0, new_client_width, new_client_height, m_window->m_dpi.GetScaleFactor());
}

void RootObject::onEraseBkgnd(IRenderTarget *pRenderTarget) {
  if (nullptr == pRenderTarget)
    return;

  WindowPaintEvent event;
  event.window = m_window->GetIWindow();
  event.rt = pRenderTarget;
  m_window->emit(WINDOW_PAINT_EVENT, &event);

  if (!event.handled) {
    Panel::onEraseBkgnd(pRenderTarget);
  }
}

} // namespace ui