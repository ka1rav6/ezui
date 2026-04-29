#pragma once

#include <cstdint>
#include <string>
#include <algorithm>

namespace EzUI {

// ── Color ────────────────────────────────────────────────
struct Color {
    uint8_t r = 0, g = 0, b = 0, a = 255;

    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    /// Construct from hex string: "#RRGGBB" or "#RRGGBBAA"
    explicit Color(const std::string& hex) {
        if (hex.empty() || hex[0] != '#') return;
        auto val = std::stoul(hex.substr(1), nullptr, 16);
        if (hex.size() == 7) {          // #RRGGBB
            r = (val >> 16) & 0xFF;
            g = (val >> 8)  & 0xFF;
            b =  val        & 0xFF;
            a = 255;
        } else if (hex.size() == 9) {   // #RRGGBBAA
            r = (val >> 24) & 0xFF;
            g = (val >> 16) & 0xFF;
            b = (val >> 8)  & 0xFF;
            a =  val        & 0xFF;
        }
    }

    /// Lighten by factor (0.0 – 1.0)
    [[nodiscard]] Color lighter(float factor) const {
        auto lift = [&](uint8_t c) -> uint8_t {
            return static_cast<uint8_t>(std::min(255.0f, c + (255 - c) * factor));
        };
        return {lift(r), lift(g), lift(b), a};
    }

    /// Darken by factor (0.0 – 1.0)
    [[nodiscard]] Color darker(float factor) const {
        auto drop = [&](uint8_t c) -> uint8_t {
            return static_cast<uint8_t>(std::max(0.0f, c * (1.0f - factor)));
        };
        return {drop(r), drop(g), drop(b), a};
    }
};

// ── Geometry ─────────────────────────────────────────────
struct Point {
    float x = 0, y = 0;
};

struct Size {
    float w = 0, h = 0;
};

struct Rect {
    float x = 0, y = 0, w = 0, h = 0;

    [[nodiscard]] bool contains(float px, float py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

struct Padding {
    float top = 0, right = 0, bottom = 0, left = 0;

    Padding() = default;
    explicit Padding(float all) : top(all), right(all), bottom(all), left(all) {}
    Padding(float vertical, float horizontal)
        : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {}
    Padding(float t, float r, float b, float l)
        : top(t), right(r), bottom(b), left(l) {}

    [[nodiscard]] float horizontal() const { return left + right; }
    [[nodiscard]] float vertical()   const { return top + bottom; }
};

} // namespace EzUI
