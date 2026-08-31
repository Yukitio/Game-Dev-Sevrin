# Prototypes 1–3 — NPC Life, Calendar, Intervention

The first three builds in the GDD's [Development Strategy](../../docs/GDD.md#14-development-strategy),
in order: 5 NPCs with schedules, needs, goals, and relationships living
autonomously; a persistent calendar with recurring, scheduled, and
conditional events; and a player who can step in and change outcomes. The
goal across all three is to prove the load-bearing claim of the whole
design — *"the world would exist without the player"* — before any engine
work happens.

This is a standalone console simulation, deliberately built with **no
engine dependency**: plain C++17 and the standard library only. See
[Porting to Unreal](#porting-to-unreal) for why.

## What it simulates

- **5 NPCs** in a small town: a blacksmith, a herbalist, an innkeeper, a
  farmer, and a town guard — each with a personality, a daily routine, and
  a personal goal.
- **Needs** (Hunger, Energy, Social, Money) decay every in-game hour and
  are restored by matching activities. A critical need can override an
  NPC's schedule (e.g. skip to eating).
- **Schedules** drive where each NPC is and what they're doing hour by
  hour — work, meals, sleep, and an evening at the town's shared social
  hub, the Wayfarer's Inn.
- **Relationships** emerge from NPCs sharing a location and activity, not
  from anything scripted between specific pairs. Affinity and trust
  accumulate from repeated interaction (capped at one interaction per pair
  per day, so a month produces a believable arc instead of everyone
  maxing out by day three).
- **Goals** (career growth, mastery, wealth, friendship, romance) progress
  passively through the NPC's normal routine and interactions, and are
  announced when completed.
- **A calendar** drives four town-wide events: a recurring Market Day
  (every 7 days) and three fixed-date events whose outcome branches on
  world state at the moment they fire — see [Prototype 2](#prototype-2--calendar).
- **Player intervention** lets you step into the town's evenings and
  encourage, discourage, or assist specific NPCs — see
  [Prototype 3](#prototype-3--player-intervention).

The five NPCs were seeded with one deliberately asymmetric pair: Corwin
(the guard) has a *Romance* goal targeting Ilya (the farmer), while Ilya's
own goal is only *Friendship* with Corwin — the GDD's "unintended paths"
pillar in miniature: nobody labels this as romance or friendship for the
player, it just plays out from both sides' perspectives.

## Build & run

Requires CMake 3.16+ and a C++17 compiler. The build produces two
executables sharing one `sim_core` library.

```bash
cmake -S . -B build
cmake --build build
./build/npc_life_sim              # Prototypes 1+2: watch the town live, no input
./build/npc_life_sim_interactive  # Prototype 3: step in and intervene
```

## Prototype 1 — NPC Life

`npc_life_sim` runs the 30-day town with no player. Sample event log with
the default seed (1337) (needs/goal snapshots trimmed — the full report
prints after the log):

```
[Day  1, 18:00] Maren and Sela are now Acquaintance.
...
[Day  5, 18:00] Maren and Corwin are now Friend.
[Day  5, 18:00] Sela and Ilya are now Friend.
...
[Day 12, 18:00] Ilya has achieved a lasting friendship with Corwin.
[Day 12, 18:00] Corwin's feelings for Ilya are now returned. They are Romantic Partners.
...
[Day 16, 14:00] Maren has achieved their goal: Career Growth!
...
[Day 18, 08:00] Doran has achieved their goal: Wealth!
...
[Day 20, 11:00] Sela has achieved their goal: Mastery!
```

Nobody drove any of this — it's the schedule, needs, and personality
compatibility of five autonomous NPCs playing out over a month.

## Prototype 2 — Calendar

A `CalendarEvent` fires when the clock reaches its trigger day/hour, either
once (`recurEveryDays == 0`) or on a repeating cadence. Four events are
registered in [`World.cpp`](src/World.cpp):

| Event | Trigger | Conditional outcome |
|---|---|---|
| Market Day | every 7 days from Day 3 | always fires; Ilya and Doran both gain Money |
| Blacksmiths' Guild Inspection | Day 10 | branches on Maren's Career Growth progress at that moment |
| Healer's Convocation | Day 20 | branches on whether Sela has already completed Mastery |
| Harvest Festival | Day 25 | branches on whether Corwin and Ilya are Romantic yet |

From the same seeded run: the Guild Inspection lands after Maren's well
underway ("praises Maren's forge without reservation"); the Convocation
catches Sela two hours *before* she finishes her Mastery goal, so she
attends as a student instead of a lecturer; and by the Harvest Festival,
Corwin and Ilya are already a couple, so the town "takes note of the new
couple" rather than getting the generic toast line. None of that is
scripted per-run — it reads whatever state the NPCs happen to be in.

## Prototype 3 — Player Intervention

`npc_life_sim_interactive` runs the same town, but pauses every 3rd
evening (Day 3, 6, 9, ... 30) with an observation of the town and a
prompt:

```
--- Day  3, 18:00 : Town Observation ---
  Maren (Blacksmith): Working at The Forge
  Sela (Herbalist): Working at Herbalist's Hut
  Doran (Innkeeper): Working at The Wayfarer's Inn
  Ilya (Farmer): Working at Ilya's Farm
  Corwin (Town Guard): Working at Town Gate
> observe corwin
Corwin (Town Guard) is Working at Town Gate
  Hunger=57 Energy=70 Social=70 Money=100
  Goal [Romance]: in progress (19.5/100)
  ...
> discourage corwin ilya
[PLAYER] You sow doubt between Corwin and Ilya.
> skip
```

Commands: `observe <npc>`, `encourage <a> <b>`, `discourage <a> <b>`,
`assist <npc> <hunger|energy|social|money>`, `skip`, `run` (stop
prompting and let the rest of the month play out on its own — the
world doesn't pause just because you stopped watching), `help`.

This is the GDD's "no traditional route menu" pillar in code: there's no
menu of outcomes, just verbs applied to whatever state the town is
actually in, and the consequences play out through the same simulation
Prototype 1 runs. Concretely, against the seeded town:

- **Doing nothing** (`skip` every window) reproduces Prototype 1's run
  exactly — confirms intervention is additive, not a different code path.
- **One or two `discourage corwin ilya`** early on doesn't prevent the
  romance, just delays it (Day 12 → Day 17) and leaves it permanently
  weaker (final affinity 93.4 vs. 100, trust 33.4 vs. 40) — a small
  decision with a lasting but non-deterministic effect, not an on/off
  switch.
- **`discourage corwin ilya` at every single window** overwhelms it
  entirely: Corwin's Romance goal stalls at 22.7% and the two end the
  month as **Rivals** (affinity −66.6) instead of a couple.
- **`encourage maren doran`** once measurably accelerates that
  friendship (affinity 67.8 vs. baseline 52.8) without touching anything
  else — an isolated, observable nudge.

"Missed opportunities are a valid outcome" holds too: the decision
windows are only every 3rd evening, and only at 18:00 — whatever happens
on the other nights and hours happens whether or not anyone was watching.

## Code layout

```
include/   Types, GameTime, Personality, Needs, Location, Schedule,
           Relationship, Goal, Npc, CalendarEvent, Simulation, World
src/       Matching .cpp files, plus:
             World.cpp             the town: locations, 5 NPCs, calendar events
             main.cpp               Prototype 1+2 driver (no input)
             interactive_main.cpp   Prototype 3 driver (decision windows + commands)
```

`Simulation` owns the clock, the NPCs, the event calendar, and the tick
loop, plus the public intervention API (`Intervene_Encourage`,
`Intervene_Discourage`, `Intervene_Assist`). Everything else is plain data
plus small pure functions (`Compatibility`, `ClassifyRelationship`). All
three source files build against one `sim_core` static library — there's
still no rendering and no engine, only two different front ends on the
same simulation.

## Porting to Unreal

Per the GDD's [Technical Direction](../../docs/GDD.md#technical-direction)
(Unreal Engine 5.5.4, C++-first, Blueprint only where it clearly helps),
this code is written so it can move into a UE5 module's `Source/` folder
largely as-is:

- No engine types, no engine macros — `std::string`/`std::vector` compile
  and run unchanged alongside `FString`/`TArray` in a UE module; nothing
  here has to be rewritten to satisfy UBT.
- The porting work is a thin adapter layer, not a rewrite: wrap
  `sim::Simulation` in a `UWorldSubsystem` (or similar) that owns an
  instance and calls `Tick()` from `Tick(float DeltaTime)`, and expose
  the parts that need Blueprint access (NPC state for UI, relationship
  queries for dialogue, the three `Intervene_*` calls for player actions)
  through `UFUNCTION`/`UPROPERTY` wrappers around the existing data.
- `main.cpp`/`interactive_main.cpp` and the CMake build stay dev-only
  tooling for iterating on the simulation logic in isolation — the way
  the GDD's prototyping order intends. Prototype 4 (presentation: the
  hand-painted background + pixel character pipeline) is the first step
  that actually needs the engine.
