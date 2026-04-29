#include "ezui/layout.h"
#include <algorithm>
#include <numeric>

namespace EzUI {

StackComponent::StackComponent(Direction dir, float spacing, const Style& s)
    : dir_(dir), spacing_(spacing) {
    style_ = s;
    style_.bg = Color(0, 0, 0, 0);  // transparent by default
}

void StackComponent::render(SDL_Renderer* renderer, TTF_Font* font) {
    for (auto& child : children_) {
        child->render(renderer, font);
    }
    dirty_ = false;
}

void StackComponent::layout(Rect available) {
    bounds_ = available;

    if (children_.empty()) return;

    float padX = style_.padding.left;
    float padY = style_.padding.top;
    float innerW = available.w - style_.padding.horizontal();

    if (dir_ == Direction::Vertical) {
        // ── VERTICAL STACK ──────────────────────────────
        // Measure pass: compute total preferred height
        float totalH = 0;
        for (auto& child : children_) {
            Size pref = child->preferredSize(nullptr);
            totalH += pref.h;
        }
        totalH += spacing_ * (float)(children_.size() - 1);

        // Arrange pass
        float yOff = available.y + padY;
        for (auto& child : children_) {
            Size pref = child->preferredSize(nullptr);
            float childW = std::min(pref.w, innerW);
            // Center horizontally within the stack
            float childX = available.x + padX + (innerW - childW) / 2;
            child->layout({childX, yOff, childW, pref.h});
            yOff += pref.h + spacing_;
        }

        // Update our own bounds height
        bounds_.h = yOff - available.y + style_.padding.bottom;

    } else {
        // ── HORIZONTAL STACK ────────────────────────────
        // Measure pass
        float totalW = 0;
        float maxH   = 0;
        for (auto& child : children_) {
            Size pref = child->preferredSize(nullptr);
            totalW += pref.w;
            maxH = std::max(maxH, pref.h);
        }
        totalW += spacing_ * (float)(children_.size() - 1);

        // Start centered horizontally within available
        float xOff = available.x + padX + (innerW - totalW) / 2;
        if (xOff < available.x + padX) xOff = available.x + padX;

        for (auto& child : children_) {
            Size pref = child->preferredSize(nullptr);
            // Center vertically within the tallest child
            float childY = available.y + padY + (maxH - pref.h) / 2;
            child->layout({xOff, childY, pref.w, pref.h});
            xOff += pref.w + spacing_;
        }

        bounds_.h = maxH + style_.padding.vertical();
    }
}

Size StackComponent::preferredSize(TTF_Font* font) const {
    if (children_.empty()) return {0, 0};

    float totalMain  = 0;
    float maxCross   = 0;

    for (auto& child : children_) {
        Size pref = child->preferredSize(font);
        if (dir_ == Direction::Vertical) {
            totalMain += pref.h;
            maxCross = std::max(maxCross, pref.w);
        } else {
            totalMain += pref.w;
            maxCross = std::max(maxCross, pref.h);
        }
    }
    totalMain += spacing_ * (float)(children_.size() - 1);

    if (dir_ == Direction::Vertical)
        return {maxCross + style_.padding.horizontal(),
                totalMain + style_.padding.vertical()};
    else
        return {totalMain + style_.padding.horizontal(),
                maxCross + style_.padding.vertical()};
}

bool StackComponent::handleEvent(const SDL_Event& event) {
    // Propagate to children (reverse order so top-most gets first crack)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        if ((*it)->handleEvent(event)) return true;
    }
    return false;
}

} // namespace EzUI
