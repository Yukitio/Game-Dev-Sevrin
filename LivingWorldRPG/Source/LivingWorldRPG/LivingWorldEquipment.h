#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LivingWorldEquipment.generated.h"

UENUM(BlueprintType)
enum class ELivingWorldEquipmentSlot : uint8 {
	Weapon,
	Armor,
	Accessory
};

// A piece of equipment as data, not code -- create one of these as a Data Asset
// in the Content Browser (right-click > Miscellaneous > Data Asset > this class)
// per item, no C++ or Blueprint required. This is the pattern the character and
// world "mass production" templates will build on: define the shape once in
// C++, then let content scale as data.
UCLASS(BlueprintType)
class LIVINGWORLDRPG_API ULivingWorldEquippableItem : public UPrimaryDataAsset {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	ELivingWorldEquipmentSlot Slot = ELivingWorldEquipmentSlot::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float AttackModifier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float DefenseModifier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float MaxHealthModifier = 0.0f;
};
