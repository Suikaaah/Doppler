#pragma once

#include <chrono>

class Timer {
private:
  using Clock = std::chrono::high_resolution_clock;

  Clock::time_point m_ref, m_sleep_until;
  int               m_delay{1000};

public:
  constexpr explicit Timer(int delay) noexcept : m_delay{delay} {}

  void set_delay(int delay) noexcept;
  void update();
  void sleep() const;
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
    const auto micro_sec = std::chrono::duration_cast<std::chrono::microseconds>(
      Clock::now() - m_ref).count();
    return static_cast<T>(static_cast<double>(micro_sec) * 1.0e-6);
  }
};
