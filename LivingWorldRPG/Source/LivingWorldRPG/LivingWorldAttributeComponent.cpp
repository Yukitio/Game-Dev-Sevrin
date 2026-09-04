#include "LivingWorldAttributeComponent.h"

#include "GameFramework/Actor.h"

ULivingWorldAttributeComponent::ULivingWorldAttributeComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void ULivingWorldAttributeComponent::BeginPlay() {
	Super::BeginPlay();
	Health = GetTotalMaxHealth();
	Mana = MaxMana;
}

float ULivingWorldAttributeComponent::ApplyDamage(float RawDamage, AActor* DamageInstigator) {
	if (!IsAlive() || RawDamage <= 0.0f) {
		return 0.0f;
	}

	const float Mitigated = FMath::Max(0.0f, RawDamage - GetTotalDefense());
	const float ActualDamage = FMath::Min(Mitigated, Health);
	Health -= ActualDamage;
	OnHealthChanged.Broadcast(Health, GetTotalMaxHealth());

	if (Health <= 0.0f) {
		OnDeath.Broadcast(GetOwner());
	}

	return ActualDamage;
}

void ULivingWorldAttributeComponent::Heal(float Amount) {
	if (Amount <= 0.0f || !IsAlive()) {
		return;
	}
	Health = FMath::Min(Health + Amount, GetTotalMaxHealth());
	OnHealthChanged.Broadcast(Health, GetTotalMaxHealth());
}

bool ULivingWorldAttributeComponent::SpendMana(float Amount) {
	if (Amount <= 0.0f || Mana < Amount) {
		return false;
	}
	Mana -= Amount;
	return true;
}

void ULivingWorldAttributeComponent::RestoreMana(float Amount) {
	if (Amount <= 0.0f) {
		return;
	}
	Mana = FMath::Min(Mana + Amount, MaxMana);
}

void ULivingWorldAttributeComponent::SetEquipmentBonuses(float InBonusAttack, float InBonusDefense, float InBonusMaxHealth) {
	BonusAttack = InBonusAttack;
	BonusDefense = InBonusDefense;
	BonusMaxHealth = InBonusMaxHealth;
	Health = FMath::Min(Health, GetTotalMaxHealth());
}
