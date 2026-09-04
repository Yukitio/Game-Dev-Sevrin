#pragma once

#include <map>
#include <string>
#include <vector>

#include "Goal.h"
#include "Needs.h"
#include "Personality.h"
#include "Relationship.h"
#include "Schedule.h"
#include "Types.h"

namespace sim {

class Npc {
public:
    std::string id;
    std::string name;
    std::string occupation;
    std::string homeLocationId;

    Personality personality;
    Needs needs;
    Schedule schedule;
    std::vector<Goal> goals;
    std::map<std::string, Relationship> relationships;

    ActivityType currentActivity = ActivityType::Idle;
    std::string currentLocationId;

    Relationship& RelationshipWith(const std::string& otherId);
};

} // namespace sim
