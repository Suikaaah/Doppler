#pragma once

#include <functional>
#include <SDL.h>
#include <SDL_ttf.h>
#include "vector.hpp"
#include "timer.hpp"
#include "font.hpp"

class SDL {
private:
  static constexpr std::string_view default_title{
    "Space: Load files | Mouse Left: Pull sound source | Mouse Right: Fast forward"
  };

  bool          m_inited{};
  bool          m_ttf_inited{};
  SDL_Window*   m_window{};
  SDL_Renderer* m_renderer{};
  Timer         m_timer_tick{1}, m_timer_event{3};

  void cleanup() noexcept;

public:
  static constexpr auto resolution = Vector(1600, 900);

  SDL();
  SDL(const SDL&)            = delete;
  SDL(SDL&& other) noexcept;
  SDL& operator=(const SDL&) = delete;
  SDL& operator=(SDL&& other) noexcept;
  ~SDL() noexcept;

  void run(const std::function<void(SDL&)>& impl);
  constexpr SDL_Window& get_window() noexcept { return *m_window; }
  constexpr SDL_Renderer& get_renderer() noexcept { return *m_renderer; }

  void draw_text(const Font& font, std::string_view text, const Vector<int, 2>& pos, const Vector<Uint8, 4>& color);
  void draw_rect(const Vector<int, 2>& pos, const Vector<int, 2>& dim, const Vector<Uint8, 4>& color);
  void draw_line(const Vector<int, 2>& begin, const Vector<int, 2>& end, const Vector<Uint8, 4>& color);
};