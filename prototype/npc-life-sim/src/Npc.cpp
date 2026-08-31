#include "Npc.h"

namespace sim {

Relationship& Npc::RelationshipWith(const std::string& otherId) {
    return relationships[otherId];
}

} // namespace sim
