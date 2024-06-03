#include "util.hpp"

#define NOMINMAX
#include <Windows.h>

void messagebox(std::string_view text) {
  MessageBoxA(nullptr, text.data(), nullptr, MB_ICONINFORMATION | MB_OK);
}