#include <iostream>
#include <sstream>
#include <string>

#include "Simulation.h"
#include "World.h"

using sim::NeedType;
using sim::Simulation;

namespace {

constexpr int kDecisionIntervalDays = 3;
constexpr int kDecisionHour = 18;
constexpr float kAssistAmount = 25.0f;

void PrintHelp() {
    std::cout << "Commands:\n"
              << "  observe <npc>            look closer at one person"
              << " (maren/sela/doran/ilya/corwin)\n"
              << "  encourage <a> <b>        nudge two people closer together\n"
              << "  discourage <a> <b>       sow doubt between two people\n"
              << "  assist <npc> <need>      help with hunger/energy/social/money\n"
              << "  skip                     do nothing this evening, let the day continue\n"
              << "  run                      stop observing; the rest of the month plays out on its own\n"
              << "  help                     show this again\n";
}

bool ParseNeed(const std::string& s, NeedType& out) {
    if (s == "hunger") { out = NeedType::Hunger; return true; }
    if (s == "energy") { out = NeedType::Energy; return true; }
    if (s == "social") { out = NeedType::Social; return true; }
    if (s == "money") { out = NeedType::Money; return true; }
    return false;
}

void ObserveTown(const Simulation& s) {
    std::cout << "\n--- " << s.Now().ToString() << " : Town Observation ---\n";
    for (const auto& npc : s.Npcs()) {
        std::cout << "  " << npc.name << " (" << npc.occupation << "): "
                  << sim::ToString(npc.currentActivity) << " at " << s.LocationName(npc.currentLocationId)
                  << "\n";
    }
}

void ObserveOne(const Simulation& s, const std::string& id) {
    for (const auto& npc : s.Npcs()) {
        if (npc.id != id) continue;
        std::cout << npc.name << " (" << npc.occupation << ") is " << sim::ToString(npc.currentActivity)
                  << " at " << s.LocationName(npc.currentLocationId) << "\n";
        std::cout << "  Hunger=" << npc.needs.Get(NeedType::Hunger)
                   << " Energy=" << npc.needs.Get(NeedType::Energy)
                   << " Social=" << npc.needs.Get(NeedType::Social)
                   << " Money=" << npc.needs.Get(NeedType::Money) << "\n";
        for (const auto& goal : npc.goals) {
            std::cout << "  Goal [" << sim::ToString(goal.type) << "]: "
                      << (goal.completed ? "COMPLETE" : "in progress") << " (" << goal.progress << "/100)\n";
        }
        for (const auto& [otherId, rel] : npc.relationships) {
            const sim::Npc* other = nullptr;
            for (const auto& candidate : s.Npcs()) {
                if (candidate.id == otherId) { other = &candidate; break; }
            }
            if (!other) continue;
            std::cout << "  " << other->name << ": " << sim::ToString(rel.type)
                       << " (affinity=" << rel.affinity << ")\n";
        }
        return;
    }
    std::cout << "No one by that name. Try: maren, sela, doran, ilya, corwin.\n";
}

void PrintNewLog(const Simulation& s, size_t& lastIndex) {
    for (; lastIndex < s.EventLog().size(); ++lastIndex) {
        std::cout << s.EventLog()[lastIndex] << "\n";
    }
}

} // namespace

int main() {
    Simulation simulation(1337);
    sim::BuildTown(simulation);

    std::cout << "Living World RPG -- Prototype 3: Player Intervention\n";
    std::cout << "You may step into the town's evenings. The world keeps moving whether you do or not.\n";
    PrintHelp();

    size_t lastLogIndex = 0;
    bool autoRun = false;

    for (int day = 1; day <= 30; ++day) {
        for (int hour = 0; hour < sim::kHoursPerDay; ++hour) {
            simulation.Tick();
            PrintNewLog(simulation, lastLogIndex);

            if (autoRun || simulation.Now().hour != kDecisionHour ||
                simulation.Now().day % kDecisionIntervalDays != 0) {
                continue;
            }

            ObserveTown(simulation);
            std::cout << "> " << std::flush;

            std::string line;
            bool advance = false;
            while (!advance && std::getline(std::cin, line)) {
                std::istringstream iss(line);
                std::string cmd;
                iss >> cmd;

                if (cmd.empty() || cmd == "skip") {
                    advance = true;
                } else if (cmd == "run" || cmd == "quit" || cmd == "exit") {
                    autoRun = true;
                    advance = true;
                    std::cout << "You step back and let the town live its own life for the rest of the month.\n";
                } else if (cmd == "help") {
                    PrintHelp();
                    std::cout << "> " << std::flush;
                } else if (cmd == "observe") {
                    std::string who;
                    iss >> who;
                    ObserveOne(simulation, who);
                    std::cout << "> " << std::flush;
                } else if (cmd == "encourage" || cmd == "discourage") {
                    std::string a, b;
                    iss >> a >> b;
                    std::string result = cmd == "encourage" ? simulation.Intervene_Encourage(a, b)
                                                              : simulation.Intervene_Discourage(a, b);
                    std::cout << result << "\n";
                    PrintNewLog(simulation, lastLogIndex);
                    std::cout << "> " << std::flush;
                } else if (cmd == "assist") {
                    std::string who, needStr;
                    iss >> who >> needStr;
                    NeedType need;
                    if (ParseNeed(needStr, need)) {
                        std::cout << simulation.Intervene_Assist(who, need, kAssistAmount) << "\n";
                    } else {
                        std::cout << "Unknown need. Try hunger/energy/social/money.\n";
                    }
                    std::cout << "> " << std::flush;
                } else {
                    std::cout << "Unknown command. Type 'help'.\n> " << std::flush;
                }
            }

            if (!advance) {
                autoRun = true;
            }
        }
    }

    PrintNewLog(simulation, lastLogIndex);
    simulation.PrintFinalReport(std::cout);

    return 0;
}
