#include "engine.hpp"

#include <filesystem>
#include <ranges>
#include <random>
#include <format>

void Engine::load_files(std::string_view dir) {
  if (!std::filesystem::exists(dir)) {
    messagebox(std::format("{} is not a valid path", dir));
    return;
  }

  std::vector<std::string> temp;
  for (const auto& entry : std::filesystem::recursive_directory_iterator{dir}) {
    const auto& path = entry.path();
    if (path.extension() == ".mp3") {
      temp.emplace_back(path.string());
    }
  }

  std::ranges::shuffle(temp, std::mt19937_64{ std::random_device{}() });
  for (auto& file : temp) {
    m_files.push(std::move(file));
  }
}

void Engine::play_and_pop() {
  if (!m_files.empty()) {
    const auto& info = get_ps().get_info();
    const auto  top  = m_files.top();

    m_audio.play(top);
    SDL_SetWindowTitle(&m_sdl.get_window(),
      std::format("{} | {} Hz | {} Ch | {} kbps", top, info.hz, info.channels, info.bitrate_kbps).c_str());

    m_files.pop();
  }
}

void Engine::draw_components() {
  static constexpr auto white = rgb(0xFF, 0xFF, 0xFF);

  const auto& ps     = get_ps();
  const auto  ffr    = ps.get_ff() / static_cast<float>(PlaybackState::max_ff);
  const auto  length = (m_mouse_pos.convert<float>() - m_source_pos).length();
  const auto  cl     = static_cast<Uint8>(std::clamp(static_cast<int>(length * 0.3f), 0, 0xFF));
  const auto  rg     = rgb(cl, 0xFF - cl, 0);
  m_sdl.draw_rect(Vector(0, 0)       , Vector<int, 2>(ffr                      * SDL::resolution[0], font_pts), rgb(127, 31, 31));
  m_sdl.draw_rect(Vector(0, font_pts), Vector<int, 2>(ps.get_progress<float>() * SDL::resolution[0], font_pts), rgb(31 , 96, 96));
  m_sdl.draw_text(m_font, std::format("{:.3f} x", ps.get_ff())                        , Vector(0, 0)           , white);
  m_sdl.draw_text(m_font, std::format("{:.3f} %", ps.get_progress<float>() * 100.0)   , Vector(0, font_pts)    , white);
  m_sdl.draw_text(m_font, std::format("{} / {}" , ps.get_position(), ps.get_samples()), Vector(0, font_pts * 2), white);
  m_sdl.draw_line(m_source_pos.convert<int>()                 , m_mouse_pos, rg);
  m_sdl.draw_rect(m_observer_pos.convert<int>() - rect_dim / 2, rect_dim   , white);
  m_sdl.draw_rect(m_source_pos.convert<int>()   - rect_dim / 2, rect_dim   , rg);
}

void Engine::process_inputs() {
  auto& ps = get_ps();

  Vector<int, 2> mouse_pos_temp;
  const auto  mouse_state = SDL_GetMouseState(&mouse_pos_temp[0], &mouse_pos_temp[1]);
  const auto  keybd_state = SDL_GetKeyboardState(nullptr);
  const bool  mouse_left  = mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT);
  const bool  mouse_right = mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT);
  const bool  keybd_space = keybd_state[SDL_SCANCODE_SPACE];
  const auto& info        = ps.get_info();

  m_space.update(keybd_space);

  if (mouse_left) {
    m_mouse_pos = mouse_pos_temp;
  }
  if (mouse_right) {
    ps.advance(info.channels * (info.hz / 2));
  }
  if (ps.get_reached_eof() || triggered(m_space)) {
    if (m_files.empty()) {
      load_files(root_path);
    }
    // no empty check needed
    play_and_pop();
  }
}

void Engine::update_physics() {
  auto& ps = get_ps();

  m_source_acc = Vector(0.0f, gravity) + pull_force
               * (m_mouse_pos.convert<float>() - m_source_pos)
               - source_damper * m_source_vel;
  m_source_vel += m_source_acc;
  m_source_pos += m_source_vel;
  const auto relative_id  = (m_observer_pos - m_source_pos).normalize();
  const auto relative_vel = std::min(Vector<float, 2>::dot(relative_id, m_source_vel),
                                     sound_speed - std::numeric_limits<float>::epsilon());
  const auto ff           = sound_speed / (sound_speed - relative_vel);
  const auto volume       = std::expf(-sound_damper * (m_observer_pos - m_source_pos).length());
  ps.set_ff(ff);
  ps.set_pan(Vector<float, 2>::dot(-1.0f * relative_id, Vector(1.0f, 0.0f)));
  ps.set_volume(std::min(volume, 1.0f) * max_volume);
}

void Engine::update_frame() {
  process_inputs();

  auto& renderer = m_sdl.get_renderer();
  SDL_SetRenderDrawColor(&renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(&renderer);
  draw_components();
  SDL_RenderPresent(&renderer);
}

void Engine::run() {
  if (!m_font.load("arial.ttf", font_pts)) {
    return;
  }

  m_sdl.run([this]{
    update_physics();
    m_timer_draw.if_expired([this]{ update_frame(); });
  });
}
