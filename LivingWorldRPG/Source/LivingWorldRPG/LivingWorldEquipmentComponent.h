#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingWorldEquipment.h"
#include "LivingWorldEquipmentComponent.generated.h"

class ULivingWorldAttributeComponent;
class ULivingWorldInventoryComponent;

// Equips/unequips items and keeps the owner's AttributeComponent bonus layer in
// sync with whatever's currently worn. Requires a ULivingWorldAttributeComponent
// on the same actor. EquipItem/UnequipSlot work directly (ignoring inventory,
// useful for e.g. a boss spawned already armed); EquipFromInventory/
// UnequipToInventory move the item to/from the actor's InventoryComponent if
// one exists, which is what player-facing UI should call.
UCLASS(ClassGroup = (LivingWorld), meta = (BlueprintSpawnableComponent))
class LIVINGWORLDRPG_API ULivingWorldEquipmentComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipItem(ULivingWorldEquippableItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipSlot(ELivingWorldEquipmentSlot Slot);

	// Returns false if the owner has no InventoryComponent or doesn't have the item.
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipFromInventory(ULivingWorldEquippableItem* Item);

	// Returns false if the owner has no InventoryComponent or the slot was empty.
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipToInventory(ELivingWorldEquipmentSlot Slot);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	ULivingWorldEquippableItem* GetEquipped(ELivingWorldEquipmentSlot Slot) const;

protected:
	virtual void BeginPlay() override;

private:
	void RecalculateModifiers();

	UPROPERTY()
	TMap<ELivingWorldEquipmentSlot, TObjectPtr<ULivingWorldEquippableItem>> EquippedItems;

	UPROPERTY()
	TObjectPtr<ULivingWorldAttributeComponent> CachedAttributes;
};
