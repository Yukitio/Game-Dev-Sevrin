#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingWorldAbility.h"
#include "LivingWorldAbilityComponent.generated.h"

// Holds the owner's known abilities and gates activation on mana + per-ability
// cooldown. Add entries to KnownAbilities in the Details panel (Instanced
// property, so each entry gets its own editable copy) to give a character spells.
UCLASS(ClassGroup = (LivingWorld), meta = (BlueprintSpawnableComponent))
class LIVINGWORLDRPG_API ULivingWorldAbilityComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Abilities")
	TArray<TObjectPtr<ULivingWorldAbility>> KnownAbilities;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateAbility(int32 AbilityIndex, AActor* Target);

private:
	UPROPERTY()
	TMap<int32, float> LastActivationTimeByIndex;
};
