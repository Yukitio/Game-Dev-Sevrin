#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingWorldDialogueComponent.generated.h"

class ULivingWorldDialogueNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FLivingWorldOnDialogueLine, FText, SpeakerName, FText, LineText, const TArray<FText>&, ChoiceTexts);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLivingWorldOnDialogueEnded);

// Drives conversation state (current line, available choices) and applies a
// choice's intervention hook, if any, to the simulation. Deliberately has no
// on-screen presentation -- bind OnDialogueLine/OnDialogueEnded to whatever
// UI gets built later (UMG, or just printing to screen for now).
UCLASS(ClassGroup = (LivingWorld), meta = (BlueprintSpawnableComponent))
class LIVINGWORLDRPG_API ULivingWorldDialogueComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FLivingWorldOnDialogueLine OnDialogueLine;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FLivingWorldOnDialogueEnded OnDialogueEnded;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(ULivingWorldDialogueNode* StartNode, AActor* Speaker);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SelectChoice(int32 ChoiceIndex);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool IsInDialogue() const { return CurrentNode != nullptr; }

private:
	void ShowCurrentNode();

	UPROPERTY()
	TObjectPtr<ULivingWorldDialogueNode> CurrentNode;

	UPROPERTY()
	TObjectPtr<AActor> CurrentSpeaker;

	FString SpeakerNpcId;
};
