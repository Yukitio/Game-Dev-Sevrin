#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LivingWorldDialogue.generated.h"

// A dialogue choice can directly trigger one of the simulation's Prototype 3
// intervention calls -- letting a conversation choice actually change the
// living world, not just branch text.
UENUM(BlueprintType)
enum class ELivingWorldDialogueInterventionType : uint8 {
	None,
	Encourage,
	Discourage,
	Assist
};

USTRUCT(BlueprintType)
struct FLivingWorldDialogueChoice {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText ChoiceText;

	// Leave unset to end the conversation after this choice.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<class ULivingWorldDialogueNode> NextNode = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Intervention")
	ELivingWorldDialogueInterventionType InterventionType = ELivingWorldDialogueInterventionType::None;

	// Encourage/Discourage: the other NPC's id -- the speaking NPC is the
	// implicit first party. Unused for Assist.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Intervention")
	FString InterventionOtherNpcId;

	// Assist only: one of hunger, energy, social, money.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Intervention")
	FString InterventionNeedName;

	// Assist only.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Intervention")
	float InterventionAmount = 20.0f;
};

// One line of dialogue plus the choices branching from it. Create instances
// as Data Assets in the Content Browser, one per line, and link them via
// Choices/NextNode to build a conversation tree -- no C++ needed per line.
UCLASS(BlueprintType)
class LIVINGWORLDRPG_API ULivingWorldDialogueNode : public UPrimaryDataAsset {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText LineText;

	// Leave empty to auto-use the speaking NPC's own name from the simulation.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText SpeakerNameOverride;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FLivingWorldDialogueChoice> Choices;
};
