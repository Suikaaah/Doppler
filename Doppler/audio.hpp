#pragma once

#include <SDL.h>
#include "playback_state.hpp"

class Audio {
private:
  PlaybackState     m_ps;
  SDL_AudioDeviceID m_device{};

  static void audio_callback(void* userdata, Uint8* byte_stream, int stream_bytes);
  void cleanup() noexcept;

public:
  Audio() noexcept               = default;
  Audio(const Audio&)            = delete;
  Audio(Audio&& other) noexcept;
  Audio& operator=(const Audio&) = delete;
  Audio& operator=(Audio&& other) noexcept;
  ~Audio() noexcept;

  void play(std::string_view filename);
  constexpr PlaybackState& get_ps() noexcept { return m_ps; }
};
