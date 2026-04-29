# EzUI — Minimal, Reactive C++ GUI Library

Build desktop GUIs in C++ as easily as writing CLI programs.

EzUI is a lightweight, declarative GUI library built on SDL2. It removes boilerplate, hides the event loop, and provides a clean API with reactive state.

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

* **Declarative API** — `Window()`, `Button()`, `Text()`, `Input()`
* **Reactive State** — `State<T>` automatically updates UI
* **No Boilerplate** — No manual event loop or redraw logic
* **Efficient Rendering** — Dirty-flag updates (no full redraw)
* **Simple Layouts** — `VStack()` and `HStack()`
* **CLI → GUI Bridge** — Convert terminal-style apps into GUIs

---

## ⚡ Installation

### 1. Install dependencies

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev cmake build-essential
```

---

### 2. Build and install EzUI

```bash
git clone https://github.com/ka1rav6/ezgui.git
cd ezgui

mkdir build && cd build
cmake ..
cmake --build .

sudo cmake --install .
```

(Optional, if linker doesn’t detect it)

```bash
sudo ldconfig
```

---

## 🧪 Using EzUI in Your Project

### 📁 Project Structure

```
my_app/
├── CMakeLists.txt
└── main.cpp
```

---

### 📄 main.cpp

```cpp
#include <ezui/ezui.h>
using namespace EzUI;

int main() {
    App app;

    app.build([&]() {
        Window("Hello", 400, 300);
        Text("EzUI is working!");
    });

    app.run();
}
```

---

### ⚙️ CMakeLists.txt (Recommended)

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_CXX_STANDARD 20)

find_package(ezui REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp ezui::ezui)
```

---

### 🔨 Build your app

```bash
mkdir build && cd build
cmake ..
make
./myapp
```

---

## 🧩 Core API

### Window

```cpp
Window("Title", width, height, "#background_hex");
```

### Text

```cpp
Text("Hello World");
```

### Button

```cpp
Button("Click Me", []() {
    // callback
});
```

### Input

```cpp
State<std::string> name("");
Input("Enter name:", name);
```

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

### Reactive State

```cpp
State<int> count(0);

count = 5;           // triggers UI update
int value = count.get();
```

---

## 🔥 CLI → GUI Bridge

Turn CLI-style logic into a GUI:

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
Reactive State (dirty updates)
```

* Declarative frontend
* Retained component tree
* Observer-based state system
* Partial redraw for performance

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
* Rapid prototypes
* CLI-to-GUI conversions

---

## 📁 Project Structure

```
include/ezui/    # Public API
src/             # Implementation
examples/        # Sample apps
cmake/           # CMake config files
```

---

## 🚧 Status

Early-stage project (v0.x)

APIs may change.

---

## 🤝 Contributing

Contributions, issues, and suggestions are welcome.

---

## 📄 License

MIT
