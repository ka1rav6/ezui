#pragma once

#include "components.h"
#include <functional>

namespace EzUI {

/// Direction for stack layouts.
enum class Direction { Vertical, Horizontal };

// ─────────────────────────────────────────────────────────
//  StackComponent  –  VStack / HStack
// ─────────────────────────────────────────────────────────
class StackComponent : public Component {
public:
    StackComponent(Direction dir, float spacing, const Style& s = {});

    void render(SDL_Renderer* renderer, TTF_Font* font) override;
    void layout(Rect available) override;
    Size preferredSize(TTF_Font* font) const override;
    bool handleEvent(const SDL_Event& event) override;

private:
    Direction dir_;
    float spacing_;
};

} // namespace EzUI
