#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LivingWorldLocationRegistrySubsystem.generated.h"

// Scans the level for ALivingWorldLocationMarker actors once play begins and
// answers "where is location X" for NPC movement. Locations with no marker
// placed simply aren't found -- callers should treat that as "stay put",
// not an error, since level content may not exist yet.
UCLASS()
class LIVINGWORLDRPG_API ULivingWorldLocationRegistrySubsystem : public UWorldSubsystem {
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintCallable, Category = "Living World")
	bool GetLocation(const FString& LocationId, FVector& OutLocation) const;

private:
	UPROPERTY()
	TMap<FString, FVector> LocationsById;
};
