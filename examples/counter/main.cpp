#include <ezui/ezui.h>
#include <string>
using namespace EzUI;

int main() {
    App app;
    State<int> count(0);

    app.build([&]() {
        Window("Counter", 500, 350, "#1a1a2e");

        Text("Counter App",
             {.color = Color("#ffffff"), .fontSize = 28});

        Text("Count: " + std::to_string(count.get()),
             {.color = Color("#e0e0e0"), .fontSize = 22});

        HStack(15, [&]() {
            Style minus;
            minus.bg      = Color("#e74c3c");
            minus.color   = Color("#ffffff");
            minus.padding = Padding(10, 24);

            Style plus;
            plus.bg      = Color("#2ecc71");
            plus.color   = Color("#ffffff");
            plus.padding = Padding(10, 24);

            Style reset;
            reset.bg      = Color("#3498db");
            reset.color   = Color("#ffffff");
            reset.padding = Padding(10, 24);

            Button(" - ", [&] { count = count.get() - 1; }, minus);
            Button(" + ", [&] { count = count.get() + 1; }, plus);
            Button("Reset", [&] { count = 0; }, reset);
        });
    });

    app.run();
    return 0;
}
