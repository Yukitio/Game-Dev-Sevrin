#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingWorldItem.h"
#include "LivingWorldInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLivingWorldOnInventoryChanged);

USTRUCT(BlueprintType)
struct FLivingWorldInventorySlot {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<ULivingWorldItem> Item = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Quantity = 0;
};

// A flat list of item stacks. Stackable items accumulate into one slot (up to
// MaxStackSize); non-stackable items always occupy their own slot.
UCLASS(ClassGroup = (LivingWorld), meta = (BlueprintSpawnableComponent))
class LIVINGWORLDRPG_API ULivingWorldInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FLivingWorldOnInventoryChanged OnInventoryChanged;

	// Returns how many were actually added (may be less than requested if a
	// stack limit was hit and no further slots were opened).
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(ULivingWorldItem* Item, int32 Quantity = 1);

	// Returns true only if the full requested quantity was removed.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(ULivingWorldItem* Item, int32 Quantity = 1);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetQuantity(const ULivingWorldItem* Item) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(const ULivingWorldItem* Item, int32 Quantity = 1) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FLivingWorldInventorySlot>& GetSlots() const { return Slots; }

private:
	UPROPERTY()
	TArray<FLivingWorldInventorySlot> Slots;
};
