#pragma once

#include <string_view>

#define NOMINMAX
#include <Windows.h>

template <class T, class... U> struct Head    { using type = T; };
template <class T>             struct Head<T> { using type = T; };

template <class>
static constexpr bool false_v = false;
template <class T = void>
struct Halt { static_assert(false_v<T>); };

inline void messagebox(std::string_view text) {
  MessageBoxA(nullptr, text.data(), nullptr, MB_ICONERROR | MB_OK);
}
