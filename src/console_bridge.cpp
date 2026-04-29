#include "ezui/console_bridge.h"
#include "ezui/ezui.h"

namespace EzUI {

ConsoleCapture::ConsoleCapture(const std::string& title, int width,
                               int height, const std::string& bg)
    : title_(title), width_(width), height_(height), bg_(bg) {}

void ConsoleCapture::addPrompt(const std::string& label) {
    prompts_.push_back(std::make_unique<Prompt>());
    prompts_.back()->label = label;
}

void ConsoleCapture::onSubmit(
    std::function<void(const std::vector<std::string>&)> callback) {
    submitCallback_ = std::move(callback);
}

void ConsoleCapture::setOutput(const std::string& text) {
    outputText_ = text;
}

void ConsoleCapture::run() {
    App app;

    auto* self = this;

    app.build([self]() {
        Window(self->title_, self->width_, self->height_, self->bg_);

        for (auto& prompt : self->prompts_) {
            Text(prompt->label, {.color = Color("#a0a0a0"), .fontSize = 14});
            Input("Type here...", prompt->value,
                  {.bg = Color("#1a1a2e"), .color = Color("#e0e0e0")});
        }

        Style btnStyle;
        btnStyle.bg    = Theme::primary();
        btnStyle.color = Color("#ffffff");
        btnStyle.padding = Padding(10, 30);

        Button("Submit", [self]() {
            if (self->submitCallback_) {
                std::vector<std::string> vals;
                vals.reserve(self->prompts_.size());
                for (auto& p : self->prompts_) {
                    vals.push_back(p->value.get());
                }
                self->submitCallback_(vals);
            }
        }, btnStyle);

        std::string out = self->outputText_.get();
        if (!out.empty()) {
            Text(out, {.color = Color("#2ecc71"), .fontSize = 18});
        }
    });

    app.run();
}

} // namespace EzUI
