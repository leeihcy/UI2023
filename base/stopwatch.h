#ifndef _BASE_STOPWATCH_H_
#define _BASE_STOPWATCH_H_

#include <chrono>

class StopWatch {
public:
  StopWatch() {
    m_last = m_start = std::chrono::system_clock::now();
  }
  int ElapseMsSinceLast() {
    auto now = std::chrono::system_clock::now();
    int result = std::chrono::duration_cast<std::chrono::microseconds>(now - m_last).count();
    m_last = now;
    return result;
  }
  int ElapseMsSinceStart() {
    auto now = std::chrono::system_clock::now();
    m_last = now;
    return std::chrono::duration_cast<std::chrono::microseconds>(now - m_start).count();
  }
private:
  std::chrono::system_clock::time_point m_start;
  std::chrono::system_clock::time_point m_last;
};

#endif