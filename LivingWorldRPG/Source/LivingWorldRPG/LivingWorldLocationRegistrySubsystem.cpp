#include "LivingWorldLocationRegistrySubsystem.h"

#include "EngineUtils.h"
#include "LivingWorldLocationMarker.h"

void ULivingWorldLocationRegistrySubsystem::OnWorldBeginPlay(UWorld& InWorld) {
	Super::OnWorldBeginPlay(InWorld);

	LocationsById.Empty();
	for (TActorIterator<ALivingWorldLocationMarker> It(&InWorld); It; ++It) {
		ALivingWorldLocationMarker* Marker = *It;
		if (Marker && !Marker->LocationId.IsEmpty()) {
			LocationsById.Add(Marker->LocationId, Marker->GetActorLocation());
		}
	}
}

bool ULivingWorldLocationRegistrySubsystem::GetLocation(const FString& LocationId, FVector& OutLocation) const {
	if (const FVector* Found = LocationsById.Find(LocationId)) {
		OutLocation = *Found;
		return true;
	}
	return false;
}
