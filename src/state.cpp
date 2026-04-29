#include "ezui/state.h"
#include "ezui/components.h"   // for Component::markDirty()

namespace EzUI {

static bool g_globalDirty = true;   // start dirty so first frame renders

void StateBase::notifyObservers() {
    std::lock_guard<std::mutex> lk(mu_);
    for (auto* c : observers_) {
        if (c) c->markDirty();
    }
}

bool anyStateDirty() { return g_globalDirty; }

void clearGlobalDirty() { g_globalDirty = false; }

void setGlobalDirty() { g_globalDirty = true; }

} // namespace EzUI
