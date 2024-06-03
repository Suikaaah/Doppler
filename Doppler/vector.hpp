#pragma once

#include <array>
#include <algorithm>
#include "util.hpp"

#define SEQ(x) \
  <std::size_t... Seq> \
  (std::index_sequence<Seq...>){ x }(std::make_index_sequence<N>{});

template <class T, std::size_t N>
requires (std::is_arithmetic_v<T>)
struct Vector {
  std::array<T, N> data{};

  constexpr Vector() noexcept = default;

  template <class... Args>
  requires (sizeof...(Args) == N)
  constexpr Vector(Args... args) noexcept : data{static_cast<T>(args)...} {}

  constexpr auto& operator[](std::size_t i) noexcept { return data[i]; }
  constexpr const auto& operator[](std::size_t i) const noexcept { return data[i]; }

  constexpr auto operator+(const Vector<T, N>& rhs) const noexcept {
    return [this, &rhs] SEQ( return (Vector<T, N>{((*this)[Seq] + rhs[Seq])...}); )
  }
  constexpr auto operator-(const Vector<T, N>& rhs) const noexcept {
    return [this, &rhs] SEQ( return (Vector<T, N>{((*this)[Seq] - rhs[Seq])...}); )
  }
  constexpr auto operator*(T rhs) const noexcept {
    return [this, rhs] SEQ( return (Vector<T, N>{((*this)[Seq] * rhs)...}); )
  }
  constexpr auto operator/(T rhs) const noexcept {
    return [this, rhs] SEQ( return (Vector<T, N>{((*this)[Seq] / rhs)...}); )
  }
  auto operator+=(const Vector<T, N>& rhs) noexcept {
    [this, &rhs] SEQ( (((*this)[Seq] += rhs[Seq]), ...); )
  }
  auto operator-=(const Vector<T, N>& rhs) noexcept {
    [this, &rhs] SEQ( (((*this)[Seq] -= rhs[Seq]), ...); )
  }
  auto operator*=(T rhs) noexcept {
    [this, rhs] SEQ( (((*this)[Seq] *= rhs), ...); )
  }
  auto operator/=(T rhs) noexcept {
    [this, rhs] SEQ( (((*this)[Seq] /= rhs), ...); )
  }
  static constexpr friend auto operator*(T lhs, const Vector<T, N>& rhs) noexcept {
    return rhs * lhs;
  }

  static constexpr auto dot(const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept {
    return [&lhs, &rhs] SEQ( return ((lhs[Seq] * rhs[Seq]) + ...); )
  }

  static constexpr auto random(const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept {
    return [&lhs, &rhs] SEQ( return (Vector<T, N>{random_scalar(lhs[Seq], rhs[Seq])...}); )
  }

  auto length() const noexcept {
    return std::sqrt(dot(*this, *this));
  }

  auto normalize() const noexcept {
    return *this / length();
  }

  constexpr auto clamp(const Vector<T, N>& vmin, const Vector<T, N>& vmax) const noexcept {
    return [this, &vmin, &vmax] SEQ(
      return (Vector<T, N>{std::clamp((*this)[Seq], vmin[Seq], vmax[Seq])...});
    )
  }

  template <class T_>
  constexpr auto convert() const noexcept {
    return [this] SEQ(
      return (Vector<T_, N>{static_cast<T_>((*this)[Seq])...});
    )
  }
};

template <class... Args>
Vector(Args...) -> Vector<typename Head<Args...>::type, sizeof...(Args)>;

constexpr auto rgb(Uint8 r, Uint8 g, Uint8 b) noexcept {
  return Vector<Uint8, 4>(r, g, b, SDL_ALPHA_OPAQUE);
}
