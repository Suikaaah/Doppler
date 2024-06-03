#include "timer.hpp"

#include <thread>

void Timer::set_delay(int delay) noexcept {
  m_delay = delay;
}

void Timer::update() {
  m_ref = Clock::now();

  const auto next = m_sleep_until + std::chrono::microseconds(m_delay);
  if (m_ref < next) {
    m_sleep_until = next;
  }
  else {
    m_sleep_until = m_ref + std::chrono::microseconds(m_delay);
  }
}

void Timer::sleep() const {
  std::this_thread::sleep_until(m_sleep_until);
}

bool Timer::is_expired() const {
  return std::chrono::microseconds{ m_delay }
    <= std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - m_ref);
}
