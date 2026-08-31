#include "Personality.h"

#include <cmath>

namespace sim {

float Compatibility(const Personality& a, const Personality& b) {
    float warmthMatch = 1.0f - std::fabs(a.warmth - b.warmth) / 2.0f;
    float temperamentClash = std::fabs(a.temperament - b.temperament) / 2.0f;
    return warmthMatch - temperamentClash;
}

} // namespace sim
