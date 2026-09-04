#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingWorldProgressionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLivingWorldOnLevelUp, int32, NewLevel);

// Simple XP/level progression. Each level-up grows the owner's AttributeComponent
// stats directly -- no separate stat curve asset yet, just linear growth per level,
// easy to swap for a curve table later without changing the public API.
UCLASS(ClassGroup = (LivingWorld), meta = (BlueprintSpawnableComponent))
class LIVINGWORLDRPG_API ULivingWorldProgressionComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	float Experience = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float ExperienceToNextLevel = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float ExperienceGrowthRate = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float AttackGrowthPerLevel = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float DefenseGrowthPerLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float MaxHealthGrowthPerLevel = 10.0f;

	UPROPERTY(BlueprintAssignable, Category = "Progression")
	FLivingWorldOnLevelUp OnLevelUp;

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void GainExperience(float Amount);

private:
	void LevelUp();
};
