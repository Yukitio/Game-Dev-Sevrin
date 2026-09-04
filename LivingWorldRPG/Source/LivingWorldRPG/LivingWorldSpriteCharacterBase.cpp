#include "LivingWorldSpriteCharacterBase.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LivingWorldAbilityComponent.h"
#include "LivingWorldAttributeComponent.h"
#include "LivingWorldCombatComponent.h"
#include "LivingWorldEquipmentComponent.h"
#include "LivingWorldProgressionComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

ALivingWorldSpriteCharacterBase::ALivingWorldSpriteCharacterBase() {
	PrimaryActorTick.bCanEverTick = true;

	SpriteComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteComponent"));
	SpriteComponent->SetupAttachment(RootComponent);
	SpriteComponent->SetCastShadow(true);

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 88.0f);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	AttributeComponent = CreateDefaultSubobject<ULivingWorldAttributeComponent>(TEXT("AttributeComponent"));
	EquipmentComponent = CreateDefaultSubobject<ULivingWorldEquipmentComponent>(TEXT("EquipmentComponent"));
	CombatComponent = CreateDefaultSubobject<ULivingWorldCombatComponent>(TEXT("CombatComponent"));
	ProgressionComponent = CreateDefaultSubobject<ULivingWorldProgressionComponent>(TEXT("ProgressionComponent"));
	AbilityComponent = CreateDefaultSubobject<ULivingWorldAbilityComponent>(TEXT("AbilityComponent"));
}

void ALivingWorldSpriteCharacterBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	FaceActiveCamera();
}

void ALivingWorldSpriteCharacterBase::SetActivityState(const FString& StateName) {
	if (CurrentState == StateName) {
		return;
	}
	CurrentState = StateName;

	if (!SpriteComponent) {
		return;
	}

	if (const TObjectPtr<UPaperFlipbook>* Flipbook = ActivityFlipbooks.Find(StateName)) {
		SpriteComponent->SetFlipbook(*Flipbook);
		return;
	}
	if (const TObjectPtr<UPaperFlipbook>* IdleFlipbook = ActivityFlipbooks.Find(TEXT("Idle"))) {
		SpriteComponent->SetFlipbook(*IdleFlipbook);
	}
}

void ALivingWorldSpriteCharacterBase::FaceActiveCamera() {
	if (!SpriteComponent) {
		return;
	}

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager) {
		return;
	}

	const FVector ToCamera = CameraManager->GetCameraLocation() - SpriteComponent->GetComponentLocation();
	if (ToCamera.IsNearlyZero()) {
		return;
	}

	FRotator FacingRotation = ToCamera.Rotation();
	FacingRotation.Pitch = 0.0f;
	FacingRotation.Roll = 0.0f;
	SpriteComponent->SetWorldRotation(FacingRotation);

	const FVector Velocity = GetVelocity();
	if (!Velocity.IsNearlyZero()) {
		const FVector Right = FRotationMatrix(FacingRotation).GetUnitAxis(EAxis::Y);
		const float Side = FVector::DotProduct(Velocity, Right);
		const float FlipScale = Side < 0.0f ? -1.0f : 1.0f;
		FVector Scale = SpriteComponent->GetRelativeScale3D();
		Scale.Y = FMath::Abs(Scale.Y) * FlipScale;
		SpriteComponent->SetRelativeScale3D(Scale);
	}
}
