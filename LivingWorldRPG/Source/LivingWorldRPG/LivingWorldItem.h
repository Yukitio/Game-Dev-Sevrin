#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LivingWorldItem.generated.h"

class UTexture2D;

// Base for anything that can sit in an inventory -- consumables, quest items,
// crafting materials, or (via ULivingWorldEquippableItem) equipment. Create
// instances as Data Assets in the Content Browser, one per item, no C++ or
// Blueprint required per item. Same template pattern the world-building and
// character mass-production tools will extend later.
UCLASS(BlueprintType)
class LIVINGWORLDRPG_API ULivingWorldItem : public UPrimaryDataAsset {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText Description;

	// Art insertion point: assign an icon texture once UI art exists. Safe to
	// leave unset -- nothing here requires it to compile or function.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool bStackable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1", EditCondition = "bStackable"))
	int32 MaxStackSize = 99;
};
