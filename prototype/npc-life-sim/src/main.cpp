#include <iostream>

#include "Simulation.h"
#include "World.h"

int main() {
    sim::Simulation simulation(1337);
    sim::BuildTown(simulation);

    std::cout << "Living World RPG -- Prototype 1+2: NPC Life + Calendar\n";
    std::cout << "5 NPCs begin living in the town. No player is present.\n\n";

    const int kDays = 30;
    simulation.RunDays(kDays);

    for (const auto& entry : simulation.EventLog()) {
        std::cout << entry << "\n";
    }

    simulation.PrintFinalReport(std::cout);

    return 0;
}
