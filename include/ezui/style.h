#pragma once

#include "types.h"
#include <string>

namespace EzUI {

/// Visual style for any component.
struct Style {
    Color bg            = Color("#2d2d2d");     // background
    Color color         = Color("#e0e0e0");     // text / foreground
    Color hoverBg       = Color("#3a3a3a");     // background on hover
    Color activeBg      = Color("#505050");     // background on press
    Color borderColor   = Color("#555555");     // border color
    Color cursorColor   = Color("#ffffff");     // text cursor color

    float fontSize      = 16.0f;
    float borderRadius  = 6.0f;
    float borderWidth   = 0.0f;

    Padding padding     = Padding(10, 18);
    Size   minSize      = {0, 0};
    Size   maxSize      = {1e6f, 1e6f};
};

// ── Pre-built theme colours ──────────────────────────────
namespace Theme {
    inline Color background()   { return Color("#1e1e1e"); }
    inline Color surface()      { return Color("#2d2d2d"); }
    inline Color primary()      { return Color("#6c63ff"); }
    inline Color secondary()    { return Color("#3dd6d0"); }
    inline Color accent()       { return Color("#e74c3c"); }
    inline Color textPrimary()  { return Color("#e0e0e0"); }
    inline Color textSecondary(){ return Color("#a0a0a0"); }
    inline Color success()      { return Color("#2ecc71"); }
    inline Color warning()      { return Color("#f39c12"); }
    inline Color error()        { return Color("#e74c3c"); }
} // namespace Theme

} // namespace EzUI
