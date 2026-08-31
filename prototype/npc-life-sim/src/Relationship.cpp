#include "Relationship.h"

namespace sim {

RelationshipType ClassifyRelationship(const Relationship& r) {
    if (r.affinity <= -25.0f && r.interactionCount >= 3) {
        return RelationshipType::Rival;
    }
    if (r.affinity >= 60.0f && r.trust >= 50.0f && r.interactionCount >= 10) {
        return RelationshipType::CloseFriend;
    }
    if (r.affinity >= 25.0f && r.interactionCount >= 3) {
        return RelationshipType::Friend;
    }
    if (r.interactionCount >= 1) {
        return RelationshipType::Acquaintance;
    }
    return RelationshipType::Stranger;
}

} // namespace sim
