#include "LivingWorldEquipmentComponent.h"

#include "GameFramework/Actor.h"
#include "LivingWorldAttributeComponent.h"

void ULivingWorldEquipmentComponent::BeginPlay() {
	Super::BeginPlay();
	if (AActor* Owner = GetOwner()) {
		CachedAttributes = Owner->FindComponentByClass<ULivingWorldAttributeComponent>();
	}
	RecalculateModifiers();
}

void ULivingWorldEquipmentComponent::EquipItem(ULivingWorldEquippableItem* Item) {
	if (!Item) {
		return;
	}
	EquippedItems.Add(Item->Slot, Item);
	RecalculateModifiers();
}

void ULivingWorldEquipmentComponent::UnequipSlot(ELivingWorldEquipmentSlot Slot) {
	if (EquippedItems.Remove(Slot) > 0) {
		RecalculateModifiers();
	}
}

ULivingWorldEquippableItem* ULivingWorldEquipmentComponent::GetEquipped(ELivingWorldEquipmentSlot Slot) const {
	if (const TObjectPtr<ULivingWorldEquippableItem>* Found = EquippedItems.Find(Slot)) {
		return *Found;
	}
	return nullptr;
}

void ULivingWorldEquipmentComponent::RecalculateModifiers() {
	if (!CachedAttributes) {
		return;
	}

	float TotalAttack = 0.0f;
	float TotalDefense = 0.0f;
	float TotalMaxHealth = 0.0f;
	for (const auto& Pair : EquippedItems) {
		if (const ULivingWorldEquippableItem* Item = Pair.Value) {
			TotalAttack += Item->AttackModifier;
			TotalDefense += Item->DefenseModifier;
			TotalMaxHealth += Item->MaxHealthModifier;
		}
	}

	CachedAttributes->SetEquipmentBonuses(TotalAttack, TotalDefense, TotalMaxHealth);
}
