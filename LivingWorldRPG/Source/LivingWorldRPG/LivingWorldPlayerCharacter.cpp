#include "LivingWorldPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LivingWorldCombatComponent.h"
#include "LivingWorldDialogueComponent.h"
#include "LivingWorldNpcCharacter.h"

ALivingWorldPlayerCharacter::ALivingWorldPlayerCharacter() {
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.0f;
	CameraBoom->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 6.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	DialogueComponent = CreateDefaultSubobject<ULivingWorldDialogueComponent>(TEXT("DialogueComponent"));
}

void ALivingWorldPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALivingWorldPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALivingWorldPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ALivingWorldPlayerCharacter::PerformAttack);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ALivingWorldPlayerCharacter::TryInteract);
}

void ALivingWorldPlayerCharacter::TryInteract() {
	if (!DialogueComponent || DialogueComponent->IsInDialogue()) {
		return;
	}

	UWorld* World = GetWorld();
	if (!World) {
		return;
	}

	ALivingWorldNpcCharacter* Nearest = nullptr;
	float NearestDistSq = 0.0f;

	for (TActorIterator<ALivingWorldNpcCharacter> It(World); It; ++It) {
		ALivingWorldNpcCharacter* Npc = *It;
		if (!Npc->DialogueRoot) {
			continue;
		}

		const float DistSq = FVector::DistSquared(GetActorLocation(), Npc->GetActorLocation());
		if (DistSq > InteractRange * InteractRange) {
			continue;
		}

		if (!Nearest || DistSq < NearestDistSq) {
			NearestDistSq = DistSq;
			Nearest = Npc;
		}
	}

	if (Nearest) {
		DialogueComponent->StartDialogue(Nearest->DialogueRoot, Nearest);
	}
}

void ALivingWorldPlayerCharacter::MoveForward(float Value) {
	if (Controller && Value != 0.0f) {
		const FRotator YawRotation(0.0f, CameraBoom->GetComponentRotation().Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ALivingWorldPlayerCharacter::MoveRight(float Value) {
	if (Controller && Value != 0.0f) {
		const FRotator YawRotation(0.0f, CameraBoom->GetComponentRotation().Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ALivingWorldPlayerCharacter::PerformAttack() {
	if (CombatComponent) {
		CombatComponent->TryMeleeAttack();
	}
}
