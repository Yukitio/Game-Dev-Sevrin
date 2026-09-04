# LivingWorldRPG -- Base UE5 Project

A base Unreal Engine 5.5.4 project wiring up the [`prototype/npc-life-sim`](../prototype/npc-life-sim)
simulation as a real, playable-in-editor foundation: a 2D sprite character
(player and 5 NPCs) moving through a full 3D level, with the NPC
simulation running underneath and driving their animation state. No art
exists yet -- every place art belongs is an explicit, empty slot you fill
in from the editor, not something to add later by changing code.

## Before you open this

**None of this has been compiled or run.** It was written without access
to Unreal Engine, so treat it as a careful first draft, not a verified
build. It follows standard UE5.5 C++ project conventions throughout, and
one deliberate choice keeps the risk contained: everything is plain C++
and `.ini` config -- no Blueprints, no Enhanced Input assets, nothing
binary -- so if something doesn't compile, the fix is visible in a normal
compiler error, not hidden in an opaque `.uasset`.

If it doesn't compile cleanly first try, the most likely spots are noted
in [Known risk areas](#known-risk-areas) below.

## Required editor setup (do this once)

The project deliberately can't include a level file (`.umap`) or
Blueprints -- those only exist once Unreal's editor serializes them, so
they can't be authored as plain text. Everything else (the module, the
simulation wrapper, the GameMode that spawns NPCs) is already done. What's
left:

1. Open `LivingWorldRPG.uproject`. Let it build (first build will be
   slow -- it's compiling the engine's shader cache and this module).
2. If prompted "this project was made with a different engine version",
   accept -- `EngineAssociation` is set to `5.5`, adjust if you're on a
   different 5.5.x point release.
3. In the editor: **File > New Level > Empty Level**.
4. Add a floor (any flat StaticMeshActor, scaled up, or a Landscape),
   a Directional Light, and a Player Start.
5. Save the level as `Content/Maps/TestLevel` (that exact path -- it's
   already wired as the default map in `Config/DefaultEngine.ini`).
6. Hit Play. You should see a capsule-shaped player (invisible sprite,
   no art yet) that can walk around with WASD, and 5 NPC actors spawned
   near the origin, each silently ticking through the simulation.

That's it -- no manual NPC placement, no Blueprint wiring. `ALivingWorldGameMode`
spawns one `ALivingWorldNpcCharacter` per NPC registered in the simulation
(see [Sim/World.cpp](Source/LivingWorldRPG/Sim/World.cpp)) automatically
at `BeginPlay`.

## Inserting art

This is the part you asked about directly -- nothing here requires
touching C++ once art exists.

**Character sprites and animation** (`ALivingWorldSpriteCharacterBase`,
in [`LivingWorldSpriteCharacterBase.h`](Source/LivingWorldRPG/LivingWorldSpriteCharacterBase.h)):
every character (player and NPCs alike) has an `ActivityFlipbooks` map --
`FString` state name to `UPaperFlipbook*` -- exposed `EditAnywhere` in the
Details panel. Import your sprite sheets as Paper2D Flipbooks, then either:
- assign them directly on the placed actor / GameMode's default classes, or
- (recommended once you have more than a couple of states) create
  Blueprint subclasses of `ALivingWorldNpcCharacter` / `ALivingWorldPlayerCharacter`
  per character and set their flipbooks there, then point `NpcCharacterClass`
  on `ALivingWorldGameMode` (or `DefaultPawnClass`) at the Blueprint.

The keys NPCs will actually request are exactly `sim::ToString(ActivityType)`
from the simulation (see [`Sim/Types.cpp`](Source/LivingWorldRPG/Sim/Types.cpp)):
`"Sleeping"`, `"Eating"`, `"Working"`, `"Socializing"`, `"Relaxing"`,
`"Idle"`. Missing a key just falls back to `"Idle"` -- safe to fill these
in incrementally, one animation at a time.

**Backgrounds and environment**: since this is a full 3D level (not
Paper2D's flat 2D mode), painted background art comes in as textures/materials
applied to level geometry -- blockout meshes, a skybox/backdrop, or a
Landscape -- authored entirely in the editor. No C++ involved on this side
at all.

## Architecture

```
LivingWorldRPG.uproject          Paper2D plugin enabled
Config/                          DefaultEngine/Game/Input.ini (all plain text)
Source/LivingWorldRPG/
  Sim/                           sim_core, ported unmodified from prototype/npc-life-sim
  LivingWorldSimSubsystem        UGameInstanceSubsystem owning + ticking sim::Simulation,
                                  exposing it to Blueprint/gameplay code
  LivingWorldSpriteCharacterBase Shared 2D-sprite-in-3D-space character (player + NPCs)
  LivingWorldNpcCharacter        Binds to one NPC id, polls its activity each tick
  LivingWorldPlayerCharacter     Camera boom + WASD movement
  LivingWorldGameMode            Spawns one NPC actor per simulation NPC at BeginPlay
```

`ULivingWorldSimSubsystem` is the entire porting boundary described in the
prototype's own README: it owns a `sim::Simulation`, calls `BuildTown()`
once at `Initialize()`, and ticks it on a timer (`InGameHoursPerRealSecond`,
tunable in `Config/DefaultGame.ini` -- defaults to 1 real second = 1
in-game hour, so a full 30-day run plays out over ~12 minutes). Everything
under `Sim/` is byte-for-byte what `prototype/npc-life-sim` already proved
works, including the calendar events and the `Intervene_Encourage/Discourage/Assist`
API from Prototype 3 -- all exposed here as `BlueprintCallable` functions,
ready to wire into UI or gameplay triggers once those exist.

## Known risk areas

Two things in this scaffold are worth checking first if something looks
wrong once you can actually see it running -- everything else here is
fairly mechanical UE5 boilerplate and low-risk by comparison:

- **`ALivingWorldSpriteCharacterBase::FaceActiveCamera()`** (in
  [`LivingWorldSpriteCharacterBase.cpp`](Source/LivingWorldRPG/LivingWorldSpriteCharacterBase.cpp)):
  rotates the flipbook to face the camera and flips it horizontally by
  movement direction. Paper2D's default sprite-plane orientation and which
  axis reads as "flipped" can vary by project setup -- if characters look
  edge-on or flip the wrong way, this function is where to look; it's a
  sign/axis fix, not a structural one.
- **First compile**: this was written to standard UE 5.5 conventions
  (verified against the module/target file format, Paper2D API surface,
  and known member-initialization gotchas like the `TUniquePtr`-to-forward-declared-type
  destructor issue already fixed in `LivingWorldSimSubsystem`), but wasn't
  run through the actual compiler. A missing include or a renamed API
  between engine versions is the most likely failure mode, and the error
  will point straight at the line.

Report back anything that doesn't compile -- that feedback is genuinely
useful, not just "the AI got it wrong": it's the only way to close the gap
between "written correctly" and "verified correctly" for this part of the
project.
