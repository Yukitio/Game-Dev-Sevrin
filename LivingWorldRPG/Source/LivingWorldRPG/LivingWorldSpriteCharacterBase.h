#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LivingWorldSpriteCharacterBase.generated.h"

class UPaperFlipbookComponent;
class UPaperFlipbook;

// Shared base for every 2D sprite character (player and NPCs) moving through the
// full 3D level -- the "2D character in a 3D world" look. No art is required to
// compile or run this: with ActivityFlipbooks empty, the sprite is just invisible
// and the capsule still moves normally. Assign flipbooks in the Details panel (or
// a Blueprint subclass) once character art exists -- no C++ changes needed.
UCLASS(Abstract)
class LIVINGWORLDRPG_API ALivingWorldSpriteCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ALivingWorldSpriteCharacterBase();

	virtual void Tick(float DeltaSeconds) override;

	// Swaps the active flipbook to whatever is mapped to this state (e.g. "Working",
	// "Sleeping" -- see sim::ToString(ActivityType) in Sim/Types.cpp for the exact
	// strings NPCs will pass). Falls back to "Idle" if the state has no flipbook
	// assigned yet, so this is always safe to call before art exists.
	UFUNCTION(BlueprintCallable, Category = "Living World|Animation")
	void SetActivityState(const FString& StateName);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Living World")
	TObjectPtr<UPaperFlipbookComponent> SpriteComponent;

	// Art insertion point: one flipbook per activity state. Populate this in the
	// Details panel (or override per-Blueprint-subclass) once character art exists.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Living World|Animation")
	TMap<FString, TObjectPtr<UPaperFlipbook>> ActivityFlipbooks;

private:
	// Keeps the sprite facing the active camera around the yaw axis (so it always
	// reads as flat 2D from the player's view) and flips it horizontally based on
	// movement direction, while the capsule still moves freely in 3D. This is the
	// one piece of this scaffold that's unverified against Paper2D's actual default
	// sprite-plane orientation -- see the project README for what to check first.
	void FaceActiveCamera();

	FString CurrentState;
};
