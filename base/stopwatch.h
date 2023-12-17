#ifndef _BASE_STOPWATCH_H_
#define _BASE_STOPWATCH_H_

#include <chrono>

class StopWatch {
public:
  StopWatch() {
    m_last = m_start = std::chrono::system_clock::now();
  }

  // 返回经过的毫秒数
  int ElapseMillisecondsSinceLast() {
    // microseconds 微秒
    // milliseconds 毫秒
    auto now = std::chrono::system_clock::now();
    auto result = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last).count();
    m_last = now;
    return (int)result;
  }
  // 返回经过的微秒数
  int ElapseMicrosecondsSinceLast() {
    auto now = std::chrono::system_clock::now();
    auto result = std::chrono::duration_cast<std::chrono::microseconds>(now - m_last).count();
    m_last = now;
    return (int)result;
  }
  int ElapseMsSinceStart() {
    auto now = std::chrono::system_clock::now();
    m_last = now;
    return (int)std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start).count();
  }
private:
  std::chrono::system_clock::time_point m_start;
  std::chrono::system_clock::time_point m_last;
};

#endif