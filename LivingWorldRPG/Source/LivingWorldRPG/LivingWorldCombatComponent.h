#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingWorldCombatComponent.generated.h"

// Minimal melee combat: a cooldown-gated sweep in front of the owner that
// applies AttackPower to any hit actor's AttributeComponent. No animation
// hookup yet -- call TryMeleeAttack() from input or AI and it just works,
// visuals attach later without changing this contract.
UCLASS(ClassGroup = (LivingWorld), meta = (BlueprintSpawnableComponent))
class LIVINGWORLDRPG_API ULivingWorldCombatComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCooldown = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TryMeleeAttack();

private:
	float LastAttackTime = -1000.0f;
};
