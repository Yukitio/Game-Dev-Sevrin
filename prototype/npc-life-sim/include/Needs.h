#pragma once

#include <array>

#include "Types.h"

namespace sim {

constexpr float kNeedMax = 100.0f;
constexpr float kNeedMin = 0.0f;
constexpr float kNeedCriticalThreshold = 20.0f;

class Needs {
public:
    Needs();

    float Get(NeedType need) const;
    void Set(NeedType need, float value);
    void Adjust(NeedType need, float delta);

    NeedType MostUrgent() const;

private:
    std::array<float, static_cast<size_t>(NeedType::COUNT)> values_;
};

} // namespace sim
