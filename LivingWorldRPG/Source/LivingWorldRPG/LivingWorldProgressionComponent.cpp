#include "LivingWorldProgressionComponent.h"

#include "GameFramework/Actor.h"
#include "LivingWorldAttributeComponent.h"

void ULivingWorldProgressionComponent::GainExperience(float Amount) {
	if (Amount <= 0.0f) {
		return;
	}
	Experience += Amount;
	while (Experience >= ExperienceToNextLevel) {
		Experience -= ExperienceToNextLevel;
		LevelUp();
	}
}

void ULivingWorldProgressionComponent::LevelUp() {
	Level++;
	ExperienceToNextLevel *= ExperienceGrowthRate;

	if (AActor* Owner = GetOwner()) {
		if (ULivingWorldAttributeComponent* Attributes = Owner->FindComponentByClass<ULivingWorldAttributeComponent>()) {
			Attributes->AttackPower += AttackGrowthPerLevel;
			Attributes->Defense += DefenseGrowthPerLevel;
			Attributes->MaxHealth += MaxHealthGrowthPerLevel;
			Attributes->Heal(MaxHealthGrowthPerLevel);
		}
	}

	OnLevelUp.Broadcast(Level);
}
