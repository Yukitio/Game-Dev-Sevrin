#pragma once

#include "CoreMinimal.h"
#include "LivingWorldSpriteCharacterBase.h"
#include "LivingWorldNpcCharacter.generated.h"

class ULivingWorldDialogueNode;

// An autonomous NPC bound to one entry in the simulation. Set NpcId to one of the
// ids registered in Sim/World.cpp's BuildTown() (maren/sela/doran/ilya/corwin) --
// typically by GameMode at spawn time, see LivingWorldGameMode.cpp.
//
// Two behaviors layer on top of the simulation-driven activity state:
//  - Routine movement: walks to wherever the simulation says it currently is,
//    via ALivingWorldLocationMarker/ULivingWorldLocationRegistrySubsystem. If
//    no marker exists for the current location yet, it just doesn't move --
//    safe before level content exists.
//  - Combat: hostile NPCs (bIsHostile) scan for and engage non-hostile targets
//    (the player, or non-hostile NPCs) within AggroRange. Non-hostile NPCs
//    never initiate combat.
UCLASS()
class LIVINGWORLDRPG_API ALivingWorldNpcCharacter : public ALivingWorldSpriteCharacterBase
{
	GENERATED_BODY()

public:
	ALivingWorldNpcCharacter();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Living World")
	FString NpcId;

	// Set on enemies/bosses. Hostile NPCs attack non-hostile targets within
	// AggroRange; non-hostile NPCs (the default) never initiate combat.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Living World|Combat")
	bool bIsHostile = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Living World|Combat")
	float AggroRange = 800.0f;

	// How often (seconds) this NPC re-evaluates movement/combat. Doesn't need
	// to run every frame.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Living World")
	float AIThinkInterval = 0.5f;

	// The first dialogue line shown when the player interacts with this NPC.
	// Leave null for NPCs that can't be talked to -- ALivingWorldPlayerCharacter::
	// TryInteract skips any NPC without one.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Living World|Dialogue")
	TObjectPtr<ULivingWorldDialogueNode> DialogueRoot;

protected:
	virtual void BeginPlay() override;

private:
	void UpdateRoutineMovement(const FString& LocationId);
	AActor* FindNearestCombatTarget() const;
	void UpdateCombatBehavior();

	float TimeSinceLastThink = 0.0f;
	bool bEngagedInCombat = false;
};
