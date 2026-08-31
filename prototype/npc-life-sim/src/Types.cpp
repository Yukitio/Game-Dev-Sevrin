#include "Types.h"

namespace sim {

std::string ToString(ActivityType activity) {
    switch (activity) {
        case ActivityType::Sleep: return "Sleeping";
        case ActivityType::Work: return "Working";
        case ActivityType::Eat: return "Eating";
        case ActivityType::Socialize: return "Socializing";
        case ActivityType::Leisure: return "Relaxing";
        case ActivityType::Idle: return "Idle";
    }
    return "Unknown";
}

std::string ToString(NeedType need) {
    switch (need) {
        case NeedType::Hunger: return "Hunger";
        case NeedType::Energy: return "Energy";
        case NeedType::Social: return "Social";
        case NeedType::Money: return "Money";
        case NeedType::COUNT: return "Unknown";
    }
    return "Unknown";
}

std::string ToString(GoalType goal) {
    switch (goal) {
        case GoalType::CareerGrowth: return "Career Growth";
        case GoalType::Mastery: return "Mastery";
        case GoalType::Wealth: return "Wealth";
        case GoalType::Friendship: return "Friendship";
        case GoalType::Romance: return "Romance";
    }
    return "Unknown";
}

std::string ToString(RelationshipType relationship) {
    switch (relationship) {
        case RelationshipType::Stranger: return "Stranger";
        case RelationshipType::Acquaintance: return "Acquaintance";
        case RelationshipType::Friend: return "Friend";
        case RelationshipType::CloseFriend: return "Close Friend";
        case RelationshipType::Rival: return "Rival";
        case RelationshipType::Romantic: return "Romantic Partner";
    }
    return "Unknown";
}

} // namespace sim
