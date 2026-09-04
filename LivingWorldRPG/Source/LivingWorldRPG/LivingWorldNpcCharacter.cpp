#include "LivingWorldNpcCharacter.h"

#include "Engine/GameInstance.h"
#include "LivingWorldSimSubsystem.h"

void ALivingWorldNpcCharacter::BeginPlay() {
	Super::BeginPlay();
	SetActivityState(TEXT("Idle"));
}

void ALivingWorldNpcCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (NpcId.IsEmpty()) {
		return;
	}

	UGameInstance* Instance = GetGameInstance();
	if (ULivingWorldSimSubsystem* SimSubsystem =
			Instance ? Instance->GetSubsystem<ULivingWorldSimSubsystem>() : nullptr) {
		SetActivityState(SimSubsystem->GetNpcSnapshot(NpcId).CurrentActivity);
	}
}
