#include "Schedule.h"

#include <stdexcept>

namespace sim {

void Schedule::AddBlock(ScheduleBlock block) {
    blocks_.push_back(std::move(block));
}

const ScheduleBlock& Schedule::BlockAt(int hour) const {
    for (const auto& block : blocks_) {
        if (block.Contains(hour)) {
            return block;
        }
    }
    throw std::runtime_error("No schedule block covers hour " + std::to_string(hour));
}

} // namespace sim
