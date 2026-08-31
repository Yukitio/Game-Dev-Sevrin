#pragma once

#include <string>

namespace sim {

constexpr int kHoursPerDay = 24;

struct GameTime {
    int day = 1;
    int hour = 0;

    std::string ToString() const;
    bool operator==(const GameTime& other) const {
        return day == other.day && hour == other.hour;
    }
};

} // namespace sim
