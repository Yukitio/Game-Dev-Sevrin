# Prototype 1 — NPC Life

The first build in the GDD's [Development Strategy](../../docs/GDD.md#14-development-strategy):
5 NPCs with schedules, needs, goals, and relationships, left to live for 30
in-game days with no player present. The goal is to prove the load-bearing
claim of the whole design — *"the world would exist without the player"* —
before any engine work happens.

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

The five NPCs were seeded with one deliberately asymmetric pair: Corwin
(the guard) has a *Romance* goal targeting Ilya (the farmer), while Ilya's
own goal is only *Friendship* with Corwin — the GDD's "unintended paths"
pillar in miniature: nobody labels this as romance or friendship for the
player, it just plays out from both sides' perspectives.

## Build & run

Requires CMake 3.16+ and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
./build/npc_life_sim
```

## Sample output

With the default seed (1337), the 30-day run produces this event log
(needs/goal snapshots trimmed — full report prints after the log):

```
[Day  1, 18:00] Maren and Sela are now Acquaintance.
...
[Day  5, 18:00] Maren and Corwin are now Friend.
[Day  5, 18:00] Sela and Ilya are now Friend.
...
[Day 12, 18:00] Ilya has achieved a lasting friendship with Corwin.
[Day 12, 18:00] Corwin's feelings for Ilya are now returned. They are Romantic Partners.
...
[Day 18, 08:00] Doran has achieved their goal: Wealth!
[Day 18, 11:00] Maren has achieved their goal: Career Growth!
[Day 21, 13:00] Sela has achieved their goal: Mastery!
```

Nobody drove any of this — it's the schedule, needs, and personality
compatibility of five autonomous NPCs playing out over a month.

## Code layout

```
include/   Types, GameTime, Personality, Needs, Location, Schedule,
           Relationship, Goal, Npc, Simulation — engine-agnostic headers
src/       Matching .cpp files, plus main.cpp (the town + CLI driver)
```

`Simulation` owns the clock, the NPCs, and the tick loop; everything else
is plain data plus small pure functions (`Compatibility`,
`ClassifyRelationship`). There's no rendering, no input, no player —
intentionally, per the GDD's own prototyping order.

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
  queries for dialogue) through `UFUNCTION`/`UPROPERTY` wrappers around
  the existing data.
- `main.cpp` and the CMake build stay dev-only tooling for iterating on
  the simulation logic in isolation, the way the GDD's prototyping order
  intends — Prototype 2 (calendar) and Prototype 3 (player intervention)
  build on this same core before it needs to touch the engine at all.
