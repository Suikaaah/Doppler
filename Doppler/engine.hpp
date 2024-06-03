#pragma once

#include <stack>

#include "sdl.hpp"
#include "audio.hpp"

class Engine {
private:
  static constexpr std::string_view root_path{ "C:\\Users\\Suika\\Desktop\\a" };

  static constexpr int   font_pts      = 32;
  static constexpr float sound_speed   = 50.0f;
  static constexpr auto  rect_dim      = Vector(16, 16);
  static constexpr float max_volume    = 0.4f;
  static constexpr float pull_force    = 4.0e-5f;
  static constexpr float gravity       = 5.0e-3f;
  static constexpr float source_damper = 2.0e-3f;
  static constexpr float sound_damper  = 5.0e-4f;

  SDL   m_sdl;
  Audio m_audio;
  Font  m_font;

  Vector<float, 2> m_source_pos, m_source_vel, m_source_acc,
                   m_observer_pos{SDL::resolution[0]/2, SDL::resolution[1]*3/4};
  Vector<int, 2>           m_mouse_pos;
  Bistate<bool>            m_space;
  std::stack<std::string>  m_files;
  Timer                    m_timer_draw{ 1'000'000 / SDL::fps };

  constexpr auto& get_ps() noexcept { return m_audio.get_ps(); }
  void load_files(std::string_view dir);
  void play_and_pop();
  void draw_components();
  void process_inputs();
  void update();

public:
  void run();
};