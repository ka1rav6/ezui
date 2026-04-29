# EzUI — Minimal, Reactive C++ GUI Library

Build desktop GUIs in C++ as easily as writing CLI programs.

EzUI is a lightweight, declarative GUI library built on SDL2. It removes boilerplate, hides the event loop, and gives you a clean, intuitive API with reactive state.

---

## 🚀 1-Minute Example

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

No event loop. No rendering code. Just UI.

---

## ✨ Why EzUI?

* **Declarative API** — Just call `Window()`, `Button()`, `Text()`
* **Reactive State** — `State<T>` automatically updates UI
* **No Boilerplate** — No event loops, no manual redraws
* **Fast** — Dirty-flag rendering (no full redraw every frame)
* **Simple Layouts** — `VStack()` and `HStack()`
* **CLI → GUI Bridge** — Turn terminal apps into GUIs quickly

---

## ⚡ Installation

### Option 1: One-line install

```bash
curl -sSL https://raw.githubusercontent.com/ka1rav6/ezgui/main/install.sh | bash
```

---

### Option 2: Manual build

#### Requirements

* C++20 compiler (GCC 10+, Clang 12+)
* SDL2
* SDL2_ttf
* CMake 3.16+

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev cmake build-essential
```

#### Build & Install

```bash
git clone https://github.com/ka1rav6/ezgui.git
cd ezgui

mkdir build && cd build
cmake ..
cmake --build .

sudo cmake --install .
```

---

## 🔧 Using EzUI in Your Project

### CMake

```cmake
find_package(ezui REQUIRED)
target_link_libraries(your_app ezui)
```

---

## 🧩 Core Concepts

### Window

```cpp
Window("Title", width, height, "#background_hex");
```

---

### Text

```cpp
Text("Hello World");
```

---

### Button

```cpp
Button("Click Me", []() {
    // callback
});
```

---

### Layouts

```cpp
VStack(10, []() {
    Text("Top");
    Text("Bottom");
});

HStack(15, []() {
    Button("A", handler);
    Button("B", handler);
});
```

---

### Reactive State

```cpp
State<int> count(0);

count = 10;           // triggers UI update
int value = count.get();
```

---

## 🔥 CLI → GUI Bridge (Unique Feature)

Convert CLI-style logic into a GUI with minimal changes:

```cpp
ConsoleCapture cli("App", 500, 400);

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

## 🏗 Architecture

```
User Code
   ↓
Declarative API
   ↓
Component Tree
   ↓
Renderer (SDL2)
   ↑
Reactive State (dirty-flag updates)
```

* Declarative frontend
* Retained component tree
* Observer-based state system
* Partial redraw (efficient)

---

## 📁 Project Structure

```
include/ezui/    # Public API
src/             # Implementation
examples/        # Sample apps
install.sh       # One-line installer
```

---

## 📊 Comparison

| Feature            | EzUI | Qt   | SDL2   |
| ------------------ | ---- | ---- | ------ |
| Boilerplate        | Low  | High | Medium |
| Event loop exposed | No   | No   | Yes    |
| Reactive state     | Yes  | No   | No     |
| Learning curve     | Low  | High | Medium |

---

## 🎯 Use Cases

* Developer tools
* Internal apps
* Prototypes
* CLI-to-GUI conversions

---

## 🚧 Status

Early-stage project (v0.x)

APIs may change as the library evolves.

---

## 🤝 Contributing

Contributions are welcome.
Feel free to open issues or submit PRs.

---

## 📄 License

MIT
