#pragma once

#include <string_view>
#include <array>
#include <memory>

#define MINIMP3_FLOAT_OUTPUT
#include <minimp3_ex.h>

#include "bistate.hpp"

class PlaybackState {
private:
  std::unique_ptr<mp3dec_ex_t> m_dec;
  struct Params {
    std::size_t    position{};
    float          ff{ 1.0f }, pan{}, volume{ 1.0f };
    bool           reached_eof{}, opened{};
    Bistate<float> ff_b, pan_b, volume_b;
  } m_params;

  void cleanup() noexcept;

public:
  static constexpr std::size_t base_chunk_size = 0x100;
  static constexpr std::size_t max_ff          = 4;
  static constexpr std::size_t chunk_size      = base_chunk_size * max_ff;

  PlaybackState();
  PlaybackState(const PlaybackState&)            = delete;
  PlaybackState(PlaybackState&& other) noexcept;
  PlaybackState& operator=(const PlaybackState&) = delete;
  PlaybackState& operator=(PlaybackState&& other) noexcept;
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
  constexpr const auto& get_info() const noexcept { return m_dec->info; }
  constexpr auto get_samples() const noexcept { return m_dec->samples; }
  constexpr auto get_position() const noexcept { return m_params.position; }
  constexpr auto get_ff() const noexcept { return m_params.ff; }
  constexpr auto get_pan() const noexcept { return m_params.pan; }
  constexpr auto get_volume() const noexcept { return m_params.volume; }
  constexpr auto get_reached_eof() const noexcept { return m_params.reached_eof; }
  constexpr auto get_opened() const noexcept { return m_params.opened; }
  constexpr auto& get_ff_b() const noexcept { return m_params.ff_b; }
  constexpr auto& get_pan_b() const noexcept { return m_params.pan_b; }
  constexpr auto& get_volume_b() const noexcept { return m_params.volume_b; }

  template <class T>
  requires std::is_floating_point_v<T>
  constexpr auto get_progress() const noexcept {
    return m_params.opened ? static_cast<T>(m_params.position) / static_cast<T>(get_samples()) : T{};
  }
};
