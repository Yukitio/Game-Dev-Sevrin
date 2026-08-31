#include "Needs.h"

#include <algorithm>

namespace sim {

Needs::Needs() {
    values_.fill(75.0f);
}

float Needs::Get(NeedType need) const {
    return values_[static_cast<size_t>(need)];
}

void Needs::Set(NeedType need, float value) {
    values_[static_cast<size_t>(need)] = std::clamp(value, kNeedMin, kNeedMax);
}

void Needs::Adjust(NeedType need, float delta) {
    Set(need, Get(need) + delta);
}

NeedType Needs::MostUrgent() const {
    size_t worst = 0;
    for (size_t i = 1; i < values_.size(); ++i) {
        if (values_[i] < values_[worst]) {
            worst = i;
        }
    }
    return static_cast<NeedType>(worst);
}

} // namespace sim
