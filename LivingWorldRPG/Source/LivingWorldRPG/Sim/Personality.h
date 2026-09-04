#pragma once

namespace sim {

struct Personality {
    float warmth = 0.0f;
    float ambition = 0.0f;
    float temperament = 0.0f;
};

float Compatibility(const Personality& a, const Personality& b);

} // namespace sim
