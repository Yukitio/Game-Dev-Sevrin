#pragma once

#include <functional>
#include <string>

#include "GameTime.h"

namespace sim {

class Simulation;

struct CalendarEvent {
    std::string id;
    std::string name;
    int triggerDay = 1;
    int triggerHour = 12;
    int recurEveryDays = 0;
    std::function<void(Simulation&)> effect;

    bool IsDue(const GameTime& now) const {
        if (now.hour != triggerHour) return false;
        if (recurEveryDays > 0) {
            return now.day >= triggerDay && (now.day - triggerDay) % recurEveryDays == 0;
        }
        return now.day == triggerDay;
    }
};

} // namespace sim
