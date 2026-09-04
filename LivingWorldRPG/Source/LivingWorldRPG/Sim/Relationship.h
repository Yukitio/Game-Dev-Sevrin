#pragma once

#include "Types.h"

namespace sim {

struct Relationship {
    float affinity = 0.0f;
    float trust = 0.0f;
    int interactionCount = 0;
    RelationshipType type = RelationshipType::Stranger;
};

RelationshipType ClassifyRelationship(const Relationship& r);

} // namespace sim
