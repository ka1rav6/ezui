#include <ezui/ezui.h>
using namespace EzUI;

int main() {
    App app;

    app.build([]() {
        Window("Hello EzUI", 500, 350, "#1a1a2e");

        Text("Welcome to EzUI!", {.color = Color("#ffffff"), .fontSize = 28});
        Text("A modern, minimal C++ GUI library.",
             {.color = Color("#a0a0a0"), .fontSize = 16});
    });

    app.run();
    return 0;
}
