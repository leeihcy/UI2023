#ifndef _BASE_STOPWATCH_H_
#define _BASE_STOPWATCH_H_

#include <chrono>

class StopWatch {
public:
  StopWatch() { Start(); }

  void Start() { m_last = m_start = std::chrono::system_clock::now(); }

  template <class T> int ElapseSinceLast() {
    // microseconds 微秒
    // milliseconds 毫秒
    auto now = std::chrono::system_clock::now();
    auto result = std::chrono::duration_cast<T>(now - m_last).count();
    m_last = now;
    return (int)result;
  }
  // 返回经过的毫秒数
  int ElapseMillisecondsSinceLast() {
    return ElapseSinceLast<std::chrono::milliseconds>();
  }
  // 返回经过的微秒数
  int ElapseMicrosecondsSinceLast() {
    return ElapseSinceLast<std::chrono::microseconds>();
  }
  // 返回经过的微秒数
  int ElapseSecondsSinceLast() {
    return ElapseSinceLast<std::chrono::seconds>();
  }

  template <class T> int ElapseSinceStart() {
    auto now = std::chrono::system_clock::now();
    m_last = now;
    return (int)std::chrono::duration_cast<T>(now - m_start).count();
  }

  // 返回经过的毫秒数
  int ElapseMillisecondsSinceStart() {
    return ElapseSinceStart<std::chrono::milliseconds>();
  }
  // 返回经过的毫秒数
  int ElapseMicrosecondsSinceStart() {
    return ElapseSinceStart<std::chrono::microseconds>();
  }
  // 返回经过的秒数
  int ElapseSecondsSinceStart() {
    return ElapseSinceStart<std::chrono::seconds>();
  }

private:
  std::chrono::system_clock::time_point m_start;
  std::chrono::system_clock::time_point m_last;
};

#endif