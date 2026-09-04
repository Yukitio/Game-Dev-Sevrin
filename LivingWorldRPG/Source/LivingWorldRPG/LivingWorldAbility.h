#pragma once

#include "CoreMinimal.h"
#include "LivingWorldAbility.generated.h"

// Base for a magic ability. Blueprintable so designers can create new abilities
// as Blueprint subclasses (override Activate) without touching C++; concrete
// C++ subclasses (like ULivingWorldFireboltAbility) exist for the ones worth
// having strongly-typed. An ULivingWorldAbilityComponent holds a list of these
// as its owner's known spells.
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class LIVINGWORLDRPG_API ULivingWorldAbility : public UObject {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	float ManaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	float Cooldown = 3.0f;

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Activate(AActor* Caster, AActor* Target);
	virtual void Activate_Implementation(AActor* Caster, AActor* Target) {}
};
