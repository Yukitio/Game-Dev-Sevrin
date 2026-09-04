#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LivingWorldGameMode.generated.h"

class ALivingWorldNpcCharacter;

// Spawns one NPC actor per id registered in the simulation (see Sim/World.cpp) at
// BeginPlay, so the test level needs no manually-placed NPCs -- just a floor,
// light, and player start. See the project README for the rest of the level setup.
UCLASS()
class LIVINGWORLDRPG_API ALivingWorldGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALivingWorldGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Living World")
	TSubclassOf<ALivingWorldNpcCharacter> NpcCharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Living World")
	float NpcSpawnSpacing = 200.0f;
};
