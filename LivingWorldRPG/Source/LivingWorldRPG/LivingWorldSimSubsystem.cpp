#include "LivingWorldSimSubsystem.h"

#include "Sim/World.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"

namespace {

bool ParseNeed(const FString& Name, sim::NeedType& OutNeed) {
	if (Name.Equals(TEXT("hunger"), ESearchCase::IgnoreCase)) { OutNeed = sim::NeedType::Hunger; return true; }
	if (Name.Equals(TEXT("energy"), ESearchCase::IgnoreCase)) { OutNeed = sim::NeedType::Energy; return true; }
	if (Name.Equals(TEXT("social"), ESearchCase::IgnoreCase)) { OutNeed = sim::NeedType::Social; return true; }
	if (Name.Equals(TEXT("money"), ESearchCase::IgnoreCase)) { OutNeed = sim::NeedType::Money; return true; }
	return false;
}

FLivingWorldNpcSnapshot MakeSnapshot(const sim::Simulation& Simulation, const sim::Npc& Npc) {
	FLivingWorldNpcSnapshot Snapshot;
	Snapshot.NpcId = UTF8_TO_TCHAR(Npc.id.c_str());
	Snapshot.DisplayName = UTF8_TO_TCHAR(Npc.name.c_str());
	Snapshot.Occupation = UTF8_TO_TCHAR(Npc.occupation.c_str());
	Snapshot.CurrentActivity = UTF8_TO_TCHAR(sim::ToString(Npc.currentActivity).c_str());
	Snapshot.CurrentLocationName = UTF8_TO_TCHAR(Simulation.LocationName(Npc.currentLocationId).c_str());
	Snapshot.Hunger = Npc.needs.Get(sim::NeedType::Hunger);
	Snapshot.Energy = Npc.needs.Get(sim::NeedType::Energy);
	Snapshot.Social = Npc.needs.Get(sim::NeedType::Social);
	Snapshot.Money = Npc.needs.Get(sim::NeedType::Money);
	for (const auto& Goal : Npc.goals) {
		FLivingWorldGoalSnapshot GoalSnapshot;
		GoalSnapshot.GoalType = UTF8_TO_TCHAR(sim::ToString(Goal.type).c_str());
		GoalSnapshot.Progress = Goal.progress;
		GoalSnapshot.bCompleted = Goal.completed;
		Snapshot.Goals.Add(GoalSnapshot);
	}
	return Snapshot;
}

} // namespace

ULivingWorldSimSubsystem::~ULivingWorldSimSubsystem() = default;

void ULivingWorldSimSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	Sim = MakeUnique<sim::Simulation>(1337u);
	sim::BuildTown(*Sim);

	if (InGameHoursPerRealSecond > 0.0f) {
		if (UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr) {
			const float TickInterval = 1.0f / InGameHoursPerRealSecond;
			World->GetTimerManager().SetTimer(TickTimerHandle, this,
				&ULivingWorldSimSubsystem::TickSimulation, TickInterval, true);
		}
	}
}

void ULivingWorldSimSubsystem::Deinitialize() {
	if (UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr) {
		World->GetTimerManager().ClearTimer(TickTimerHandle);
	}
	Sim.Reset();
	Super::Deinitialize();
}

void ULivingWorldSimSubsystem::TickSimulation() {
	if (Sim) {
		Sim->Tick();
	}
}

TArray<FString> ULivingWorldSimSubsystem::GetNpcIds() const {
	TArray<FString> Ids;
	if (Sim) {
		for (const auto& Npc : Sim->Npcs()) {
			Ids.Add(UTF8_TO_TCHAR(Npc.id.c_str()));
		}
	}
	return Ids;
}

FLivingWorldNpcSnapshot ULivingWorldSimSubsystem::GetNpcSnapshot(const FString& NpcId) const {
	if (Sim) {
		const std::string Id = TCHAR_TO_UTF8(*NpcId);
		for (const auto& Npc : Sim->Npcs()) {
			if (Npc.id == Id) {
				return MakeSnapshot(*Sim, Npc);
			}
		}
	}
	return FLivingWorldNpcSnapshot();
}

TArray<FString> ULivingWorldSimSubsystem::GetRecentEvents(int32 Count) const {
	TArray<FString> Events;
	if (Sim) {
		const auto& Log = Sim->EventLog();
		const int32 Start = FMath::Max(0, static_cast<int32>(Log.size()) - Count);
		for (int32 i = Start; i < static_cast<int32>(Log.size()); ++i) {
			Events.Add(UTF8_TO_TCHAR(Log[i].c_str()));
		}
	}
	return Events;
}

FString ULivingWorldSimSubsystem::Intervene_Encourage(const FString& NpcAId, const FString& NpcBId) {
	if (!Sim) return TEXT("Simulation not initialized.");
	return UTF8_TO_TCHAR(Sim->Intervene_Encourage(TCHAR_TO_UTF8(*NpcAId), TCHAR_TO_UTF8(*NpcBId)).c_str());
}

FString ULivingWorldSimSubsystem::Intervene_Discourage(const FString& NpcAId, const FString& NpcBId) {
	if (!Sim) return TEXT("Simulation not initialized.");
	return UTF8_TO_TCHAR(Sim->Intervene_Discourage(TCHAR_TO_UTF8(*NpcAId), TCHAR_TO_UTF8(*NpcBId)).c_str());
}

FString ULivingWorldSimSubsystem::Intervene_Assist(const FString& NpcId, const FString& NeedName, float Amount) {
	if (!Sim) return TEXT("Simulation not initialized.");
	sim::NeedType Need;
	if (!ParseNeed(NeedName, Need)) {
		return TEXT("Unknown need. Use hunger, energy, social, or money.");
	}
	return UTF8_TO_TCHAR(Sim->Intervene_Assist(TCHAR_TO_UTF8(*NpcId), Need, Amount).c_str());
}
