#pragma once

#include "app.h"
#include "state.h"
#include "components.h"

#include <string>
#include <vector>
#include <sstream>
#include <functional>

namespace EzUI {

/// ── ConsoleCapture ─────────────────────────────────────
/// Bridges a CLI-style workflow into a GUI.
///
/// Usage:
///   ConsoleCapture cli(app);
///   cli.addPrompt("Enter x:");           // becomes an Input widget
///   cli.addPrompt("Enter y:");
///   cli.onSubmit([&](const std::vector<std::string>& inputs) {
///       int x = std::stoi(inputs[0]);
///       int y = std::stoi(inputs[1]);
///       cli.setOutput("Sum = " + std::to_string(x + y));
///   });
///   cli.run();
///
class ConsoleCapture {
public:
    explicit ConsoleCapture(const std::string& title = "CLI App",
                            int width = 500, int height = 400,
                            const std::string& bg = "#16213e");

    /// Add an input prompt (equivalent to cin >> var).
    void addPrompt(const std::string& label);

    /// Set the callback for when the user submits all inputs.
    void onSubmit(std::function<void(const std::vector<std::string>&)> callback);

    /// Set output text (equivalent to cout << ...).
    void setOutput(const std::string& text);

    /// Build and run the GUI.
    void run();

private:
    std::string title_;
    int width_, height_;
    std::string bg_;

    struct Prompt {
        std::string label;
        State<std::string> value{""};
    };

    std::vector<std::unique_ptr<Prompt>> prompts_;
    State<std::string> outputText_{""};
    std::function<void(const std::vector<std::string>&)> submitCallback_;
};

} // namespace EzUI
