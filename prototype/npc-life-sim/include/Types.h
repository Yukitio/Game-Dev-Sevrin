#pragma once

#include <string>

namespace sim {

enum class NeedType { Hunger, Energy, Social, Money, COUNT };

enum class ActivityType { Sleep, Work, Eat, Socialize, Leisure, Idle };

enum class GoalType { CareerGrowth, Mastery, Wealth, Friendship, Romance };

enum class RelationshipType {
    Stranger,
    Acquaintance,
    Friend,
    CloseFriend,
    Rival,
    Romantic
};

std::string ToString(ActivityType activity);
std::string ToString(NeedType need);
std::string ToString(GoalType goal);
std::string ToString(RelationshipType relationship);

} // namespace sim
