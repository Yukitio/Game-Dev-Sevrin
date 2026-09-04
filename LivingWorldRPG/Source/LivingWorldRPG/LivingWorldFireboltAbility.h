#pragma once

#include "CoreMinimal.h"
#include "LivingWorldAbility.h"
#include "LivingWorldFireboltAbility.generated.h"

UCLASS()
class LIVINGWORLDRPG_API ULivingWorldFireboltAbility : public ULivingWorldAbility {
	GENERATED_BODY()

public:
	ULivingWorldFireboltAbility();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	float Damage = 25.0f;

	virtual void Activate_Implementation(AActor* Caster, AActor* Target) override;
};
