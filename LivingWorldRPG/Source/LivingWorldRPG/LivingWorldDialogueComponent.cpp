#include "LivingWorldDialogueComponent.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "LivingWorldDialogue.h"
#include "LivingWorldNpcCharacter.h"
#include "LivingWorldSimSubsystem.h"

void ULivingWorldDialogueComponent::StartDialogue(ULivingWorldDialogueNode* StartNode, AActor* Speaker) {
	CurrentNode = StartNode;
	CurrentSpeaker = Speaker;

	if (const ALivingWorldNpcCharacter* NpcSpeaker = Cast<ALivingWorldNpcCharacter>(Speaker)) {
		SpeakerNpcId = NpcSpeaker->NpcId;
	} else {
		SpeakerNpcId.Empty();
	}

	ShowCurrentNode();
}

void ULivingWorldDialogueComponent::ShowCurrentNode() {
	if (!CurrentNode) {
		EndDialogue();
		return;
	}

	FText SpeakerName = CurrentNode->SpeakerNameOverride;
	if (SpeakerName.IsEmpty() && !SpeakerNpcId.IsEmpty()) {
		if (UGameInstance* Instance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr) {
			if (ULivingWorldSimSubsystem* SimSubsystem = Instance->GetSubsystem<ULivingWorldSimSubsystem>()) {
				SpeakerName = FText::FromString(SimSubsystem->GetNpcSnapshot(SpeakerNpcId).DisplayName);
			}
		}
	}

	TArray<FText> ChoiceTexts;
	ChoiceTexts.Reserve(CurrentNode->Choices.Num());
	for (const FLivingWorldDialogueChoice& Choice : CurrentNode->Choices) {
		ChoiceTexts.Add(Choice.ChoiceText);
	}

	OnDialogueLine.Broadcast(SpeakerName, CurrentNode->LineText, ChoiceTexts);
}

void ULivingWorldDialogueComponent::SelectChoice(int32 ChoiceIndex) {
	if (!CurrentNode || !CurrentNode->Choices.IsValidIndex(ChoiceIndex)) {
		return;
	}

	const FLivingWorldDialogueChoice& Choice = CurrentNode->Choices[ChoiceIndex];

	if (Choice.InterventionType != ELivingWorldDialogueInterventionType::None && !SpeakerNpcId.IsEmpty()) {
		if (UGameInstance* Instance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr) {
			if (ULivingWorldSimSubsystem* SimSubsystem = Instance->GetSubsystem<ULivingWorldSimSubsystem>()) {
				switch (Choice.InterventionType) {
					case ELivingWorldDialogueInterventionType::Encourage:
						SimSubsystem->Intervene_Encourage(SpeakerNpcId, Choice.InterventionOtherNpcId);
						break;
					case ELivingWorldDialogueInterventionType::Discourage:
						SimSubsystem->Intervene_Discourage(SpeakerNpcId, Choice.InterventionOtherNpcId);
						break;
					case ELivingWorldDialogueInterventionType::Assist:
						SimSubsystem->Intervene_Assist(SpeakerNpcId, Choice.InterventionNeedName, Choice.InterventionAmount);
						break;
					default:
						break;
				}
			}
		}
	}

	CurrentNode = Choice.NextNode;
	ShowCurrentNode();
}

void ULivingWorldDialogueComponent::EndDialogue() {
	CurrentNode = nullptr;
	CurrentSpeaker = nullptr;
	SpeakerNpcId.Empty();
	OnDialogueEnded.Broadcast();
}
