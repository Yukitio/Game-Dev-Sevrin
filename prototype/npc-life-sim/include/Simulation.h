#pragma once

#include <iosfwd>
#include <map>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "CalendarEvent.h"
#include "GameTime.h"
#include "Location.h"
#include "Npc.h"

namespace sim {

class Simulation {
public:
    explicit Simulation(unsigned seed = 1337);

    Npc& AddNpc(Npc npc);
    void AddLocation(Location location);
    void AddEvent(CalendarEvent event);

    void Tick();
    void RunDays(int days);

    const GameTime& Now() const { return clock_; }
    const std::vector<Npc>& Npcs() const { return npcs_; }
    std::vector<Npc>& MutableNpcs() { return npcs_; }
    Npc* FindNpc(const std::string& id);
    std::string LocationName(const std::string& id) const;
    const std::vector<std::string>& EventLog() const { return eventLog_; }

    void Announce(const std::string& message);
    std::string Intervene_Encourage(const std::string& aId, const std::string& bId);
    std::string Intervene_Discourage(const std::string& aId, const std::string& bId);
    std::string Intervene_Assist(const std::string& npcId, NeedType need, float amount);

    void PrintFinalReport(std::ostream& out) const;

private:
    void UpdateNeedsAndActivity(Npc& npc);
    void ApplyActivityEffects(Npc& npc);
    void ResolveInteractions();
    void ResolvePair(Npc& a, Npc& b);
    void ResolveCalendarEvents();
    void ProgressGoal(Npc& npc, const Npc& other, float interactionDelta);
    void Log(const std::string& message);

    GameTime clock_;
    std::mt19937 rng_;
    std::vector<Npc> npcs_;
    std::map<std::string, Location> locations_;
    std::vector<CalendarEvent> events_;
    std::vector<std::string> eventLog_;
    std::set<std::pair<std::string, std::string>> interactedToday_;
};

} // namespace sim
