#include "LivingWorldAbilityComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "LivingWorldAttributeComponent.h"

bool ULivingWorldAbilityComponent::TryActivateAbility(int32 AbilityIndex, AActor* Target) {
	if (!KnownAbilities.IsValidIndex(AbilityIndex)) {
		return false;
	}
	ULivingWorldAbility* Ability = KnownAbilities[AbilityIndex];
	if (!Ability) {
		return false;
	}

	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!World || !Owner) {
		return false;
	}

	ULivingWorldAttributeComponent* Attributes = Owner->FindComponentByClass<ULivingWorldAttributeComponent>();
	if (!Attributes || !Attributes->IsAlive()) {
		return false;
	}

	const float Now = World->GetTimeSeconds();
	if (const float* LastTime = LastActivationTimeByIndex.Find(AbilityIndex)) {
		if (Now - *LastTime < Ability->Cooldown) {
			return false;
		}
	}

	if (!Attributes->SpendMana(Ability->ManaCost)) {
		return false;
	}

	Ability->Activate(Owner, Target);
	LastActivationTimeByIndex.Add(AbilityIndex, Now);
	return true;
}
