#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LivingWorldLocationMarker.generated.h"

class UBillboardComponent;

// Place one of these in the level per location the simulation knows about
// (see the ids registered in Sim/World.cpp's BuildTown() -- "forge", "inn",
// "farm", "home_maren", etc.). No art needed: this is just a position marker
// NPCs use to know where to actually walk. Purely an editor-time placement
// tool, invisible in a packaged build.
UCLASS()
class LIVINGWORLDRPG_API ALivingWorldLocationMarker : public AActor {
	GENERATED_BODY()

public:
	ALivingWorldLocationMarker();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Living World")
	FString LocationId;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> EditorBillboard;
#endif
};
