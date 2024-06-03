#include "sdl.hpp"

void SDL::cleanup() noexcept {
  if (m_ttf_inited) {
    TTF_Quit();
    m_ttf_inited = false;
  }
  if (m_renderer) {
    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;
  }
  if (m_window) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
  }
  if (m_inited) {
    SDL_Quit();
    m_inited = false;
  }
}

SDL::SDL() {
  m_inited = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO) == 0;
  if (!m_inited) {
    cleanup();
  }

  m_window = SDL_CreateWindow(default_title.data(),
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              resolution[0], resolution[1],
                              SDL_WINDOW_SHOWN);
  if (!m_window) {
    cleanup();
  }

  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (!m_renderer) {
    cleanup();
  }

  m_ttf_inited = TTF_Init() == 0;
  if (!m_ttf_inited) {
    cleanup();
  }
}

SDL::SDL(SDL&& other) noexcept {
  *this = std::move(other);
}

SDL& SDL::operator=(SDL&& other) noexcept {
  if (this != &other) {
    std::swap(m_inited     , other.m_inited);
    std::swap(m_ttf_inited , other.m_ttf_inited);
    std::swap(m_window     , other.m_window);
    std::swap(m_renderer   , other.m_renderer);
    std::swap(m_timer_tick , other.m_timer_tick);
    std::swap(m_timer_event, other.m_timer_event);
    other.cleanup();
  }

  return *this;
}

SDL::~SDL() noexcept {
  cleanup();
}

void SDL::run(const std::function<void(SDL&)>& impl) {
  for (bool running = true; running; ) {
    m_timer_tick.update();

    m_timer_event.if_expired([&running]{
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;

        default:
          break;
        }
      }
    });

    impl(*this);

    m_timer_tick.sleep();
  }
}

void SDL::draw_text(const Font& font, std::string_view text, const Vector<int, 2>& pos, const Vector<Uint8, 4>& color) {
  const SDL_Color sdl_color{ color[0], color[1], color[2], color[3] };
  auto text_surface = TTF_RenderText_Blended(font.get_font(), text.data(), sdl_color);
  auto text_texture = SDL_CreateTextureFromSurface(m_renderer, text_surface);
  const SDL_Rect text_rect{pos[0], pos[1], text_surface->w, text_surface->h};
  SDL_FreeSurface(text_surface);
  SDL_RenderCopy(m_renderer, text_texture, nullptr, &text_rect);
  SDL_DestroyTexture(text_texture);
}

void SDL::draw_rect(const Vector<int, 2>& pos, const Vector<int, 2>& dim, const Vector<Uint8, 4>& color) {
  const SDL_Rect rect{pos[0], pos[1], dim[0], dim[1]};
  SDL_SetRenderDrawColor(m_renderer, color[0], color[1], color[2], color[3]);
  SDL_RenderFillRect(m_renderer, &rect);
}

void SDL::draw_line(const Vector<int, 2>& begin, const Vector<int, 2>& end, const Vector<Uint8, 4>& color) {
  SDL_SetRenderDrawColor(m_renderer, color[0], color[1], color[2], color[3]);
  SDL_RenderDrawLine(m_renderer, begin[0], begin[1], end[0], end[1]);
}
