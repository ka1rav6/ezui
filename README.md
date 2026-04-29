# EzUI — Modern Minimal C++ GUI Library

A high-performance, intuitive C++ GUI library built on SDL2. Write GUI code as naturally as writing a CLI app.

## Quick Start

```cpp
#include <ezui/ezui.h>
using namespace EzUI;

int main() {
    App app;
    State<int> count(0);

    app.build([&]() {
        Window("Counter", 500, 350, "#1a1a2e");
        Text("Count: " + std::to_string(count.get()));
        HStack(15, [&]() {
            Button(" - ", [&] { count = count.get() - 1; });
            Button(" + ", [&] { count = count.get() + 1; });
        });
    });

    app.run();
}
```

That's it. No boilerplate, no event loop, no manual rendering.

---

## Features

| Feature | Description |
|---|---|
| **Declarative API** | `Window()`, `Button()`, `Text()`, `Input()` — just call functions |
| **Reactive State** | `State<T>` auto-triggers UI updates on assignment |
| **Hidden Event Loop** | `app.run()` handles everything |
| **Layout System** | `VStack()` and `HStack()` with automatic centering |
| **Styling** | Hex colors, padding, border radius, hover effects |
| **CLI → GUI Bridge** | Convert `cin`/`cout` programs to GUI with `ConsoleCapture` |
| **High Performance** | Dirty-flag rendering — no full redraw every frame |

---

## Building

### Prerequisites

- **C++20** compiler (GCC 10+, Clang 12+)
- **SDL2** (`libsdl2-dev`)
- **SDL2_ttf** (`libsdl2-ttf-dev`)
- **CMake** 3.16+

```bash
# Ubuntu/Debian
sudo apt install libsdl2-dev libsdl2-ttf-dev cmake build-essential
```

### Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Run Examples

```bash
./hello_ezui      # Hello World
./counter_ezui    # Counter with +/- buttons
./cli2gui_ezui    # CLI-to-GUI bridge demo
```

---

## API Reference

### Window
```cpp
Window("Title", width, height, "#background_hex");
```

### Text
```cpp
Text("Hello!", {.color = Color("#ffffff"), .fontSize = 24});
```

### Button
```cpp
Button("Click Me", []() {
    // callback
}, {.bg = Color("#6c63ff"), .color = Color("#ffffff")});
```

### Input
```cpp
State<std::string> name("");
Input("Enter name:", name, {.bg = Color("#2d2d2d")});
```

### Layout
```cpp
VStack(10, []() {   // vertical stack, 10px spacing
    Text("Top");
    Text("Bottom");
});

HStack(15, []() {   // horizontal stack, 15px spacing
    Button("A", handler);
    Button("B", handler);
});
```

### State
```cpp
State<int> count(0);
count = 5;              // triggers UI update
int v = count.get();    // read value
```

### CLI → GUI Bridge
```cpp
ConsoleCapture cli("App Title", 500, 400);
cli.addPrompt("Enter x:");
cli.addPrompt("Enter y:");
cli.onSubmit([&](const std::vector<std::string>& inputs) {
    int x = std::stoi(inputs[0]);
    int y = std::stoi(inputs[1]);
    cli.setOutput("Sum = " + std::to_string(x + y));
});
cli.run();
```

---

## Architecture

```
User Code  →  Declarative API  →  Component Tree  →  SDL2 Renderer
                                       ↑
                State<T> ──── dirty flag ──── partial redraw
```

- **Hybrid retained + declarative**: You write declarative code, EzUI builds a retained component tree
- **Observer pattern**: `State<T>` notifies dependent components on value change
- **Dirty-flag rendering**: Only dirty components are re-rendered each frame

---

## Project Structure

```
CppGUI/
├── CMakeLists.txt
├── include/ezui/
│   ├── ezui.h              # Single-include public API
│   ├── app.h               # App runtime (hidden event loop)
│   ├── components.h         # Window, Button, Text, Input
│   ├── layout.h            # VStack, HStack
│   ├── state.h             # Reactive State<T>
│   ├── style.h             # Styling + dark theme
│   ├── types.h             # Color, Rect, Point, Padding
│   └── console_bridge.h    # CLI → GUI bridge
├── src/                     # Implementations
└── examples/                # Working demos
```

---

## License

MIT
