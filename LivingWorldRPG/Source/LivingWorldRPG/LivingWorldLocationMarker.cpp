#include "LivingWorldLocationMarker.h"

#include "Components/SceneComponent.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

ALivingWorldLocationMarker::ALivingWorldLocationMarker() {
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

#if WITH_EDITORONLY_DATA
	EditorBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorBillboard"));
	EditorBillboard->SetupAttachment(RootComponent);
#endif
}
