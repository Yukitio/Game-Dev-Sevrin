#include "LivingWorldCombatComponent.h"

#include "CollisionShape.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "LivingWorldAttributeComponent.h"

bool ULivingWorldCombatComponent::TryMeleeAttack() {
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!World || !Owner) {
		return false;
	}

	const float Now = World->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown) {
		return false;
	}

	ULivingWorldAttributeComponent* MyAttributes = Owner->FindComponentByClass<ULivingWorldAttributeComponent>();
	if (!MyAttributes || !MyAttributes->IsAlive()) {
		return false;
	}

	const FVector Start = Owner->GetActorLocation();
	const FVector End = Start + Owner->GetActorForwardVector() * AttackRange;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(AttackRadius);

	TArray<FHitResult> Hits;
	World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, Sphere);

	LastAttackTime = Now;

	bool bHitSomething = false;
	for (const FHitResult& Hit : Hits) {
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == Owner) {
			continue;
		}
		if (ULivingWorldAttributeComponent* TargetAttributes = HitActor->FindComponentByClass<ULivingWorldAttributeComponent>()) {
			TargetAttributes->ApplyDamage(MyAttributes->GetTotalAttack(), Owner);
			bHitSomething = true;
		}
	}

	return bHitSomething;
}
