#include "playback_state.hpp"

#include <algorithm>
#include <format>

#define NOMINMAX
#define MINIMP3_IMPLEMENTATION
#include <minimp3_ex.h>

#include "util.hpp"

void PlaybackState::cleanup() noexcept {
  set_position(0);
  set_reached_eof(false);

  if (m_params.opened) {
    mp3dec_ex_close(m_dec.get());
    m_params.opened = false;
  }
}

PlaybackState::PlaybackState() : m_dec{std::make_unique<mp3dec_ex_t>()} {
}

PlaybackState::PlaybackState(PlaybackState&& other) noexcept {
  *this = std::move(other);
}

PlaybackState& PlaybackState::operator=(PlaybackState&& other) noexcept {
  if (this != &other) {
    std::swap(m_dec   , other.m_dec);
    std::swap(m_params, other.m_params);
    other.cleanup();
  }

  return *this;
}

PlaybackState::~PlaybackState() noexcept {
  cleanup();
}

void PlaybackState::open(std::string_view filename) {
  if (m_params.opened) {
    cleanup();
  }

  m_params.opened = !mp3dec_ex_open(m_dec.get(), filename.data(), MP3D_SEEK_TO_SAMPLE);
  if (!m_params.opened) {
    messagebox(std::format("Failed to open {} as an mp3 file", filename));
    set_reached_eof(true);
  }
}

void PlaybackState::advance(std::size_t step) noexcept {
  set_position(m_params.position + step);
}

std::size_t PlaybackState::read(std::array<float, chunk_size>& buffer) {
  if (mp3dec_ex_seek(m_dec.get(), m_params.position)) {
    messagebox("Seek failed: your file is probably corrupted");
    set_reached_eof(true);
  }
  return mp3dec_ex_read(m_dec.get(), buffer.data(), chunk_size);
}

void PlaybackState::update_bistates() noexcept {
  auto& p = m_params;
  p.ff_b.update(p.ff);
  p.pan_b.update(p.pan);
  p.volume_b.update(p.volume);
}

void PlaybackState::set_position(std::size_t position) noexcept {
  m_params.position = std::min(position, get_samples());
}

void PlaybackState::set_ff(float ff) noexcept {
  static constexpr float min_ff = 0.01f;
  m_params.ff = std::clamp(ff, min_ff, static_cast<float>(max_ff));
}

void PlaybackState::set_reached_eof(bool reached) noexcept {
  m_params.reached_eof = reached;
}

void PlaybackState::set_pan(float pan) noexcept {
  m_params.pan = pan;
}

void PlaybackState::set_volume(float volume) noexcept {
  m_params.volume = volume;
}
