# LivingWorldRPG -- Base UE5 Project

A base Unreal Engine 5.5.4 project wiring up the [`prototype/npc-life-sim`](../prototype/npc-life-sim)
simulation as a real, playable-in-editor foundation: a 2D sprite character
(player and 5 NPCs) moving through a full 3D level, with the NPC
simulation running underneath and driving their animation state, plus a
standard RPG layer (combat, equipment, magic, progression) on top. No art
exists yet -- every place art belongs is an explicit, empty slot you fill
in from the editor, not something to add later by changing code.

**Status: builds and runs.** Both the core project (simulation subsystem,
character classes, GameMode auto-spawning the 5 NPCs) and the RPG systems
(Prototype 5: combat, equipment, magic, progression) have been verified
compiling and playing in the editor (UE 5.5.4, MSVC).

## Updating after a new push

Run [`update_and_build.bat`](update_and_build.bat) (double-click it in
File Explorer, or run it from a terminal) instead of doing this by hand.
It pulls the latest changes, clears the old build cache (this avoids a
real bug in Unreal's incremental build system where it can report
"target is up to date" without actually compiling new files after a
`git pull`), rebuilds, and prints one of two banners:

- **`BUILD SUCCEEDED`** -- open `LivingWorldRPG.uproject` as normal.
- **`BUILD FAILED`** -- it prints just the error lines (not the full
  multi-thousand-line build log) and tells you where the full log is
  saved. Copy what it shows between the dashed lines and send it over.

Close the editor and Visual Studio before running it, so the build
tools aren't fighting over locked files.

## Required editor setup (do this once)

The project deliberately can't include a level file (`.umap`) or
Blueprints -- those only exist once Unreal's editor serializes them, so
they can't be authored as plain text. Everything else (the module, the
simulation wrapper, the GameMode that spawns NPCs) is already done. What's
left:

1. Open `LivingWorldRPG.uproject`. Let it build (first build will be
   slow -- it's compiling the engine's shader cache and this module).
2. In the editor: **File > New Level > Empty Level**.
3. Add a floor (any flat StaticMeshActor, scaled up, or a Landscape),
   a Directional Light, and a Player Start.
4. Save the level as `Content/Maps/TestLevel` (that exact path -- it's
   already wired as the default map in `Config/DefaultEngine.ini`).
5. Hit Play, then **click inside the viewport** to give it input focus.
   WASD moves the player; **left mouse button** (or gamepad face button
   south) triggers a melee attack via the new CombatComponent. Check the
   Outliner to confirm the 5 NPC actors spawned.

`ALivingWorldGameMode` spawns one `ALivingWorldNpcCharacter` per NPC
registered in the simulation (see
[Sim/World.cpp](Source/LivingWorldRPG/Sim/World.cpp)) automatically at
`BeginPlay` -- no manual NPC placement, no Blueprint wiring required.

## Inserting art

Nothing here requires touching C++ once art exists.

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

## RPG systems (Prototype 5)

Per the GDD's Core RPG Loop ("traditional RPG progression sits underneath"
the living-world loop), every sprite character -- player and NPCs alike,
so any NPC can be a combatant, boss, or vendor -- carries five components,
all in [`Source/LivingWorldRPG/`](Source/LivingWorldRPG):

| Component | Does |
|---|---|
| `ULivingWorldAttributeComponent` | Health, Mana, Attack, Defense. `ApplyDamage`/`Heal`/`SpendMana`, `OnDeath`/`OnHealthChanged` delegates. |
| `ULivingWorldEquipmentComponent` | Equips `ULivingWorldEquippableItem` assets per slot (Weapon/Armor/Accessory), aggregates their stat modifiers into the AttributeComponent. |
| `ULivingWorldCombatComponent` | `TryMeleeAttack()` -- cooldown-gated sweep in front of the owner, applies `GetTotalAttack()` to whatever it hits. Bound to left-click on the player. |
| `ULivingWorldProgressionComponent` | `GainExperience()`, level-up loop with a growing XP threshold, linear stat growth per level. |
| `ULivingWorldAbilityComponent` | Holds a list of `ULivingWorldAbility` (mana cost + cooldown + `Activate`), gates activation the same way combat does. |

None of this depends on art -- it's already fully playable with invisible
capsules, exactly like the NPC simulation itself.

### The template pattern (for the world/character mass-production tools)

`ULivingWorldEquippableItem` (in
[`LivingWorldEquipment.h`](Source/LivingWorldRPG/LivingWorldEquipment.h))
is a `UPrimaryDataAsset`, not a UObject or a struct. That's deliberate: it
means a sword, a shield, or a ring is a **Data Asset you create in the
Content Browser** (right-click > Miscellaneous > Data Asset > pick the
class, fill in the fields) -- no C++ and no Blueprint graph needed per
item. `ULivingWorldAbility` is `Blueprintable` for the same reason at one
level up (new spells as Blueprint subclasses when C++ isn't warranted).

This is the pattern the planned **world-building template** and
**character mass-production template** systems will extend: define an
NPC archetype or a location layout once as a Data Asset class in C++
(personality ranges, occupation, starting equipment, schedule shape), then
let content scale as data instances instead of hand-written `BuildTown()`
C++ entries -- the same move already made here for items.

## Architecture

```
LivingWorldRPG.uproject          Paper2D plugin enabled
Config/                          DefaultEngine/Game/Input.ini (all plain text)
Source/LivingWorldRPG/
  Sim/                           sim_core, ported unmodified from prototype/npc-life-sim
  LivingWorldSimSubsystem        UGameInstanceSubsystem owning + ticking sim::Simulation,
                                  exposing it to Blueprint/gameplay code
  LivingWorldSpriteCharacterBase Shared 2D-sprite-in-3D-space character (player + NPCs),
                                  owns all 5 RPG components below
  LivingWorldAttributeComponent  Health / Mana / Attack / Defense
  LivingWorldEquipment(Component) Equippable item Data Asset + the component wearing them
  LivingWorldCombatComponent     Melee attack
  LivingWorldProgressionComponent XP and leveling
  LivingWorldAbility(Component)  Magic/abilities base + the component casting them
  LivingWorldFireboltAbility     One concrete example ability
  LivingWorldNpcCharacter        Binds to one NPC id, polls its activity each tick
  LivingWorldPlayerCharacter     Camera boom + WASD movement + attack input
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

- **New code added after this point hasn't been compiled yet.** Any
  fresh classes/components follow the same conventions already verified
  working (forward declarations for pointer `UPROPERTY`s, full includes
  only where a complete type is actually needed, no UObject pointers as
  reflected `TMap` keys), but "written correctly" and "verified" are
  different things until `update_and_build.bat` says `BUILD SUCCEEDED`.
- **`ALivingWorldSpriteCharacterBase::FaceActiveCamera()`**: rotates the
  flipbook to face the camera and flips it horizontally by movement
  direction. Paper2D's default sprite-plane orientation and which axis
  reads as "flipped" can vary by project setup -- once real art is in,
  this is the first place to check if a character looks edge-on or flips
  the wrong way.

Report back anything that doesn't compile or look right once you can see
it running -- that feedback is what actually closes the gap between
"written correctly" and "verified correctly."
