#include "LivingWorldGameMode.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "LivingWorldNpcCharacter.h"
#include "LivingWorldPlayerCharacter.h"
#include "LivingWorldSimSubsystem.h"

ALivingWorldGameMode::ALivingWorldGameMode() {
	DefaultPawnClass = ALivingWorldPlayerCharacter::StaticClass();
	NpcCharacterClass = ALivingWorldNpcCharacter::StaticClass();
}

void ALivingWorldGameMode::BeginPlay() {
	Super::BeginPlay();

	if (!NpcCharacterClass) {
		return;
	}

	UGameInstance* Instance = GetGameInstance();
	ULivingWorldSimSubsystem* SimSubsystem =
		Instance ? Instance->GetSubsystem<ULivingWorldSimSubsystem>() : nullptr;
	if (!SimSubsystem) {
		return;
	}

	const TArray<FString> NpcIds = SimSubsystem->GetNpcIds();
	for (int32 Index = 0; Index < NpcIds.Num(); ++Index) {
		const FVector SpawnLocation(Index * NpcSpawnSpacing, 300.0f, 100.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (ALivingWorldNpcCharacter* Npc = GetWorld()->SpawnActor<ALivingWorldNpcCharacter>(
				NpcCharacterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams)) {
			Npc->NpcId = NpcIds[Index];
		}
	}
}
