#include "World.h"

#include "Simulation.h"

namespace sim {

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

void RegisterCalendarEvents(Simulation& sim) {
    CalendarEvent marketDay;
    marketDay.id = "market_day";
    marketDay.name = "Market Day";
    marketDay.triggerDay = 3;
    marketDay.triggerHour = 10;
    marketDay.recurEveryDays = 7;
    marketDay.effect = [](Simulation& s) {
        Npc* ilya = s.FindNpc("ilya");
        Npc* doran = s.FindNpc("doran");
        if (!ilya) return;
        ilya->needs.Adjust(NeedType::Money, 12.0f);
        s.Announce("Market Day: Ilya sells produce at the town square, filling her purse.");
        if (doran) {
            doran->needs.Adjust(NeedType::Money, 4.0f);
            s.Announce("Doran restocks the inn's larder from the market.");
        }
    };
    sim.AddEvent(marketDay);

    CalendarEvent inspection;
    inspection.id = "guild_inspection";
    inspection.name = "Blacksmiths' Guild Inspection";
    inspection.triggerDay = 10;
    inspection.triggerHour = 12;
    inspection.effect = [](Simulation& s) {
        Npc* maren = s.FindNpc("maren");
        if (!maren) return;
        float progress = 0.0f;
        for (auto& g : maren->goals) {
            if (g.type == GoalType::CareerGrowth) progress = g.progress;
        }
        if (progress >= 50.0f) {
            s.Announce("Guild Inspection: The blacksmiths' guild praises Maren's forge without reservation.");
            for (auto& g : maren->goals) {
                if (g.type == GoalType::CareerGrowth && !g.completed) g.progress += 10.0f;
            }
        } else {
            s.Announce("Guild Inspection: The guild finds Maren's forge adequate, with room to grow.");
            for (auto& g : maren->goals) {
                if (g.type == GoalType::CareerGrowth && !g.completed) g.progress += 3.0f;
            }
        }
    };
    sim.AddEvent(inspection);

    CalendarEvent convocation;
    convocation.id = "healers_convocation";
    convocation.name = "Healer's Convocation";
    convocation.triggerDay = 20;
    convocation.triggerHour = 9;
    convocation.effect = [](Simulation& s) {
        Npc* sela = s.FindNpc("sela");
        if (!sela) return;
        bool mastered = false;
        for (auto& g : sela->goals) {
            if (g.type == GoalType::Mastery && g.completed) mastered = true;
        }
        if (mastered) {
            s.Announce("Healer's Convocation: Sela is invited to lecture on her perfected remedy.");
            sela->needs.Adjust(NeedType::Social, 15.0f);
        } else {
            s.Announce("Healer's Convocation: Sela attends as a student, taking notes for her own research.");
            for (auto& g : sela->goals) {
                if (g.type == GoalType::Mastery && !g.completed) g.progress += 5.0f;
            }
        }
    };
    sim.AddEvent(convocation);

    CalendarEvent festival;
    festival.id = "harvest_festival";
    festival.name = "Harvest Festival";
    festival.triggerDay = 25;
    festival.triggerHour = 18;
    festival.effect = [](Simulation& s) {
        s.Announce("Harvest Festival: The whole town gathers at the square for food, music, and firelight.");
        for (auto& npc : s.MutableNpcs()) {
            npc.needs.Adjust(NeedType::Social, 10.0f);
        }
        Npc* ilya = s.FindNpc("ilya");
        Npc* corwin = s.FindNpc("corwin");
        if (!ilya || !corwin) return;
        auto it = ilya->relationships.find("corwin");
        if (it != ilya->relationships.end() && it->second.type == RelationshipType::Romantic) {
            s.Announce("Corwin and Ilya are seen dancing together; the town takes note of the new couple.");
        } else {
            s.Announce("Corwin and Ilya share a friendly toast among the crowd.");
        }
    };
    sim.AddEvent(festival);
}

} // namespace

void BuildTown(Simulation& sim) {
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

    RegisterCalendarEvents(sim);
}

} // namespace sim
