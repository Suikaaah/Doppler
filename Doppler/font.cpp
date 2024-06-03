#include "font.hpp"

#include <format>
#include "util.hpp"

Font::Font(Font&& other) noexcept {
  *this = std::move(other);
}

Font& Font::operator=(Font&& other) noexcept {
  if (this != &other) {
    std::swap(m_font, other.m_font);
    other.cleanup();
  }

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
