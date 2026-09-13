#include "LivingWorldNpcCharacter.h"

#include "AIController.h"
#include "CollisionShape.h"
#include "Engine/GameInstance.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "LivingWorldAttributeComponent.h"
#include "LivingWorldCombatComponent.h"
#include "LivingWorldLocationRegistrySubsystem.h"
#include "LivingWorldSimSubsystem.h"

ALivingWorldNpcCharacter::ALivingWorldNpcCharacter() {
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ALivingWorldNpcCharacter::BeginPlay() {
	Super::BeginPlay();
	SetActivityState(TEXT("Idle"));
}

void ALivingWorldNpcCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	FString CurrentLocationId;
	bool bHasSnapshot = false;

	if (!NpcId.IsEmpty()) {
		if (UGameInstance* Instance = GetGameInstance()) {
			if (ULivingWorldSimSubsystem* SimSubsystem = Instance->GetSubsystem<ULivingWorldSimSubsystem>()) {
				const FLivingWorldNpcSnapshot Snapshot = SimSubsystem->GetNpcSnapshot(NpcId);
				SetActivityState(Snapshot.CurrentActivity);
				CurrentLocationId = Snapshot.CurrentLocationId;
				bHasSnapshot = true;
			}
		}
	}

	TimeSinceLastThink += DeltaSeconds;
	if (TimeSinceLastThink < AIThinkInterval) {
		return;
	}
	TimeSinceLastThink = 0.0f;

	UpdateCombatBehavior();
	if (!bEngagedInCombat && bHasSnapshot) {
		UpdateRoutineMovement(CurrentLocationId);
	}
}

void ALivingWorldNpcCharacter::UpdateRoutineMovement(const FString& LocationId) {
	if (LocationId.IsEmpty()) {
		return;
	}

	UWorld* World = GetWorld();
	ULivingWorldLocationRegistrySubsystem* Registry = World ? World->GetSubsystem<ULivingWorldLocationRegistrySubsystem>() : nullptr;
	if (!Registry) {
		return;
	}

	FVector TargetLocation;
	if (!Registry->GetLocation(LocationId, TargetLocation)) {
		return;
	}

	constexpr float kArrivalThreshold = 150.0f;
	if (FVector::DistSquared(GetActorLocation(), TargetLocation) < kArrivalThreshold * kArrivalThreshold) {
		return;
	}

	if (AAIController* AIController = Cast<AAIController>(GetController())) {
		AIController->MoveToLocation(TargetLocation, kArrivalThreshold * 0.5f);
	}
}

AActor* ALivingWorldNpcCharacter::FindNearestCombatTarget() const {
	UWorld* World = GetWorld();
	if (!World) {
		return nullptr;
	}

	TArray<FOverlapResult> Overlaps;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(AggroRange);
	World->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere);

	AActor* Nearest = nullptr;
	float NearestDistSq = 0.0f;

	for (const FOverlapResult& Overlap : Overlaps) {
		AActor* Other = Overlap.GetActor();
		if (!Other || Other == this) {
			continue;
		}

		const ALivingWorldNpcCharacter* OtherNpc = Cast<ALivingWorldNpcCharacter>(Other);
		if (OtherNpc && OtherNpc->bIsHostile) {
			continue;
		}

		const ULivingWorldAttributeComponent* OtherAttributes = Other->FindComponentByClass<ULivingWorldAttributeComponent>();
		if (!OtherAttributes || !OtherAttributes->IsAlive()) {
			continue;
		}

		const float DistSq = FVector::DistSquared(GetActorLocation(), Other->GetActorLocation());
		if (!Nearest || DistSq < NearestDistSq) {
			NearestDistSq = DistSq;
			Nearest = Other;
		}
	}

	return Nearest;
}

void ALivingWorldNpcCharacter::UpdateCombatBehavior() {
	if (!bIsHostile) {
		bEngagedInCombat = false;
		return;
	}

	AActor* Target = FindNearestCombatTarget();
	bEngagedInCombat = Target != nullptr;
	if (!Target) {
		return;
	}

	const float DistToTarget = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (CombatComponent && DistToTarget <= CombatComponent->AttackRange) {
		CombatComponent->TryMeleeAttack();
	} else if (AAIController* AIController = Cast<AAIController>(GetController())) {
		AIController->MoveToActor(Target, CombatComponent ? CombatComponent->AttackRange * 0.8f : 100.0f);
	}
}
