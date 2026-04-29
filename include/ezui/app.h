#pragma once

#include "components.h"
#include "layout.h"
#include "state.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace EzUI {

/// ── App ─────────────────────────────────────────────────
/// The top-level runtime.  Hides the SDL event loop.
///
///   App app;
///   app.build([]() {
///       Window("Hello", 400, 300);
///       Text("Hi!");
///   });
///   app.run();
///
class App {
public:
    App();
    ~App();

    /// Set the builder function.  This will be called once to
    /// construct the component tree.
    void build(std::function<void()> builder);

    /// Enter the event loop (blocks until window close).
    void run();

    // ── Used by the free-function API  ──────────────────
    static App* current();   // the app whose build() is executing

    void pushWindow(std::unique_ptr<WindowComponent> w);
    void pushComponent(std::unique_ptr<Component> c);
    void pushContainer(StackComponent* container);
    void popContainer();

    [[nodiscard]] TTF_Font* font() const { return font_; }

private:
    void initSDL();
    void shutdownSDL();
    void rebuild();
    void dispatchEvent(const SDL_Event& e);
    void renderFrame();

    std::function<void()> builder_;

    // SDL resources
    SDL_Window*   sdlWindow_   = nullptr;
    SDL_Renderer* sdlRenderer_ = nullptr;
    TTF_Font*     font_        = nullptr;

    // Component tree
    std::unique_ptr<WindowComponent> root_;

    // Build-time container stack (for nesting VStack/HStack)
    std::vector<Component*> containerStack_;

    // Singleton for build context
    static App* s_current_;
};

} // namespace EzUI
