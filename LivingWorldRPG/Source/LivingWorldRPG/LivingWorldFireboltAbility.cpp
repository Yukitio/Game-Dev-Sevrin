#include "LivingWorldFireboltAbility.h"

#include "GameFramework/Actor.h"
#include "LivingWorldAttributeComponent.h"

ULivingWorldFireboltAbility::ULivingWorldFireboltAbility() {
	DisplayName = FText::FromString(TEXT("Firebolt"));
	ManaCost = 15.0f;
	Cooldown = 2.0f;
}

void ULivingWorldFireboltAbility::Activate_Implementation(AActor* Caster, AActor* Target) {
	if (!Target) {
		return;
	}
	if (ULivingWorldAttributeComponent* TargetAttributes = Target->FindComponentByClass<ULivingWorldAttributeComponent>()) {
		TargetAttributes->ApplyDamage(Damage, Caster);
	}
}
