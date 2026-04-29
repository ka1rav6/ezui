#include "ezui/app.h"
#include "ezui/state.h"

#include <iostream>
#include <stdexcept>
#include <filesystem>

namespace EzUI {

App* App::s_current_ = nullptr;

// ── Constructor / Destructor ─────────────────────────────

App::App()  { initSDL(); }
App::~App() { shutdownSDL(); }

// ── SDL lifecycle ────────────────────────────────────────

void App::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
        throw std::runtime_error(std::string("SDL_Init: ") + SDL_GetError());

    if (TTF_Init() != 0)
        throw std::runtime_error(std::string("TTF_Init: ") + TTF_GetError());

    // Try several common font paths
    const char* fontPaths[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        nullptr
    };

    for (int i = 0; fontPaths[i]; ++i) {
        if (std::filesystem::exists(fontPaths[i])) {
            font_ = TTF_OpenFont(fontPaths[i], 16);
            if (font_) break;
        }
    }

    if (!font_) {
        std::cerr << "[EzUI] Warning: Could not load any system font.\n";
    }
}

void App::shutdownSDL() {
    if (font_)        { TTF_CloseFont(font_); font_ = nullptr; }
    if (sdlRenderer_) { SDL_DestroyRenderer(sdlRenderer_); sdlRenderer_ = nullptr; }
    if (sdlWindow_)   { SDL_DestroyWindow(sdlWindow_); sdlWindow_ = nullptr; }
    TTF_Quit();
    SDL_Quit();
}

// ── Build ────────────────────────────────────────────────

void App::build(std::function<void()> builder) {
    builder_ = std::move(builder);
}

void App::rebuild() {
    root_.reset();
    containerStack_.clear();

    s_current_ = this;
    if (builder_) builder_();
    s_current_ = nullptr;

    if (!root_) {
        throw std::runtime_error("[EzUI] build() must call Window(...)");
    }

    // Create the actual SDL window now that we know dimensions
    if (!sdlWindow_) {
        sdlWindow_ = SDL_CreateWindow(
            root_->title().c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            root_->width(), root_->height(),
            SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
        if (!sdlWindow_)
            throw std::runtime_error(std::string("SDL_CreateWindow: ") + SDL_GetError());

        sdlRenderer_ = SDL_CreateRenderer(sdlWindow_, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!sdlRenderer_)
            throw std::runtime_error(std::string("SDL_CreateRenderer: ") + SDL_GetError());

        SDL_SetRenderDrawBlendMode(sdlRenderer_, SDL_BLENDMODE_BLEND);
    }

    // Layout
    root_->layout({0, 0, (float)root_->width(), (float)root_->height()});
    setGlobalDirty();
}

// ── Singleton access ─────────────────────────────────────

App* App::current() { return s_current_; }

void App::pushWindow(std::unique_ptr<WindowComponent> w) {
    root_ = std::move(w);
    containerStack_.clear();
    containerStack_.push_back(root_.get());
}

void App::pushComponent(std::unique_ptr<Component> c) {
    if (containerStack_.empty()) {
        throw std::runtime_error(
            "[EzUI] Component created outside a Window/container");
    }
    containerStack_.back()->addChild(std::move(c));
}

void App::pushContainer(StackComponent* container) {
    containerStack_.push_back(container);
}

void App::popContainer() {
    if (containerStack_.size() > 1)
        containerStack_.pop_back();
}

// ── Event dispatch ───────────────────────────────────────

void App::dispatchEvent(const SDL_Event& e) {
    if (!root_) return;

    // Propagate to all children (depth-first, reverse order)
    std::function<bool(Component*)> dispatch = [&](Component* c) -> bool {
        for (auto it = c->children().rbegin(); it != c->children().rend(); ++it) {
            if (dispatch(it->get())) return true;
        }
        return c->handleEvent(e);
    };

    dispatch(root_.get());
}

// ── Render ───────────────────────────────────────────────

void App::renderFrame() {
    if (!root_ || !sdlRenderer_) return;
    root_->render(sdlRenderer_, font_);
    SDL_RenderPresent(sdlRenderer_);
}

// ── Main loop ────────────────────────────────────────────

void App::run() {
    rebuild();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            dispatchEvent(event);
        }

        // Re-build the tree if state changed (to pick up new text values etc.)
        if (anyStateDirty()) {
            // Save the SDL resources – don't recreate the window
            auto savedWindow   = sdlWindow_;
            auto savedRenderer = sdlRenderer_;

            root_.reset();
            containerStack_.clear();

            s_current_ = this;
            if (builder_) builder_();
            s_current_ = nullptr;

            sdlWindow_   = savedWindow;
            sdlRenderer_ = savedRenderer;

            if (root_) {
                root_->layout({0, 0, (float)root_->width(), (float)root_->height()});
            }
            clearGlobalDirty();
        }

        renderFrame();
        SDL_Delay(16);  // ~60 FPS cap
    }
}

} // namespace EzUI
