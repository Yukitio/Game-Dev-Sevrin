#include <iostream>

#include "Simulation.h"

using namespace sim;

namespace {

Npc MakeNpc(const std::string& id, const std::string& name, const std::string& occupation,
            const std::string& home, Personality personality) {
    Npc npc;
    npc.id = id;
    npc.name = name;
    npc.occupation = occupation;
    npc.homeLocationId = home;
    npc.personality = personality;
    return npc;
}

void AddDailyRoutine(Npc& npc, const std::string& workplace, const std::string& socialHub,
                      bool worksEvening) {
    npc.schedule.AddBlock({22, 6, ActivityType::Sleep, npc.homeLocationId});
    npc.schedule.AddBlock({6, 7, ActivityType::Eat, npc.homeLocationId});
    npc.schedule.AddBlock({7, 12, ActivityType::Work, workplace});
    npc.schedule.AddBlock({12, 13, ActivityType::Eat, workplace});
    npc.schedule.AddBlock({13, 18, ActivityType::Work, workplace});
    if (worksEvening) {
        npc.schedule.AddBlock({18, 21, ActivityType::Work, socialHub});
    } else {
        npc.schedule.AddBlock({18, 21, ActivityType::Socialize, socialHub});
    }
    npc.schedule.AddBlock({21, 22, ActivityType::Leisure, npc.homeLocationId});
}

} // namespace

int main() {
    Simulation sim(1337);

    sim.AddLocation({"home_maren", "Maren's Cottage"});
    sim.AddLocation({"home_sela", "Sela's Cottage"});
    sim.AddLocation({"home_doran", "Doran's Room at the Inn"});
    sim.AddLocation({"home_ilya", "Ilya's Farmhouse"});
    sim.AddLocation({"home_corwin", "Corwin's Barracks Room"});
    sim.AddLocation({"forge", "The Forge"});
    sim.AddLocation({"herbalist_hut", "Herbalist's Hut"});
    sim.AddLocation({"inn", "The Wayfarer's Inn"});
    sim.AddLocation({"farm", "Ilya's Farm"});
    sim.AddLocation({"town_gate", "Town Gate"});

    Npc maren = MakeNpc("maren", "Maren", "Blacksmith", "home_maren", {0.2f, 0.8f, 0.1f});
    maren.goals.push_back({GoalType::CareerGrowth, "", 0.0f});
    AddDailyRoutine(maren, "forge", "inn", false);

    Npc sela = MakeNpc("sela", "Sela", "Herbalist", "home_sela", {0.6f, 0.5f, -0.2f});
    sela.goals.push_back({GoalType::Mastery, "", 0.0f});
    AddDailyRoutine(sela, "herbalist_hut", "inn", false);

    Npc doran = MakeNpc("doran", "Doran", "Innkeeper", "home_doran", {0.9f, 0.4f, 0.0f});
    doran.goals.push_back({GoalType::Wealth, "", 0.0f});
    AddDailyRoutine(doran, "inn", "inn", true);

    Npc ilya = MakeNpc("ilya", "Ilya", "Farmer", "home_ilya", {0.5f, 0.2f, -0.1f});
    ilya.goals.push_back({GoalType::Friendship, "corwin", 0.0f});
    AddDailyRoutine(ilya, "farm", "inn", false);

    Npc corwin = MakeNpc("corwin", "Corwin", "Town Guard", "home_corwin", {0.3f, 0.3f, 0.2f});
    corwin.goals.push_back({GoalType::Romance, "ilya", 0.0f});
    AddDailyRoutine(corwin, "town_gate", "inn", false);

    sim.AddNpc(maren);
    sim.AddNpc(sela);
    sim.AddNpc(doran);
    sim.AddNpc(ilya);
    sim.AddNpc(corwin);

    std::cout << "Living World RPG -- Prototype 1: NPC Life\n";
    std::cout << "5 NPCs begin living in the town. No player is present.\n\n";

    const int kDays = 30;
    sim.RunDays(kDays);

    for (const auto& entry : sim.EventLog()) {
        std::cout << entry << "\n";
    }

    sim.PrintFinalReport(std::cout);

    return 0;
}
