#include "Simulation.h"

#include <algorithm>
#include <iostream>

namespace sim {

namespace {

constexpr float kHungerDecay = 3.0f;
constexpr float kEnergyDecay = 2.5f;
constexpr float kSocialDecay = 1.5f;
constexpr float kMoneyDecay = 0.5f;

float InteractionChance(ActivityType activity) {
    switch (activity) {
        case ActivityType::Socialize: return 0.9f;
        case ActivityType::Leisure: return 0.6f;
        case ActivityType::Eat: return 0.5f;
        case ActivityType::Work: return 0.3f;
        default: return 0.0f;
    }
}

} // namespace

Simulation::Simulation(unsigned seed) : rng_(seed) {}

Npc& Simulation::AddNpc(Npc npc) {
    npcs_.push_back(std::move(npc));
    return npcs_.back();
}

void Simulation::AddLocation(Location location) {
    locations_[location.id] = std::move(location);
}

void Simulation::AddEvent(CalendarEvent event) {
    events_.push_back(std::move(event));
}

Npc* Simulation::FindNpc(const std::string& id) {
    for (auto& npc : npcs_) {
        if (npc.id == id) return &npc;
    }
    return nullptr;
}

std::string Simulation::LocationName(const std::string& id) const {
    auto it = locations_.find(id);
    return it != locations_.end() ? it->second.name : id;
}

void Simulation::Log(const std::string& message) {
    eventLog_.push_back("[" + clock_.ToString() + "] " + message);
}

void Simulation::Announce(const std::string& message) {
    Log(message);
}

void Simulation::UpdateNeedsAndActivity(Npc& npc) {
    npc.needs.Adjust(NeedType::Hunger, -kHungerDecay);
    npc.needs.Adjust(NeedType::Energy, -kEnergyDecay);
    npc.needs.Adjust(NeedType::Social, -kSocialDecay);
    npc.needs.Adjust(NeedType::Money, -kMoneyDecay);

    const ScheduleBlock& block = npc.schedule.BlockAt(clock_.hour);
    ActivityType activity = block.activity;
    std::string locationId = block.locationId;

    NeedType urgent = npc.needs.MostUrgent();
    if (npc.needs.Get(urgent) < kNeedCriticalThreshold && activity != ActivityType::Sleep) {
        if (urgent == NeedType::Hunger && activity != ActivityType::Eat) {
            activity = ActivityType::Eat;
            locationId = npc.homeLocationId;
        } else if (urgent == NeedType::Energy && npc.needs.Get(NeedType::Energy) < 10.0f) {
            activity = ActivityType::Sleep;
            locationId = npc.homeLocationId;
        }
    }

    npc.currentActivity = activity;
    npc.currentLocationId = locationId;
}

void Simulation::ApplyActivityEffects(Npc& npc) {
    switch (npc.currentActivity) {
        case ActivityType::Sleep:
            npc.needs.Adjust(NeedType::Energy, 15.0f);
            break;
        case ActivityType::Eat:
            npc.needs.Adjust(NeedType::Hunger, 30.0f);
            break;
        case ActivityType::Work: {
            npc.needs.Adjust(NeedType::Money, 8.0f);
            for (auto& goal : npc.goals) {
                if (goal.completed) continue;
                if (goal.type == GoalType::CareerGrowth || goal.type == GoalType::Mastery) {
                    goal.progress += 0.35f + npc.personality.ambition * 0.3f;
                } else if (goal.type == GoalType::Wealth) {
                    goal.progress += 0.45f;
                }
            }
            break;
        }
        case ActivityType::Socialize:
            npc.needs.Adjust(NeedType::Social, 10.0f);
            break;
        case ActivityType::Leisure:
            npc.needs.Adjust(NeedType::Energy, 5.0f);
            npc.needs.Adjust(NeedType::Social, 5.0f);
            break;
        case ActivityType::Idle:
            break;
    }
}

void Simulation::ProgressGoal(Npc& npc, const Npc& other, float interactionDelta) {
    if (interactionDelta <= 0.0f) return;
    for (auto& goal : npc.goals) {
        if (goal.completed || goal.targetNpcId != other.id) continue;
        if (goal.type == GoalType::Friendship || goal.type == GoalType::Romance) {
            goal.progress += interactionDelta * 2.0f;
            if (goal.progress >= kGoalComplete && !goal.completed) {
                goal.completed = true;
                goal.progress = kGoalComplete;
            }
        }
    }
}

void Simulation::ResolvePair(Npc& a, Npc& b) {
    auto pairKey = a.id < b.id ? std::make_pair(a.id, b.id) : std::make_pair(b.id, a.id);
    if (interactedToday_.count(pairKey)) return;

    float chance = std::min(InteractionChance(a.currentActivity), InteractionChance(b.currentActivity));
    if (chance <= 0.0f) return;

    std::uniform_real_distribution<float> roll(0.0f, 1.0f);
    if (roll(rng_) > chance) return;

    interactedToday_.insert(pairKey);

    float compatibility = Compatibility(a.personality, b.personality);
    std::uniform_real_distribution<float> noise(-1.5f, 1.5f);
    float delta = compatibility * 6.0f + noise(rng_);

    Relationship& ab = a.RelationshipWith(b.id);
    Relationship& ba = b.RelationshipWith(a.id);

    ab.affinity = std::clamp(ab.affinity + delta, -100.0f, 100.0f);
    ba.affinity = std::clamp(ba.affinity + delta, -100.0f, 100.0f);
    ab.trust = std::clamp(ab.trust + std::max(delta * 0.3f, 0.0f), 0.0f, 100.0f);
    ba.trust = std::clamp(ba.trust + std::max(delta * 0.3f, 0.0f), 0.0f, 100.0f);
    ab.interactionCount++;
    ba.interactionCount++;

    a.needs.Adjust(NeedType::Social, 4.0f);
    b.needs.Adjust(NeedType::Social, 4.0f);

    RelationshipType before = ab.type;
    if (before != RelationshipType::Romantic) {
        RelationshipType classified = ClassifyRelationship(ab);
        if (classified != before) {
            ab.type = classified;
            ba.type = classified;
            Log(a.name + " and " + b.name + " are now " + ToString(classified) + ".");
        }
    }

    ProgressGoal(a, b, delta);
    ProgressGoal(b, a, delta);

    for (auto& goal : a.goals) {
        if (!goal.completed || goal.announced || goal.targetNpcId != b.id) continue;
        goal.announced = true;
        if (goal.type == GoalType::Romance) {
            ab.type = RelationshipType::Romantic;
            ba.type = RelationshipType::Romantic;
            Log(a.name + "'s feelings for " + b.name + " are now returned. They are Romantic Partners.");
        } else if (goal.type == GoalType::Friendship) {
            Log(a.name + " has achieved a lasting friendship with " + b.name + ".");
        }
    }
    for (auto& goal : b.goals) {
        if (!goal.completed || goal.announced || goal.targetNpcId != a.id) continue;
        goal.announced = true;
        if (goal.type == GoalType::Romance) {
            ab.type = RelationshipType::Romantic;
            ba.type = RelationshipType::Romantic;
            Log(b.name + "'s feelings for " + a.name + " are now returned. They are Romantic Partners.");
        } else if (goal.type == GoalType::Friendship) {
            Log(b.name + " has achieved a lasting friendship with " + a.name + ".");
        }
    }
}

void Simulation::ResolveInteractions() {
    for (size_t i = 0; i < npcs_.size(); ++i) {
        for (size_t j = i + 1; j < npcs_.size(); ++j) {
            Npc& a = npcs_[i];
            Npc& b = npcs_[j];
            if (a.currentLocationId.empty() || a.currentLocationId != b.currentLocationId) continue;
            ResolvePair(a, b);
        }
    }
}

void Simulation::ResolveCalendarEvents() {
    for (auto& event : events_) {
        if (event.effect && event.IsDue(clock_)) {
            event.effect(*this);
        }
    }
}

std::string Simulation::Intervene_Encourage(const std::string& aId, const std::string& bId) {
    Npc* a = FindNpc(aId);
    Npc* b = FindNpc(bId);
    if (!a || !b) return "No such NPCs.";

    Relationship& ab = a->RelationshipWith(b->id);
    Relationship& ba = b->RelationshipWith(a->id);
    constexpr float kDelta = 15.0f;
    ab.affinity = std::clamp(ab.affinity + kDelta, -100.0f, 100.0f);
    ba.affinity = std::clamp(ba.affinity + kDelta, -100.0f, 100.0f);
    ab.trust = std::clamp(ab.trust + kDelta * 0.4f, 0.0f, 100.0f);
    ba.trust = std::clamp(ba.trust + kDelta * 0.4f, 0.0f, 100.0f);
    ab.interactionCount++;
    ba.interactionCount++;

    if (ab.type != RelationshipType::Romantic) {
        RelationshipType classified = ClassifyRelationship(ab);
        ab.type = classified;
        ba.type = classified;
    }

    ProgressGoal(*a, *b, kDelta);
    ProgressGoal(*b, *a, kDelta);

    std::string msg = "[PLAYER] You spend time bringing " + a->name + " and " + b->name + " closer together.";
    Log(msg);
    return msg;
}

std::string Simulation::Intervene_Discourage(const std::string& aId, const std::string& bId) {
    Npc* a = FindNpc(aId);
    Npc* b = FindNpc(bId);
    if (!a || !b) return "No such NPCs.";

    Relationship& ab = a->RelationshipWith(b->id);
    Relationship& ba = b->RelationshipWith(a->id);
    constexpr float kDelta = -20.0f;
    ab.affinity = std::clamp(ab.affinity + kDelta, -100.0f, 100.0f);
    ba.affinity = std::clamp(ba.affinity + kDelta, -100.0f, 100.0f);
    ab.trust = std::clamp(ab.trust + kDelta * 0.5f, 0.0f, 100.0f);
    ba.trust = std::clamp(ba.trust + kDelta * 0.5f, 0.0f, 100.0f);

    RelationshipType before = ab.type;
    RelationshipType classified = before;
    if (before == RelationshipType::Romantic) {
        if (ab.affinity < 50.0f) classified = RelationshipType::Friend;
    } else {
        classified = ClassifyRelationship(ab);
    }
    ab.type = classified;
    ba.type = classified;

    for (auto* npc : {a, b}) {
        const Npc* other = npc == a ? b : a;
        for (auto& goal : npc->goals) {
            if (goal.completed || goal.targetNpcId != other->id) continue;
            if (goal.type == GoalType::Friendship || goal.type == GoalType::Romance) {
                goal.progress = std::max(0.0f, goal.progress - 25.0f);
            }
        }
    }

    std::string msg = "[PLAYER] You sow doubt between " + a->name + " and " + b->name + ".";
    Log(msg);
    if (before == RelationshipType::Romantic && classified != RelationshipType::Romantic) {
        Log(a->name + " and " + b->name + "'s romance falls apart.");
    }
    return msg;
}

std::string Simulation::Intervene_Assist(const std::string& npcId, NeedType need, float amount) {
    Npc* npc = FindNpc(npcId);
    if (!npc) return "No such NPC.";

    npc->needs.Adjust(need, amount);
    std::string msg = "[PLAYER] You help " + npc->name + " with their " + ToString(need) + ".";
    Log(msg);
    return msg;
}

void Simulation::Tick() {
    if (clock_.hour == 0) {
        interactedToday_.clear();
    }

    for (auto& npc : npcs_) {
        UpdateNeedsAndActivity(npc);
        ApplyActivityEffects(npc);
    }
    ResolveInteractions();

    for (auto& npc : npcs_) {
        for (auto& goal : npc.goals) {
            if (goal.completed || goal.announced) continue;
            if (goal.progress >= kGoalComplete &&
                (goal.type == GoalType::CareerGrowth || goal.type == GoalType::Mastery ||
                 goal.type == GoalType::Wealth)) {
                goal.completed = true;
                goal.announced = true;
                Log(npc.name + " has achieved their goal: " + ToString(goal.type) + "!");
            }
        }
    }

    ResolveCalendarEvents();

    clock_.hour++;
    if (clock_.hour >= kHoursPerDay) {
        clock_.hour = 0;
        clock_.day++;
    }
}

void Simulation::RunDays(int days) {
    int totalTicks = days * kHoursPerDay;
    for (int i = 0; i < totalTicks; ++i) {
        Tick();
    }
}

void Simulation::PrintFinalReport(std::ostream& out) const {
    out << "\n=== Final Report after " << clock_.day - 1 << " days ===\n\n";
    for (const auto& npc : npcs_) {
        out << npc.name << " (" << npc.occupation << ")\n";
        out << "  Needs: Hunger=" << npc.needs.Get(NeedType::Hunger)
            << " Energy=" << npc.needs.Get(NeedType::Energy)
            << " Social=" << npc.needs.Get(NeedType::Social)
            << " Money=" << npc.needs.Get(NeedType::Money) << "\n";
        for (const auto& goal : npc.goals) {
            out << "  Goal [" << ToString(goal.type) << "]: "
                << (goal.completed ? "COMPLETE" : "in progress")
                << " (" << goal.progress << "/100)\n";
        }
        for (const auto& [otherId, rel] : npc.relationships) {
            const Npc* other = nullptr;
            for (const auto& candidate : npcs_) {
                if (candidate.id == otherId) { other = &candidate; break; }
            }
            if (!other) continue;
            out << "  Relationship with " << other->name << ": " << ToString(rel.type)
                << " (affinity=" << rel.affinity << ", trust=" << rel.trust
                << ", interactions=" << rel.interactionCount << ")\n";
        }
        out << "\n";
    }
}

} // namespace sim
