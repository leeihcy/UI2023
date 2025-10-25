#ifndef _UI_SDK_INCLUDE_EVENT_H_
#define _UI_SDK_INCLUDE_EVENT_H_

namespace ui {
struct IObject;

enum class EventPhase { 
  Capturing, 
  AtTarget, 
  Bubbling 
};

// 需要外部处理的事件
class EventDispatcher;
class Event {
public:
  const char* Type() const { return type; }
  IObject* Target() const { return target; }
  IObject* Node() const { return node; }
  void StopPropagation() { propagationStopped = true; }
  bool IsPropagationStopped() { return propagationStopped; }

protected:
  const char* type;  // event name
  IObject* target;   // target control
  IObject* node;     // current control
  EventPhase phase;
  bool propagationStopped = false;

  friend class EventDispatcher;
};

// 对象继承链内部路由的消息
struct Msg {
  int message;
};

}

#endif