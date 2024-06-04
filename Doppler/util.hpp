#pragma once

#include <string_view>
#include <random>

template <class T, class... U> struct Head    { using type = T; };
template <class T>             struct Head<T> { using type = T; };

template <class T, class U, class... V>
static constexpr bool all_same       = std::same_as<T, U> && all_same<U, V...>;
template <class T, class U>
static constexpr bool all_same<T, U> = std::same_as<T, U>;

template <class>
static constexpr bool false_v = false;
template <class T = void>
struct Halt { static_assert(false_v<T>); };

void messagebox(std::string_view text);

template <class T>
T random_scalar(T min, T max) {
  static std::mt19937_64 mt{ std::random_device{}() };

  if constexpr (std::is_floating_point_v<T>) {
    std::uniform_real_distribution<T> dist{ min, max };
    return dist(mt);
  }
  else if constexpr (std::is_integral_v<T>) {
    std::uniform_int_distribution<T> dist{ min, max };
    return dist(mt);
  }
  else {
    Halt{};
  }
}
