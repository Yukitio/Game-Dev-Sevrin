#include "GameTime.h"

#include <cstdio>

namespace sim {

std::string GameTime::ToString() const {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "Day %2d, %02d:00", day, hour);
    return buf;
}

} // namespace sim
