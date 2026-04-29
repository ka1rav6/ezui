#pragma once

#include <algorithm>
#include <functional>
#include <mutex>
#include <string>
#include <type_traits>
#include <vector>

namespace EzUI {

// Forward declaration — Component registers itself as an observer
class Component;

/// ── StateBase ───────────────────────────────────────────
/// Non-templated base so we can store heterogeneous state in a manager.
class StateBase {
public:
    virtual ~StateBase() = default;

    void addObserver(Component* c) {
        std::lock_guard<std::mutex> lk(mu_);
        observers_.push_back(c);
    }

    void removeObserver(Component* c) {
        std::lock_guard<std::mutex> lk(mu_);
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), c),
            observers_.end());
    }

protected:
    void notifyObservers();                       // defined in state.cpp

    std::mutex mu_;
    std::vector<Component*> observers_;
};

/// ── State<T> ────────────────────────────────────────────
/// A reactive variable.  Assigning a new value marks all
/// observing components as dirty so they get re-rendered.
///
///   State<int> count(0);
///   count = 5;          // triggers redraw on watchers
///   int v = count.get();
///
template <typename T>
class State : public StateBase {
public:
    explicit State(T initial = T{}) : value_(std::move(initial)) {}

    /// Read the current value.
    [[nodiscard]] const T& get() const { return value_; }

    /// Implicit conversion for convenience.
    operator const T&() const { return value_; }  // NOLINT

    /// Assign a new value and notify.
    State& operator=(const T& v) {
        // Skip update if value is unchanged (works for any == comparable type)
        if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, std::string>) {
            if (value_ == v) return *this;
        }
        value_ = v;
        notifyObservers();
        globalDirty_ = true;
        return *this;
    }

    /// Move-assign.
    State& operator=(T&& v) {
        value_ = std::move(v);
        notifyObservers();
        globalDirty_ = true;
        return *this;
    }

    /// Check and clear the global dirty flag (used by the render loop).
    static bool consumeGlobalDirty() {
        bool d = globalDirty_;
        globalDirty_ = false;
        return d;
    }

    static void markGlobalDirty() { globalDirty_ = true; }

private:
    T value_;
    static inline bool globalDirty_ = false;
};

/// Global dirty flag across ALL state types (checked per frame).
bool anyStateDirty();
void clearGlobalDirty();
void setGlobalDirty();

} // namespace EzUI
