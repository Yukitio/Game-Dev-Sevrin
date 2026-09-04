#pragma once

#include "CoreMinimal.h"
#include "LivingWorldSpriteCharacterBase.h"
#include "LivingWorldNpcCharacter.generated.h"

// An autonomous NPC bound to one entry in the simulation. Set NpcId to one of the
// ids registered in Sim/World.cpp's BuildTown() (maren/sela/doran/ilya/corwin) --
// typically by GameMode at spawn time, see LivingWorldGameMode.cpp.
UCLASS()
class LIVINGWORLDRPG_API ALivingWorldNpcCharacter : public ALivingWorldSpriteCharacterBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Living World")
	FString NpcId;

protected:
	virtual void BeginPlay() override;
};
