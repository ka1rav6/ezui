#include "ezui/components.h"
#include "ezui/state.h"
#include <cmath>
#include <algorithm>

namespace EzUI {

// ══════════════════════════════════════════════════════════
//  Component  (static helpers)
// ══════════════════════════════════════════════════════════

void Component::fillRoundedRect(SDL_Renderer* r, const Rect& rc, float radius) {
    if (radius <= 0) {
        fillRect(r, rc);
        return;
    }

    int x  = (int)rc.x, y  = (int)rc.y;
    int w  = (int)rc.w, h  = (int)rc.h;
    int rad = (int)std::min(radius, std::min(rc.w, rc.h) / 2.0f);

    // Main body (excluding corners)
    SDL_Rect body{x + rad, y, w - 2 * rad, h};
    SDL_RenderFillRect(r, &body);
    SDL_Rect left{x, y + rad, rad, h - 2 * rad};
    SDL_RenderFillRect(r, &left);
    SDL_Rect right{x + w - rad, y + rad, rad, h - 2 * rad};
    SDL_RenderFillRect(r, &right);

    // Draw the four corners as filled circles (quarter arcs)
    auto fillCircleQuarter = [&](int cx, int cy, int r_rad,
                                  int dx_sign, int dy_sign) {
        for (int dy = 0; dy <= r_rad; ++dy) {
            int dx = (int)std::sqrt((double)(r_rad * r_rad - dy * dy));
            int startX = (dx_sign > 0) ? cx : cx - dx;
            SDL_Rect row{startX, (dy_sign > 0) ? cy + dy : cy - dy, dx, 1};
            SDL_RenderFillRect(r, &row);
        }
    };

    // Top-left
    fillCircleQuarter(x + rad, y + rad, rad, -1, -1);
    // Top-right
    fillCircleQuarter(x + w - rad, y + rad, rad, 1, -1);
    // Bottom-left
    fillCircleQuarter(x + rad, y + h - rad, rad, -1, 1);
    // Bottom-right
    fillCircleQuarter(x + w - rad, y + h - rad, rad, 1, 1);
}

void Component::renderText(SDL_Renderer* r, TTF_Font* font,
                           const std::string& text, float x, float y,
                           const Color& color) {
    if (text.empty() || !font) return;

    SDL_Color sdlc{color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), sdlc);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(r, surface);
    if (texture) {
        SDL_Rect dst{(int)x, (int)y, surface->w, surface->h};
        SDL_RenderCopy(r, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

// ══════════════════════════════════════════════════════════
//  WindowComponent
// ══════════════════════════════════════════════════════════

WindowComponent::WindowComponent(const std::string& title, int width,
                                 int height, const std::string& bg)
    : title_(title), width_(width), height_(height) {
    style_.bg = Color(bg);
}

void WindowComponent::render(SDL_Renderer* renderer, TTF_Font* font) {
    setDrawColor(renderer, style_.bg);
    SDL_RenderClear(renderer);

    for (auto& child : children_) {
        child->render(renderer, font);
    }
    dirty_ = false;
}

void WindowComponent::layout(Rect /*available*/) {
    bounds_ = {0, 0, (float)width_, (float)height_};
    // By default, children stack vertically with padding
    float yOffset = style_.padding.top;
    float xPad    = style_.padding.left;
    float availW  = (float)width_ - style_.padding.horizontal();

    for (auto& child : children_) {
        child->layout({xPad, yOffset, availW, 0});
        yOffset += child->bounds().h + 8;  // 8px gap between children
    }
}

Size WindowComponent::preferredSize(TTF_Font*) const {
    return {(float)width_, (float)height_};
}

// ══════════════════════════════════════════════════════════
//  TextComponent
// ══════════════════════════════════════════════════════════

TextComponent::TextComponent(const std::string& text, const Style& s)
    : text_(text) {
    style_ = s;
    style_.bg = Color(0, 0, 0, 0);  // transparent by default
}

void TextComponent::render(SDL_Renderer* renderer, TTF_Font* font) {
    renderText(renderer, font, text_,
               bounds_.x + style_.padding.left,
               bounds_.y + style_.padding.top,
               style_.color);
    dirty_ = false;
}

Size TextComponent::preferredSize(TTF_Font* font) const {
    if (!font || text_.empty()) return {style_.padding.horizontal(), 30};

    int tw = 0, th = 0;
    TTF_SizeUTF8(font, text_.c_str(), &tw, &th);
    return {(float)tw + style_.padding.horizontal(),
            (float)th + style_.padding.vertical()};
}

// ══════════════════════════════════════════════════════════
//  ButtonComponent
// ══════════════════════════════════════════════════════════

ButtonComponent::ButtonComponent(const std::string& label, Callback onClick,
                                 const Style& s)
    : label_(label), onClick_(std::move(onClick)) {
    style_ = s;
    if (style_.hoverBg.r == 0x3a && style_.hoverBg.g == 0x3a) {
        // Auto-generate hover color if still default
        style_.hoverBg = style_.bg.lighter(0.15f);
    }
    style_.activeBg = style_.bg.darker(0.1f);
}

void ButtonComponent::render(SDL_Renderer* renderer, TTF_Font* font) {
    Color bg = pressed_ ? style_.activeBg
             : hovered_ ? style_.hoverBg
             : style_.bg;

    setDrawColor(renderer, bg);
    fillRoundedRect(renderer, bounds_, style_.borderRadius);

    // Center text
    if (font && !label_.empty()) {
        int tw = 0, th = 0;
        TTF_SizeUTF8(font, label_.c_str(), &tw, &th);
        float tx = bounds_.x + (bounds_.w - tw) / 2;
        float ty = bounds_.y + (bounds_.h - th) / 2;
        renderText(renderer, font, label_, tx, ty, style_.color);
    }

    dirty_ = false;
}

bool ButtonComponent::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int mx = event.motion.x, my = event.motion.y;
        bool wasHovered = hovered_;
        hovered_ = bounds_.contains((float)mx, (float)my);
        if (wasHovered != hovered_) markDirty();
        return hovered_;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mx = event.button.x, my = event.button.y;
        if (bounds_.contains((float)mx, (float)my)) {
            pressed_ = true;
            markDirty();
            return true;
        }
    }
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        if (pressed_) {
            pressed_ = false;
            int mx = event.button.x, my = event.button.y;
            if (bounds_.contains((float)mx, (float)my) && onClick_) {
                onClick_();
            }
            markDirty();
            return true;
        }
    }
    return false;
}

Size ButtonComponent::preferredSize(TTF_Font* font) const {
    if (!font || label_.empty())
        return {100 + style_.padding.horizontal(),
                30  + style_.padding.vertical()};

    int tw = 0, th = 0;
    TTF_SizeUTF8(font, label_.c_str(), &tw, &th);
    return {(float)tw + style_.padding.horizontal(),
            (float)th + style_.padding.vertical()};
}

// ══════════════════════════════════════════════════════════
//  InputComponent
// ══════════════════════════════════════════════════════════

InputComponent::InputComponent(const std::string& placeholder,
                               std::function<std::string()> getter,
                               std::function<void(const std::string&)> setter,
                               const Style& s)
    : placeholder_(placeholder),
      getter_(std::move(getter)),
      setter_(std::move(setter)) {
    style_ = s;
    style_.borderWidth = 1.5f;
    style_.borderColor = Color("#555555");
}

void InputComponent::render(SDL_Renderer* renderer, TTF_Font* font) {
    // Background
    Color bg = focused_ ? style_.bg.lighter(0.08f) : style_.bg;
    setDrawColor(renderer, bg);
    fillRoundedRect(renderer, bounds_, style_.borderRadius);

    // Border
    if (style_.borderWidth > 0) {
        Color bc = focused_ ? Theme::primary() : style_.borderColor;
        setDrawColor(renderer, bc);
        // Draw border outline (top, bottom, left, right)
        SDL_Rect top   {(int)bounds_.x, (int)bounds_.y, (int)bounds_.w, (int)style_.borderWidth};
        SDL_Rect bottom{(int)bounds_.x, (int)(bounds_.y + bounds_.h - style_.borderWidth), (int)bounds_.w, (int)style_.borderWidth};
        SDL_Rect left  {(int)bounds_.x, (int)bounds_.y, (int)style_.borderWidth, (int)bounds_.h};
        SDL_Rect right {(int)(bounds_.x + bounds_.w - style_.borderWidth), (int)bounds_.y, (int)style_.borderWidth, (int)bounds_.h};
        SDL_RenderFillRect(renderer, &top);
        SDL_RenderFillRect(renderer, &bottom);
        SDL_RenderFillRect(renderer, &left);
        SDL_RenderFillRect(renderer, &right);
    }

    std::string value = getter_ ? getter_() : "";
    float textX = bounds_.x + style_.padding.left;
    float textY = bounds_.y + style_.padding.top;

    if (value.empty() && !focused_) {
        // Placeholder
        renderText(renderer, font, placeholder_, textX, textY,
                   style_.color.darker(0.4f));
    } else {
        renderText(renderer, font, value, textX, textY, style_.color);
    }

    // Blinking cursor
    if (focused_ && font) {
        Uint32 now = SDL_GetTicks();
        if ((now - cursorBlink_) % 1000 < 500) {
            int tw = 0, th = 0;
            std::string beforeCursor = value.substr(0, std::min((int)value.size(), cursorPos_));
            TTF_SizeUTF8(font, beforeCursor.empty() ? " " : beforeCursor.c_str(), &tw, &th);
            if (beforeCursor.empty()) tw = 0;

            setDrawColor(renderer, style_.cursorColor);
            SDL_Rect cur{(int)(textX + tw), (int)textY, 2, th};
            SDL_RenderFillRect(renderer, &cur);
        }
        markDirty();  // keep refreshing for cursor blink
    }

    dirty_ = false;
}

bool InputComponent::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mx = event.button.x, my = event.button.y;
        bool wasFocused = focused_;
        focused_ = bounds_.contains((float)mx, (float)my);
        if (focused_ != wasFocused) {
            cursorBlink_ = SDL_GetTicks();
            markDirty();
            if (focused_) {
                SDL_StartTextInput();
                std::string val = getter_ ? getter_() : "";
                cursorPos_ = (int)val.size();
            } else {
                SDL_StopTextInput();
            }
        }
        return focused_;
    }

    if (!focused_) return false;

    if (event.type == SDL_TEXTINPUT) {
        std::string val = getter_ ? getter_() : "";
        val.insert(cursorPos_, event.text.text);
        cursorPos_ += (int)std::string(event.text.text).size();
        if (setter_) setter_(val);
        setGlobalDirty();
        markDirty();
        return true;
    }

    if (event.type == SDL_KEYDOWN) {
        std::string val = getter_ ? getter_() : "";
        switch (event.key.keysym.sym) {
            case SDLK_BACKSPACE:
                if (cursorPos_ > 0 && !val.empty()) {
                    val.erase(cursorPos_ - 1, 1);
                    cursorPos_--;
                    if (setter_) setter_(val);
                    setGlobalDirty();
                    markDirty();
                }
                return true;
            case SDLK_DELETE:
                if (cursorPos_ < (int)val.size()) {
                    val.erase(cursorPos_, 1);
                    if (setter_) setter_(val);
                    setGlobalDirty();
                    markDirty();
                }
                return true;
            case SDLK_LEFT:
                if (cursorPos_ > 0) { cursorPos_--; markDirty(); }
                return true;
            case SDLK_RIGHT:
                if (cursorPos_ < (int)val.size()) { cursorPos_++; markDirty(); }
                return true;
            case SDLK_HOME:
                cursorPos_ = 0; markDirty();
                return true;
            case SDLK_END:
                cursorPos_ = (int)val.size(); markDirty();
                return true;
            default:
                break;
        }
    }

    return false;
}

Size InputComponent::preferredSize(TTF_Font* font) const {
    int th = 20;
    if (font) {
        TTF_SizeUTF8(font, "Ay", nullptr, &th);
    }
    return {300 + style_.padding.horizontal(),
            (float)th + style_.padding.vertical()};
}

} // namespace EzUI
