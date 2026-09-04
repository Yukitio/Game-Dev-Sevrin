#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingWorldEquipment.h"
#include "LivingWorldEquipmentComponent.generated.h"

class ULivingWorldAttributeComponent;

// Equips/unequips items and keeps the owner's AttributeComponent bonus layer in
// sync with whatever's currently worn. Requires a ULivingWorldAttributeComponent
// on the same actor.
UCLASS(ClassGroup = (LivingWorld), meta = (BlueprintSpawnableComponent))
class LIVINGWORLDRPG_API ULivingWorldEquipmentComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipItem(ULivingWorldEquippableItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipSlot(ELivingWorldEquipmentSlot Slot);

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
