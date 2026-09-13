#pragma once

#include "CoreMinimal.h"
#include "LivingWorldItem.h"
#include "LivingWorldEquipment.generated.h"

UENUM(BlueprintType)
enum class ELivingWorldEquipmentSlot : uint8 {
	Weapon,
	Armor,
	Accessory
};

// A piece of equipment -- adds stat modifiers and a slot on top of the base
// item. Still just a Data Asset instance per item, no code needed per sword.
UCLASS(BlueprintType)
class LIVINGWORLDRPG_API ULivingWorldEquippableItem : public ULivingWorldItem {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	ELivingWorldEquipmentSlot Slot = ELivingWorldEquipmentSlot::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float AttackModifier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float DefenseModifier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float MaxHealthModifier = 0.0f;
};
