#ifndef _UI_SDK_SRC_WINDOW_EVENTDISPATHER_H_
#define _UI_SDK_SRC_WINDOW_EVENTDISPATHER_H_

// 事件分发器
// 参照HTML，实现为 “捕获 -> 目标 -> 冒泡” 的三阶段模型

#include "include/event.h"
#include "include/macro/helper.h"
#include "src/object/object.h"
#include "window_mouse_key.h"

namespace ui {
class EventDispatcher {
public:
  static void Dispatch(Object &target, Event *event) {
    UIASSERT(event && event->type);
    event->target = target.GetIObject();

    //  捕获阶段
    event->phase = EventPhase::Capturing;
    if (target.GetParentObject()) {
      onEventCapture(*target.GetParentObject(), event);
    }
    if (event->IsPropagationStopped()) {
      return;
    }

    // 目标阶段
    event->phase = EventPhase::AtTarget;
    event->node = target.GetIObject();
    target.emit(event);

    //  冒泡阶段
    event->phase = EventPhase::Bubbling;
    Object *node = target.GetParentObject();
    while (node) {
      event->node = node->GetIObject();
      node->emit(event);
      node = node->GetParentObject();
      if (event->IsPropagationStopped()) {
        break;
      }
    }
  }

private:
  static void onEventCapture(Object &node, Event *event) {
    Object *parent = node.GetParentObject();
    if (parent) {
      onEventCapture(*parent, event);
      if (event->IsPropagationStopped()) {
        return;
      }
    }

    event->node = node.GetIObject();
    node.emit(event, EventPhase::Capturing);
  }
};

} // namespace ui
#endif