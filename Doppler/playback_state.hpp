#pragma once

#include <string_view>
#include <array>

#define MINIMP3_FLOAT_OUTPUT
#include <minimp3_ex.h>

#include "bistate.hpp"

class PlaybackState {
private:
  mp3dec_ex_t    m_dec{};
  std::size_t    m_position{};
  float          m_ff{ 1.0f };
  float          m_pan{};
  float          m_volume{ 1.0f };
  bool           m_reached_eof{};
  bool           m_opened{};

  void cleanup() noexcept;

public:
  static constexpr std::size_t base_chunk_size = 256;
  static constexpr std::size_t max_ff          = 4;
  static constexpr std::size_t chunk_size      = base_chunk_size * max_ff;

  Bistate<float> m_ff_b, m_pan_b, m_volume_b;

  constexpr PlaybackState() noexcept             = default;
  PlaybackState(const PlaybackState&)            = delete;
  PlaybackState(PlaybackState&&)                 = delete;
  PlaybackState& operator=(const PlaybackState&) = delete;
  PlaybackState& operator=(PlaybackState&&)      = delete;
  ~PlaybackState() noexcept;

  void open(std::string_view filename);
  void advance(std::size_t step) noexcept;
  std::size_t read(std::array<float, chunk_size>& buffer);
  void update_bistates() noexcept;
  void set_position(std::size_t position) noexcept;
  void set_ff(float ff) noexcept;
  void set_reached_eof(bool reached) noexcept;
  void set_pan(float pan) noexcept;
  void set_volume(float volume) noexcept;
  constexpr const auto& get_info() const noexcept { return m_dec.info; }
  constexpr auto get_position() const noexcept { return m_position; }
  constexpr auto get_ff() const noexcept { return m_ff; }
  constexpr auto get_reached_eof() const noexcept { return m_reached_eof; }
  constexpr auto get_pan() const noexcept { return m_pan; }
  constexpr auto get_volume() const noexcept { return m_volume; }
  constexpr auto get_samples() const noexcept { return m_dec.samples; }

  template <class T>
  requires (std::is_floating_point_v<T>)
  constexpr auto get_progress() const noexcept {
    return m_opened ? static_cast<T>(m_position) / static_cast<T>(m_dec.samples) : T{};
  }
};
