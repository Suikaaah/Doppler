#pragma once

template <class T>
struct Bistate {
  T previous{}, current{};

  void shift() noexcept {
    previous = std::move(current);
  }

  void update(T&& val) noexcept {
    shift();
    current = std::move(val);
  }

  void update(const T& val) {
    shift();
    current = val;
  }
};

constexpr bool triggered(const Bistate<bool>& b) noexcept {
  return b.current && !b.previous;
}

constexpr float interpolate(const Bistate<float>& b, float t) noexcept {
  return (1.0f - t) * b.previous + t * b.current;
}
