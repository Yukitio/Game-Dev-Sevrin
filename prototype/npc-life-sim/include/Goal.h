#pragma once

#include <string>

#include "Types.h"

namespace sim {

constexpr float kGoalComplete = 100.0f;

struct Goal {
    GoalType type;
    std::string targetNpcId;
    float progress = 0.0f;
    bool completed = false;
    bool announced = false;
};

} // namespace sim
