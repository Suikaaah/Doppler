#pragma once

#include <string_view>
#include <SDL_ttf.h>

class Font {
private:
  TTF_Font* m_font{};

public:
  constexpr Font() noexcept    = default;
  Font(const Font&)            = delete;
  Font(Font&& other) noexcept;
  Font& operator=(const Font&) = delete;
  Font& operator=(Font&& other) noexcept;
  ~Font() noexcept;

  void cleanup() noexcept;
  [[nodiscard]] bool load(std::string_view filename, int ptsize);
  constexpr auto get_font() const noexcept { return m_font; }
};
