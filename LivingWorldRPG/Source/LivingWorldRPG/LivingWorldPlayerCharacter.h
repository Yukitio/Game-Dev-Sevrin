#pragma once

#include "CoreMinimal.h"
#include "LivingWorldSpriteCharacterBase.h"
#include "LivingWorldPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ULivingWorldDialogueComponent;

// The player's own 2D sprite character, moving freely through the 3D level.
// Input uses the classic Axis Mapping system (see Config/DefaultInput.ini) rather
// than Enhanced Input, since Enhanced Input's Input Actions/Mapping Contexts are
// editor-authored .uasset files that can't be checked in as plain text.
UCLASS()
class LIVINGWORLDRPG_API ALivingWorldPlayerCharacter : public ALivingWorldSpriteCharacterBase
{
	GENERATED_BODY()

public:
	ALivingWorldPlayerCharacter();

	// Finds the nearest NPC within InteractRange that has a DialogueRoot set and
	// starts a conversation via DialogueComponent. Bound to the "Interact" action
	// (E key, see Config/DefaultInput.ini). No-ops if already in dialogue or no
	// eligible NPC is nearby.
	UFUNCTION(BlueprintCallable, Category = "Living World|Dialogue")
	void TryInteract();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Living World|Dialogue")
	TObjectPtr<ULivingWorldDialogueComponent> DialogueComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Living World|Dialogue")
	float InteractRange = 250.0f;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Living World|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Living World|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void PerformAttack();
};
