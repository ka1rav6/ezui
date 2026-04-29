#pragma once

/// ─── EzUI ── Single-include public API ─────────────────
///
///  #include <ezui/ezui.h>
///  using namespace EzUI;
///
///  int main() {
///      App app;
///      app.build([]() {
///          Window("Hello", 400, 300);
///          Text("Hello, World!");
///      });
///      app.run();
///  }

#include "types.h"
#include "style.h"
#include "state.h"
#include "components.h"
#include "layout.h"
#include "app.h"
#include "console_bridge.h"

#include <string>
#include <functional>
#include <iostream>
#include <memory>

namespace EzUI {

// ── Convenience free functions ──────────────────────────
// These operate on App::current() during a build() call.

/// Create / configure the application window.
inline void Window(const std::string& title, int width, int height,
                   const std::string& bg = "#1e1e1e") {
    auto* app = App::current();
    if (!app) return;
    auto w = std::make_unique<WindowComponent>(title, width, height, bg);
    // Default window padding
    Style s = w->style();
    s.padding = Padding(20, 30);
    w->setStyle(s);
    app->pushWindow(std::move(w));
}

/// Add a text label.
inline void Text(const std::string& text, const Style& style = {}) {
    auto* app = App::current();
    if (!app) return;
    auto c = std::make_unique<TextComponent>(text, style);
    // Measure with current font
    if (app->font()) {
        Size pref = c->preferredSize(app->font());
        c->layout({0, 0, pref.w, pref.h});
    }
    app->pushComponent(std::move(c));
}

/// Add a button with a click callback.
inline void Button(const std::string& label,
                   std::function<void()> onClick,
                   const Style& style = {}) {
    auto* app = App::current();
    if (!app) return;
    Style s = style;
    // Default button style if bg is still the default surface color
    if (s.bg.r == 0x2d && s.bg.g == 0x2d && s.bg.b == 0x2d) {
        s.bg = Theme::primary();
        s.hoverBg = Theme::primary().lighter(0.15f);
        s.color = Color("#ffffff");
    }
    auto c = std::make_unique<ButtonComponent>(label, std::move(onClick), s);
    if (app->font()) {
        Size pref = c->preferredSize(app->font());
        c->layout({0, 0, pref.w, pref.h});
    }
    app->pushComponent(std::move(c));
}

/// Add a text input field bound to a State<std::string>.
template <typename StateT>
void Input(const std::string& placeholder, StateT& state,
           const Style& style = {}) {
    auto* app = App::current();
    if (!app) return;

    auto getter = [&state]() -> std::string { return state.get(); };
    auto setter = [&state](const std::string& v) { state = v; };

    auto c = std::make_unique<InputComponent>(
        placeholder, std::move(getter), std::move(setter), style);
    if (app->font()) {
        Size pref = c->preferredSize(app->font());
        c->layout({0, 0, pref.w, pref.h});
    }
    app->pushComponent(std::move(c));
}

/// Begin a vertical stack.  Call the builder inside, then the stack closes.
inline void VStack(float spacing, std::function<void()> builder,
                   const Style& style = {}) {
    auto* app = App::current();
    if (!app) return;

    auto stack = std::make_unique<StackComponent>(
        Direction::Vertical, spacing, style);
    auto* rawPtr = stack.get();
    app->pushComponent(std::move(stack));
    app->pushContainer(rawPtr);

    if (builder) builder();

    app->popContainer();
}

/// Begin a horizontal stack.
inline void HStack(float spacing, std::function<void()> builder,
                   const Style& style = {}) {
    auto* app = App::current();
    if (!app) return;

    auto stack = std::make_unique<StackComponent>(
        Direction::Horizontal, spacing, style);
    auto* rawPtr = stack.get();
    app->pushComponent(std::move(stack));
    app->pushContainer(rawPtr);

    if (builder) builder();

    app->popContainer();
}

/// Print to stdout (works the same in GUI and CLI).
template <typename... Args>
void print(Args&&... args) {
    (std::cout << ... << std::forward<Args>(args)) << "\n";
}

} // namespace EzUI
