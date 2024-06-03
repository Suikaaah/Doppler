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

  if (m_opened) {
    mp3dec_ex_close(&m_dec);
    m_opened = false;
  }
}

PlaybackState::~PlaybackState() noexcept {
  cleanup();
}

void PlaybackState::open(std::string_view filename) {
  if (m_opened) {
    cleanup();
  }

  m_opened = !mp3dec_ex_open(&m_dec, filename.data(), MP3D_SEEK_TO_SAMPLE);
  if (!m_opened) {
    messagebox(std::format("Failed to open {} as an mp3 file", filename));
    set_reached_eof(true);
  }
}

void PlaybackState::advance(std::size_t step) noexcept {
  set_position(get_position() + step);
}

std::size_t PlaybackState::read(std::array<float, chunk_size>& buffer) {
  if (mp3dec_ex_seek(&m_dec, m_position)) {
    messagebox("Seek failed: your file is probably corrupted");
    set_reached_eof(true);
  }
  return mp3dec_ex_read(&m_dec, buffer.data(), chunk_size);
}

void PlaybackState::update_bistates() noexcept {
  m_ff_b.update(m_ff);
  m_pan_b.update(m_pan);
  m_volume_b.update(m_volume);
}

void PlaybackState::set_position(std::size_t position) noexcept {
  m_position = std::min(position, m_dec.samples);
}

void PlaybackState::set_ff(float ff) noexcept {
  static constexpr float min_ff = 0.01f;
  m_ff = std::clamp(ff, min_ff, static_cast<float>(max_ff));
}

void PlaybackState::set_reached_eof(bool reached) noexcept {
  m_reached_eof = reached;
}

void PlaybackState::set_pan(float pan) noexcept {
  m_pan = pan;
}

void PlaybackState::set_volume(float volume) noexcept {
  m_volume = volume;
}
