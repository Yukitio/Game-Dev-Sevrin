#pragma once

#include "CoreMinimal.h"
#include "LivingWorldSpriteCharacterBase.h"
#include "LivingWorldPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

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

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Living World|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Living World|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
};
