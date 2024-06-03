#include "audio.hpp"

#include <format>
#include "util.hpp"

void Audio::audio_callback(void* userdata, Uint8* byte_stream, int stream_bytes) {
  static std::array<float, PlaybackState::chunk_size> buffer;

  const auto stream        = reinterpret_cast<float*>(byte_stream);
  const auto stream_length = stream_bytes / sizeof(float);
  auto&      ps            = *static_cast<PlaybackState*>(userdata);

  if (ps.get_reached_eof()) {
    std::fill_n(stream, stream_length, 0.0f);
    return;
  }

  const auto& info               = ps.get_info();
  const auto  read               = ps.read(buffer);
  const auto  copy_length_per_ch = std::min(stream_length / 2, read / info.channels);
  const auto  pos_initial        = ps.get_position();

  ps.update_bistates();

  for (std::size_t i{}; i < copy_length_per_ch; ++i) {
    const auto t      = static_cast<float>(i) / static_cast<float>(copy_length_per_ch);
    const auto ff     = interpolate(ps.m_ff_b, t);
    const auto pan    = interpolate(ps.m_pan_b, t);
    const auto volume = interpolate(ps.m_volume_b, t);

    const auto offset_stream = i * 2;
    const auto offset_buffer = static_cast<std::size_t>(static_cast<float>(i) * ff) * info.channels;
    const auto pan_function  = [pan](bool left) {
      if ((0.0f < pan && left) || (pan < 0.0f && !left)) {
        return -0.75f * pan * pan + 1.0f;
      }
      else {
        return 1.0f;
      }
    };
    stream[offset_stream + 0]
      = buffer[offset_buffer + 0]                            * volume * pan_function(true);
    stream[offset_stream + 1]
      = buffer[offset_buffer + (info.channels == 2 ? 1 : 0)] * volume * pan_function(false);
    ps.set_position(pos_initial + offset_buffer + info.channels);
  }

  if (read < info.channels) {
    ps.set_reached_eof(true);
  }
}

void Audio::cleanup() noexcept {
  if (m_device) {
    SDL_CloseAudioDevice(m_device);
    m_device = 0;
  }
}

Audio::~Audio() noexcept {
  cleanup();
}

void Audio::play(std::string_view filename) {
  cleanup();

  m_ps.open(filename);
  const auto& info = m_ps.get_info();

  if (!(info.channels == 1 || info.channels == 2)) {
    m_ps.set_reached_eof(true);
    return;
  }

  SDL_AudioSpec desired{}, obtained{};
  desired.freq     = info.hz;
  desired.format   = AUDIO_F32SYS;
  desired.channels = 2;
  desired.samples  = static_cast<Uint16>(PlaybackState::base_chunk_size / 2); // samples per channel
  desired.userdata = &m_ps;
  desired.callback = audio_callback;
  m_device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
  if (!m_device) {
    messagebox(std::format("SDL_OpenAudioDevice failed: {}", SDL_GetError()));
    return;
  }
  SDL_PauseAudioDevice(m_device, 0);
}
