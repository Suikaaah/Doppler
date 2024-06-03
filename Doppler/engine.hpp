#pragma once

#include <stack>

#include "sdl.hpp"
#include "audio.hpp"

class Engine {
private:
  static constexpr int   font_pts    = 32;
  static constexpr float sound_speed = 40.0f;
  static constexpr auto  rect_dim    = Vector(16, 16);

  SDL   m_sdl;
  Audio m_audio;
  Font  m_font;

  Vector<float, 2> m_source_pos, m_source_vel, m_source_acc,
                   m_observer_pos{(SDL::resolution / 2).convert<float>()};
  Vector<int, 2>           m_mouse_pos;
  Bistate<bool>            m_space;
  std::stack<std::string>  m_files;
  Timer                    m_timer_draw{1'000'000 / 240};

  constexpr auto& get_ps() noexcept { return m_audio.get_ps(); }
  void load_files(std::string_view dir);
  void play_and_pop();
  void draw_components();
  void process_inputs();
  void update();

public:
  void run();
};