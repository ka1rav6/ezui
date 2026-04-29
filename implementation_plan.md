# CppGUI — Modern Minimal C++ GUI Library

A high-performance, intuitive C++ GUI library that abstracts SDL2 complexity behind a clean declarative API.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                   USER APPLICATION LAYER                    │
│                                                             │
│   Window("My App", 800, 600, bg="#1e1e1e");                │
│   Button("Click Me", []{ print("Hello"); });               │
│   Text("Counter: " + to_string(count));                    │
│   Input("Name", name_var);                                 │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│                  DECLARATIVE API LAYER                      │
│                                                             │
│   EzUI::Window()  EzUI::Button()  EzUI::Text()            │
│   EzUI::Input()   EzUI::VStack()  EzUI::HStack()          │
│   EzUI::State<T>  EzUI::Run()     EzUI::print()           │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│                    CORE RUNTIME LAYER                       │
│                                                             │
│  ┌──────────┐  ┌──────────────┐  ┌───────────────────┐     │
│  │  Event   │  │    Layout    │  │  State Manager    │     │
│  │  Loop    │  │    Engine    │  │  (dirty tracking) │     │
│  └────┬─────┘  └──────┬───────┘  └────────┬──────────┘     │
│       │               │                   │                │
│  ┌────┴───────────────┴───────────────────┴──────────┐     │
│  │              Component Tree (Retained)             │     │
│  │   Window ─── VStack ─── Button                    │     │
│  │                     └── Text                       │     │
│  │                     └── Input                      │     │
│  └────────────────────────────────────────────────────┘     │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│                    BACKEND LAYER (SDL2)                      │
│                                                             │
│   SDL_Window    SDL_Renderer    SDL_Event                   │
│   SDL_Texture   TTF_Font        Input Handling              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## Design Decisions

### 1. UI Model: **Hybrid Retained + Declarative Rebuild**

| Approach | Pros | Cons |
|---|---|---|
| Immediate mode (Dear ImGui) | Simple mental model, no state sync | Full redraw every frame, hard to optimize |
| Retained mode (Qt) | Efficient updates, persistent tree | Complex API, heavy boilerplate |
| **Hybrid (our choice)** | Clean API + efficient updates | Slightly more internal complexity |

**Our approach**: User code runs a **build function** once (like a declarative description). Internally we build a **retained component tree**. When `State<T>` variables change, only the affected subtree is marked dirty and re-rendered. This gives us:
- The **clean API** of immediate mode (user just calls `Button(...)`, `Text(...)`)
- The **performance** of retained mode (no full redraw every frame)
- **Automatic dirty tracking** via `State<T>` wrapper

### 2. Event Loop: **Hidden Behind `EzUI::Run()`**

The user never writes an event loop. Instead:
```cpp
int main() {
    EzUI::App app;
    app.build([]() {
        Window("My App", 800, 600);
        Button("Click Me", [] { print("Hello!"); });
    });
    app.run();  // blocks, handles everything
}
```

`app.run()` internally:
1. Calls the build function to construct the component tree
2. Computes layout
3. Enters the SDL event loop
4. Dispatches events to hit-tested components
5. Re-renders only dirty regions

### 3. Component Storage: **Tree of `std::unique_ptr<Component>`**

Each component inherits from a base `Component` class:
```cpp
class Component {
    std::string id;
    Rect bounds;          // computed by layout
    Style style;          // colors, padding, font size
    bool dirty = true;
    Component* parent = nullptr;
    std::vector<std::unique_ptr<Component>> children;

    virtual void render(SDL_Renderer* r) = 0;
    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual void layout(Rect available) = 0;
};
```

### 4. State Management: **`State<T>` with Observer Pattern**

```cpp
State<int> count(0);   // reactive variable

// When count changes, any component referencing it is marked dirty
count = 5;             // triggers dirty flag on dependent components
count.get();           // returns current value
```

Implementation: `State<T>` holds a value + a list of component observers. On `operator=`, it notifies observers to mark themselves dirty. The render loop only redraws dirty components.

### 5. Layout: **Simple Stack-Based**

- `VStack` → children laid out top-to-bottom
- `HStack` → children laid out left-to-right
- Each component has `preferred_size()`, padding, and margin
- Layout is recomputed only when the tree structure changes or window resizes

### 6. CLI → GUI Bridge

```cpp
// Original CLI:
//   int x; cin >> x; cout << x * 2;

// GUI version:
EzUI::App app;
State<std::string> input_val("");
State<std::string> output_val("");

app.build([&]() {
    Window("CLI App", 400, 300);
    Input("Enter x:", input_val);
    Button("Compute", [&] {
        int x = std::stoi(input_val.get());
        output_val = std::to_string(x * 2);
    });
    Text("Result: " + output_val.get());
});
app.run();
```

Additionally, we provide a `ConsoleCapture` utility that redirects `cin`/`cout` to GUI input/output widgets automatically.

---

## User Review Required

> [!IMPORTANT]
> **SDL2_ttf dependency**: Your system has SDL2 installed but **not SDL2_ttf** (needed for text rendering). I will need to install `libsdl2-ttf-dev` via apt. Is that acceptable?

> [!IMPORTANT]
> **Library name**: I'm using **EzUI** as the namespace/library name. Let me know if you prefer a different name (e.g., `CppGUI`, `SimpleUI`, `QuickUI`).

> [!IMPORTANT]
> **Font**: I'll bundle **DejaVu Sans** (already on your system at `/usr/share/fonts/truetype/dejavu/`) as the default font. No external downloads needed.

---

## Proposed Changes

### Project Structure

```
CppGUI/
├── CMakeLists.txt                  # Build system
├── README.md                       # Documentation
├── include/
│   └── ezui/
│       ├── ezui.h                  # Single-header public API (includes everything)
│       ├── app.h                   # App class (runtime, event loop)
│       ├── components.h            # Window, Button, Text, Input
│       ├── layout.h                # VStack, HStack, layout engine
│       ├── state.h                 # State<T> reactive state
│       ├── style.h                 # Style struct, color parsing
│       ├── types.h                 # Rect, Color, Size, common types
│       └── console_bridge.h        # CLI → GUI bridge utilities
├── src/
│   ├── app.cpp                     # Event loop, renderer management
│   ├── components.cpp              # Component implementations
│   ├── layout.cpp                  # Layout computation
│   ├── state.cpp                   # State manager
│   ├── style.cpp                   # Color parsing, style defaults
│   └── console_bridge.cpp          # ConsoleCapture implementation
├── examples/
│   ├── counter/
│   │   └── main.cpp                # Counter app example
│   ├── cli_to_gui/
│   │   └── main.cpp                # CLI-to-GUI bridge example
│   └── hello/
│       └── main.cpp                # Minimal hello world
└── assets/
    └── fonts/                      # (symlink to system DejaVu Sans)
```

---

### Backend Layer — SDL2 Abstraction

#### [NEW] [types.h](file:///home/kairav-dutta/Desktop/projects/CppGUI/include/ezui/types.h)
- `Color` struct with hex string parsing (`"#1e1e1e"` → RGB)
- `Rect`, `Size`, `Point` structs
- `Padding` struct

#### [NEW] [style.h](file:///home/kairav-dutta/Desktop/projects/CppGUI/include/ezui/style.h)
- `Style` struct: background color, text color, font size, padding, border radius, hover color
- Default style presets (dark theme)
- `StyleBuilder` for chained configuration

#### [NEW] [style.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/src/style.cpp)
- Hex color parser implementation
- Default theme values

---

### Core Runtime Layer

#### [NEW] [state.h](file:///home/kairav-dutta/Desktop/projects/CppGUI/include/ezui/state.h)
- `State<T>` template class with:
  - `operator=` that triggers dirty notifications
  - `get()` / implicit conversion
  - Observer registration for components
- `StateManager` singleton tracking all active state variables

#### [NEW] [state.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/src/state.cpp)
- StateManager implementation

#### [NEW] [layout.h](file:///home/kairav-dutta/Desktop/projects/CppGUI/include/ezui/layout.h)
- `VStack` and `HStack` container components
- Layout computation algorithm (single-pass top-down)
- Spacing, alignment options

#### [NEW] [layout.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/src/layout.cpp)
- Layout algorithm: measure pass → arrange pass
- VStack: sum heights, max width
- HStack: sum widths, max height

#### [NEW] [app.h](file:///home/kairav-dutta/Desktop/projects/CppGUI/include/ezui/app.h)
- `App` class:
  - `build(std::function<void()> builder)` — takes the declarative UI description
  - `run()` — enters the hidden event loop
  - Internal: SDL_Window, SDL_Renderer, TTF_Font management
  - Event dispatch, hit testing, dirty-region rendering

#### [NEW] [app.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/src/app.cpp)
- SDL2 initialization and teardown
- Main event loop (60 FPS cap with `SDL_Delay`)
- Event routing: mouse clicks → button callbacks, key events → input fields
- Render loop: only redraw components with `dirty == true`
- Font loading from system path

---

### Declarative API Layer — Components

#### [NEW] [components.h](file:///home/kairav-dutta/Desktop/projects/CppGUI/include/ezui/components.h)
- Base `Component` class (abstract)
- `WindowComponent` — creates the SDL window, acts as root
- `ButtonComponent` — clickable with hover states, callback
- `TextComponent` — static text display
- `InputComponent` — editable text field with cursor

#### [NEW] [components.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/src/components.cpp)
- Rendering implementations using SDL2 renderer
- Rounded rectangles for buttons
- Text rendering via SDL2_ttf
- Input field: cursor blinking, text selection, key handling

---

### CLI → GUI Bridge

#### [NEW] [console_bridge.h](file:///home/kairav-dutta/Desktop/projects/CppGUI/include/ezui/console_bridge.h)
- `ConsoleCapture` class that wraps cin/cout
- `AutoGUI` builder that generates Input + Text widgets from cin/cout usage

#### [NEW] [console_bridge.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/src/console_bridge.cpp)
- Stream buffer redirection
- Auto-widget generation

---

### Public API Header

#### [NEW] [ezui.h](file:///home/kairav-dutta/Desktop/projects/CppGUI/include/ezui/ezui.h)
- Single include that pulls in everything
- Convenience free functions: `Window()`, `Button()`, `Text()`, `Input()`, `VStack()`, `HStack()`
- `print()` utility function

---

### Examples

#### [NEW] [hello/main.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/examples/hello/main.cpp)
```cpp
#include <ezui/ezui.h>
using namespace EzUI;

int main() {
    App app;
    app.build([]() {
        Window("Hello EzUI", 400, 300, "#1e1e1e");
        Text("Hello, World!", {.color = "#ffffff", .fontSize = 24});
    });
    app.run();
}
```

#### [NEW] [counter/main.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/examples/counter/main.cpp)
```cpp
#include <ezui/ezui.h>
using namespace EzUI;

int main() {
    App app;
    State<int> count(0);

    app.build([&]() {
        Window("Counter", 400, 300, "#1a1a2e");
        Text("Count: " + std::to_string(count.get()),
             {.color = "#e0e0e0", .fontSize = 28});
        HStack(10, [&]() {
            Button("−", [&] { count = count.get() - 1; },
                   {.bg = "#e74c3c", .color = "#fff"});
            Button("+", [&] { count = count.get() + 1; },
                   {.bg = "#2ecc71", .color = "#fff"});
        });
    });
    app.run();
}
```

#### [NEW] [cli_to_gui/main.cpp](file:///home/kairav-dutta/Desktop/projects/CppGUI/examples/cli_to_gui/main.cpp)
```cpp
#include <ezui/ezui.h>
using namespace EzUI;

// Original CLI: cin >> x; cout << x * 2;
int main() {
    App app;
    State<std::string> input_val("");
    State<std::string> result("");

    app.build([&]() {
        Window("CLI → GUI", 400, 300, "#16213e");
        Input("Enter a number:", input_val, {.bg = "#1a1a2e"});
        Button("Compute", [&] {
            int x = std::stoi(input_val.get());
            result = "Result: " + std::to_string(x * 2);
        }, {.bg = "#0f3460"});
        Text(result.get(), {.color = "#e0e0e0", .fontSize = 20});
    });
    app.run();
}
```

---

### Build System

#### [NEW] [CMakeLists.txt](file:///home/kairav-dutta/Desktop/projects/CppGUI/CMakeLists.txt)
- C++17 standard
- Find SDL2 and SDL2_ttf via `pkg-config`
- Build `libezui` as a static library
- Build example executables linking against `libezui`

---

## Open Questions

1. **Library name**: Happy with **EzUI** or prefer something else?
2. **SDL2_ttf install**: Can I run `sudo apt install libsdl2-ttf-dev` to get text rendering support?
3. **Additional components**: Want any other widgets in the initial version (e.g., Slider, Checkbox, Dropdown)?
4. **Grid layout**: Should I include grid layout in v1 or defer it?

## Verification Plan

### Automated Tests
1. `cmake --build . --target all` — compile the library + all examples
2. Run each example binary to verify no crashes
3. Validate that the counter app increments/decrements correctly

### Manual Verification
- Launch each example app and verify:
  - Window appears with correct title and background
  - Button hover/click effects work
  - Text renders with correct font and color
  - Input field accepts keyboard input
  - Counter state updates the display
  - CLI-to-GUI example computes correctly

## Implementation Order

| Step | Component | Estimated Complexity |
|------|-----------|---------------------|
| 1 | CMakeLists.txt + project scaffolding | Low |
| 2 | types.h, style.h/cpp (Color, Rect, Style) | Low |
| 3 | state.h/cpp (State\<T\>, observer pattern) | Medium |
| 4 | components.h/cpp (base Component, render) | High |
| 5 | layout.h/cpp (VStack, HStack) | Medium |
| 6 | app.h/cpp (event loop, SDL2 init, font loading) | High |
| 7 | ezui.h (convenience API, free functions) | Low |
| 8 | console_bridge.h/cpp (CLI→GUI) | Medium |
| 9 | Example apps (hello, counter, cli_to_gui) | Low |
| 10 | README.md | Low |

## Future Improvements

- **Animations**: Tween engine for smooth transitions
- **Theming**: Multiple built-in themes (dark, light, nord, solarized)
- **Custom fonts**: User-loadable font support
- **Image component**: Display images/textures
- **Scroll containers**: For overflow content
- **Grid layout**: CSS Grid-like system
- **Hot reload**: Watch source files and rebuild
- **OpenGL backend**: Alternative to SDL2 renderer for GPU-accelerated rendering
- **Accessibility**: Screen reader support via platform APIs
