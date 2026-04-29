#pragma once

#include "types.h"
#include "style.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace EzUI {

// ─────────────────────────────────────────────────────────
//  Component  (abstract base)
// ─────────────────────────────────────────────────────────
class Component {
public:
    virtual ~Component() = default;

    /// Render this component (and children) to the renderer.
    virtual void render(SDL_Renderer* renderer, TTF_Font* font) = 0;

    /// Handle an SDL event.  Return true if consumed.
    virtual bool handleEvent(const SDL_Event& event) { (void)event; return false; }

    /// Compute layout for this component within `available` rect.
    virtual void layout(Rect available) { bounds_ = available; }

    /// Preferred (natural) size of this component.
    virtual Size preferredSize(TTF_Font* font) const { (void)font; return {0, 0}; }

    // ── Dirty tracking ──────────────────────────────────
    void markDirty()       { dirty_ = true; }
    void clearDirty()      { dirty_ = false; }
    [[nodiscard]] bool isDirty() const { return dirty_; }

    // ── Tree ────────────────────────────────────────────
    void addChild(std::unique_ptr<Component> child) {
        child->parent_ = this;
        children_.push_back(std::move(child));
    }

    [[nodiscard]] const std::vector<std::unique_ptr<Component>>& children() const {
        return children_;
    }

    std::vector<std::unique_ptr<Component>>& children() {
        return children_;
    }

    // ── Accessors ───────────────────────────────────────
    [[nodiscard]] const Rect&  bounds() const { return bounds_; }
    [[nodiscard]] const Style& style()  const { return style_; }
    void setStyle(const Style& s) { style_ = s; }

protected:
    Rect   bounds_{};
    Style  style_{};
    bool   dirty_ = true;
    Component* parent_ = nullptr;
    std::vector<std::unique_ptr<Component>> children_;

    // ── SDL helpers ─────────────────────────────────────
    static void setDrawColor(SDL_Renderer* r, const Color& c) {
        SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    }

    static void fillRect(SDL_Renderer* r, const Rect& rc) {
        SDL_Rect sr{(int)rc.x, (int)rc.y, (int)rc.w, (int)rc.h};
        SDL_RenderFillRect(r, &sr);
    }

    static void fillRoundedRect(SDL_Renderer* r, const Rect& rc, float radius);
    static void renderText(SDL_Renderer* r, TTF_Font* font,
                           const std::string& text, float x, float y,
                           const Color& color);
};

// ─────────────────────────────────────────────────────────
//  WindowComponent
// ─────────────────────────────────────────────────────────
class WindowComponent : public Component {
public:
    WindowComponent(const std::string& title, int width, int height,
                    const std::string& bg = "#1e1e1e");

    void render(SDL_Renderer* renderer, TTF_Font* font) override;
    void layout(Rect available) override;
    Size preferredSize(TTF_Font* font) const override;

    [[nodiscard]] const std::string& title()  const { return title_; }
    [[nodiscard]] int  width()  const { return width_; }
    [[nodiscard]] int  height() const { return height_; }

private:
    std::string title_;
    int width_, height_;
};

// ─────────────────────────────────────────────────────────
//  TextComponent
// ─────────────────────────────────────────────────────────
class TextComponent : public Component {
public:
    explicit TextComponent(const std::string& text, const Style& s = {});

    void render(SDL_Renderer* renderer, TTF_Font* font) override;
    Size preferredSize(TTF_Font* font) const override;

    void setText(const std::string& t) { text_ = t; markDirty(); }
    [[nodiscard]] const std::string& text() const { return text_; }

private:
    std::string text_;
};

// ─────────────────────────────────────────────────────────
//  ButtonComponent
// ─────────────────────────────────────────────────────────
class ButtonComponent : public Component {
public:
    using Callback = std::function<void()>;

    ButtonComponent(const std::string& label, Callback onClick,
                    const Style& s = {});

    void render(SDL_Renderer* renderer, TTF_Font* font) override;
    bool handleEvent(const SDL_Event& event) override;
    Size preferredSize(TTF_Font* font) const override;

private:
    std::string label_;
    Callback onClick_;
    bool hovered_ = false;
    bool pressed_ = false;
};

// ─────────────────────────────────────────────────────────
//  InputComponent
// ─────────────────────────────────────────────────────────
class InputComponent : public Component {
public:
    InputComponent(const std::string& placeholder,
                   std::function<std::string()> getter,
                   std::function<void(const std::string&)> setter,
                   const Style& s = {});

    void render(SDL_Renderer* renderer, TTF_Font* font) override;
    bool handleEvent(const SDL_Event& event) override;
    Size preferredSize(TTF_Font* font) const override;

private:
    std::string placeholder_;
    std::function<std::string()> getter_;
    std::function<void(const std::string&)> setter_;

    bool focused_  = false;
    bool hovered_  = false;
    int  cursorPos_= 0;
    Uint32 cursorBlink_ = 0;
};

} // namespace EzUI
