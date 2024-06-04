#pragma once

#include <chrono>

class Timer {
private:
  using Clock     = std::chrono::high_resolution_clock;
  using Precision = std::chrono::microseconds;
  using Unit      = std::chrono::milliseconds;

  Clock::time_point m_ref, m_sleep_until;
  int               m_delay{1000};

public:
  constexpr explicit Timer(int delay) noexcept : m_delay{delay} {}

  void set_delay(int delay) noexcept;
  void update();
  void sleep() const;
  Precision delta_duration() const;
  bool is_expired() const;

  template <class Callable>
  void if_expired(Callable&& impl) {
    if (is_expired()) {
      update();
      impl();
    }
  }

  template <class T>
  auto delta() const {
    return static_cast<T>(
      static_cast<double>(delta_duration().count()) * Precision::period::num / Precision::period::den
    );
  }
};
