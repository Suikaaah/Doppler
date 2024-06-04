#include "timer.hpp"

#include <thread>

void Timer::set_delay(int delay) noexcept {
  m_delay = delay;
}

void Timer::update() {
  m_ref = Clock::now();

  const auto next = m_sleep_until + Unit{m_delay};
  if (m_ref < next) {
    m_sleep_until = next;
  }
  else {
    m_sleep_until = m_ref + Unit{m_delay};
  }
}

void Timer::sleep() const {
  std::this_thread::sleep_until(m_sleep_until);
}

Timer::Precision Timer::delta_duration() const {
  return std::chrono::duration_cast<Precision>(Clock::now() - m_ref);
}

bool Timer::is_expired() const {
  return Unit{ m_delay } <= delta_duration();
}
