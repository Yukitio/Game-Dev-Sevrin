#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/UniquePtr.h"
#include "TimerManager.h"
#include "Sim/Simulation.h"
#include "LivingWorldSimSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FLivingWorldGoalSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	FString GoalType;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	float Progress = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	bool bCompleted = false;
};

USTRUCT(BlueprintType)
struct FLivingWorldNpcSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	FString NpcId;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	FString Occupation;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	FString CurrentActivity;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	FString CurrentLocationName;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	float Hunger = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	float Energy = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	float Social = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	float Money = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Living World")
	TArray<FLivingWorldGoalSnapshot> Goals;
};

// Owns the engine-agnostic sim::Simulation (see Sim/) for the lifetime of the game
// instance, advances it on a timer, and exposes it to Blueprint/UI/gameplay code.
// This is the whole porting boundary described in prototype/npc-life-sim/README.md --
// everything under Sim/ is unmodified from the standalone console prototype.
UCLASS(Config = Game)
class LIVINGWORLDRPG_API ULivingWorldSimSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Declared (not defaulted here) because Sim is a TUniquePtr to a forward-declared
	// type -- the implicit destructor must be instantiated where sim::Simulation is
	// complete (LivingWorldSimSubsystem.cpp), not wherever this header is included.
	virtual ~ULivingWorldSimSubsystem() override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Living World")
	TArray<FString> GetNpcIds() const;

	UFUNCTION(BlueprintCallable, Category = "Living World")
	FLivingWorldNpcSnapshot GetNpcSnapshot(const FString& NpcId) const;

	// Returns the last Count entries of the simulation's narrative event log
	// (relationship changes, goal completions, calendar events, player interventions).
	UFUNCTION(BlueprintCallable, Category = "Living World")
	TArray<FString> GetRecentEvents(int32 Count) const;

	UFUNCTION(BlueprintCallable, Category = "Living World")
	FString Intervene_Encourage(const FString& NpcAId, const FString& NpcBId);

	UFUNCTION(BlueprintCallable, Category = "Living World")
	FString Intervene_Discourage(const FString& NpcAId, const FString& NpcBId);

	// NeedName is one of: hunger, energy, social, money (case-insensitive).
	UFUNCTION(BlueprintCallable, Category = "Living World")
	FString Intervene_Assist(const FString& NpcId, const FString& NeedName, float Amount);

protected:
	// In-game hours advanced per real-world second. Tunable via DefaultGame.ini.
	UPROPERTY(Config, EditDefaultsOnly, Category = "Living World")
	float InGameHoursPerRealSecond = 1.0f;

private:
	void TickSimulation();

	TUniquePtr<sim::Simulation> Sim;
	FTimerHandle TickTimerHandle;
};
