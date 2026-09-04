#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingWorldAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLivingWorldOnDeath, AActor*, DeadActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLivingWorldOnHealthChanged, float, NewHealth, float, MaxHealth);

// Core stats shared by the player and NPCs alike (so any NPC can be a combatant,
// not just designated enemies). Equipment modifiers are applied as a separate
// bonus layer on top of the base stats, kept in sync by ULivingWorldEquipmentComponent.
UCLASS(ClassGroup = (LivingWorld), meta = (BlueprintSpawnableComponent))
class LIVINGWORLDRPG_API ULivingWorldAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULivingWorldAttributeComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxMana = 50.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	float Mana = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float AttackPower = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Defense = 0.0f;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FLivingWorldOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FLivingWorldOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float ApplyDamage(float RawDamage, AActor* DamageInstigator);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool SpendMana(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void RestoreMana(float Amount);

	UFUNCTION(BlueprintPure, Category = "Attributes")
	bool IsAlive() const { return Health > 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetTotalAttack() const { return AttackPower + BonusAttack; }

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetTotalDefense() const { return Defense + BonusDefense; }

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetTotalMaxHealth() const { return MaxHealth + BonusMaxHealth; }

	void SetEquipmentBonuses(float InBonusAttack, float InBonusDefense, float InBonusMaxHealth);

protected:
	virtual void BeginPlay() override;

private:
	float BonusAttack = 0.0f;
	float BonusDefense = 0.0f;
	float BonusMaxHealth = 0.0f;
};
