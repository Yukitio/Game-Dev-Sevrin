#include "LivingWorldEquipmentComponent.h"

#include "GameFramework/Actor.h"
#include "LivingWorldAttributeComponent.h"
#include "LivingWorldInventoryComponent.h"

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

bool ULivingWorldEquipmentComponent::EquipFromInventory(ULivingWorldEquippableItem* Item) {
	if (!Item) {
		return false;
	}
	AActor* Owner = GetOwner();
	ULivingWorldInventoryComponent* Inventory = Owner ? Owner->FindComponentByClass<ULivingWorldInventoryComponent>() : nullptr;
	if (!Inventory || !Inventory->HasItem(Item, 1)) {
		return false;
	}

	UnequipToInventory(Item->Slot);

	if (!Inventory->RemoveItem(Item, 1)) {
		return false;
	}
	EquipItem(Item);
	return true;
}

bool ULivingWorldEquipmentComponent::UnequipToInventory(ELivingWorldEquipmentSlot Slot) {
	AActor* Owner = GetOwner();
	ULivingWorldInventoryComponent* Inventory = Owner ? Owner->FindComponentByClass<ULivingWorldInventoryComponent>() : nullptr;
	ULivingWorldEquippableItem* Current = GetEquipped(Slot);
	if (!Inventory || !Current) {
		return false;
	}

	UnequipSlot(Slot);
	Inventory->AddItem(Current, 1);
	return true;
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
