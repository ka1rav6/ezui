#include <ezui/ezui.h>
#include <string>
using namespace EzUI;

// ───────────────────────────────────────────────────────
// Original CLI version:
//
//   int main() {
//       int x;
//       std::cout << "Enter a number: ";
//       std::cin >> x;
//       std::cout << "Double: " << x * 2 << std::endl;
//   }
//
// GUI version using ConsoleCapture:
// ───────────────────────────────────────────────────────

int main() {
    ConsoleCapture cli("Number Doubler", 500, 400, "#16213e");

    cli.addPrompt("Enter a number:");

    cli.onSubmit([&](const std::vector<std::string>& inputs) {
        try {
            int x = std::stoi(inputs[0]);
            cli.setOutput("Double: " + std::to_string(x * 2));
        } catch (...) {
            cli.setOutput("Error: Please enter a valid integer.");
        }
    });

    cli.run();
    return 0;
}
