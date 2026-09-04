#pragma once

#include <string>
#include <vector>

#include "Types.h"

namespace sim {

struct ScheduleBlock {
    int startHour;
    int endHour;
    ActivityType activity;
    std::string locationId;

    bool Contains(int hour) const {
        if (startHour <= endHour) {
            return hour >= startHour && hour < endHour;
        }
        return hour >= startHour || hour < endHour;
    }
};

class Schedule {
public:
    void AddBlock(ScheduleBlock block);
    const ScheduleBlock& BlockAt(int hour) const;

private:
    std::vector<ScheduleBlock> blocks_;
};

} // namespace sim
