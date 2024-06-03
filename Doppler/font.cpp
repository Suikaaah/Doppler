#include "font.hpp"

#include <format>
#include "util.hpp"

Font::Font(Font&& other) noexcept {
  m_font = std::exchange(other.m_font, nullptr);
}

Font& Font::operator=(Font&& other) noexcept {
  m_font = std::exchange(other.m_font, nullptr);
  return *this;
}

Font::~Font() noexcept {
  cleanup();
}

void Font::cleanup() noexcept {
  if (m_font) {
    TTF_CloseFont(m_font);
    m_font = nullptr;
  }
}

bool Font::load(std::string_view filename, int ptsize) {
  m_font = TTF_OpenFont(filename.data(), ptsize);

  if (!m_font) {
    messagebox(std::format("Failed to open {} as a font", filename));
    return false;
  }

  return true;
}
