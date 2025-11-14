#include "GM_RaceManager.h"
#include "GS_RaceState.h"
#include "PS_PlayerState.h"
#include "TimerManager.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

// the game mode manages the race works with the game and player states to control

void AGM_RaceManager::BeginPlay()
{
	Super::BeginPlay();

	// Gather all PlayerStart actors in the level
	TArray<AActor*> FoundStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), FoundStarts);

	AllPlayerStarts.Empty();
	AvailablePlayerStarts.Empty();

	for (AActor* Actor : FoundStarts)
	{
		if (APlayerStart* PS = Cast<APlayerStart>(Actor))
		{
			AllPlayerStarts.Add(PS);
			AvailablePlayerStarts.Add(PS);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("RaceManager: Found %d PlayerStarts"), AllPlayerStarts.Num());
}

AActor* AGM_RaceManager::ChoosePlayerStart_Implementation(AController* Player)
{
	// GameMode only exists on the server, so no extra HasAuthority check needed here
	APlayerStart* Chosen = GetFreePlayerStart(Player);
	if (!Chosen)
	{
		UE_LOG(LogTemp, Warning, TEXT("RaceManager: No free PlayerStarts, falling back to Super::ChoosePlayerStart_Implementation"));
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	UE_LOG(LogTemp, Log, TEXT("RaceManager: Chose PlayerStart %s for %s"),
		*Chosen->GetName(),
		*GetNameSafe(Player));

	return Chosen;
}

APlayerStart* AGM_RaceManager::GetFreePlayerStart(AController* Player)
{
	// If we've run out of free starts, you can either:
	// - reuse them, or
	// - just give up and let Super handle it
	// Here we choose to reuse them.
	if (AvailablePlayerStarts.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("RaceManager: AvailablePlayerStarts empty, reusing AllPlayerStarts"));
		AvailablePlayerStarts = AllPlayerStarts;
		PlayerToStartMap.Empty();
	}

	if (AvailablePlayerStarts.Num() == 0)
	{
		return nullptr;
	}

	const int32 Index = FMath::RandRange(0, AvailablePlayerStarts.Num() - 1);
	APlayerStart* Chosen = AvailablePlayerStarts[Index];

	// Remove from the available list so no other player can spawn here this cycle
	AvailablePlayerStarts.RemoveAtSwap(Index, 1, false);

	// Track which start this player received (optional, but handy for debugging / freeing later)
	PlayerToStartMap.Add(Player, Chosen);

	return Chosen;
}

void AGM_RaceManager::StartRaceWithCountdown(float Seconds)
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();

		bCourtesyStarted = false;

		RS->TotalLaps = Config_TotalLaps;
		RS->bRaceFinished = false;
		RS->bRaceRunning = false;
		RS->CountdownEndServerTime = Now + Seconds;
		RS->RaceStartServerTime = 0.0;
		RS->RaceEndServerTime = 0.0;

		RS->NotifyCountdownStarted();
		RS->OnRep_Countdown();

		GetWorldTimerManager().ClearTimer(TH_CountdownDone);
		GetWorldTimerManager().SetTimer(
			TH_CountdownDone,
			this,
			&AGM_RaceManager::HandleCountdownFinished,
			Seconds,
			false
		);

		GetWorldTimerManager().ClearTimer(TH_CourtesyEnd);
	}
}

void AGM_RaceManager::HandleCountdownFinished()
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();

		RS->CountdownEndServerTime = 0.0;
		RS->RaceStartServerTime = Now;
		RS->bRaceRunning = true;
		RS->bRaceFinished = false;

		OnStarted.Broadcast();

		RS->OnRep_RaceFlags();
	}
}

void AGM_RaceManager::NotifyPlayerFinished(APS_PlayerState* FinishedPlayer)
{
	if (!HasAuthority() || !FinishedPlayer) return;

	if (!bCourtesyStarted)
	{
		bCourtesyStarted = true;

		ForceFinishRace(Config_CourtesySeconds);
	}
}

void AGM_RaceManager::ForceFinishRace(float CourtesySeconds)
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		RS->bRaceRunning = false;
		RS->bRaceFinished = false;

		GetWorldTimerManager().ClearTimer(TH_CourtesyEnd);
		if (CourtesySeconds > 0.f)
		{
			GetWorldTimerManager().SetTimer(
				TH_CourtesyEnd,
				this,
				&AGM_RaceManager::HandleCourtesyEnd,
				CourtesySeconds,
				false
			);
		}
		else
		{
			HandleCourtesyEnd();
		}
	}
}

void AGM_RaceManager::HandleCourtesyEnd()
{
	if (!HasAuthority()) return;

	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		const double Now = RS->GetServerWorldTimeSeconds();

		RS->RaceEndServerTime = Now;
		RS->bRaceRunning = false;
		RS->bRaceFinished = true;

		OnFinished.Broadcast();
		RS->OnRep_RaceFlags();
	}
}

void AGM_RaceManager::RestartRace()
{
	if (!HasAuthority()) return;

	GetWorldTimerManager().ClearTimer(TH_CountdownDone);
	GetWorldTimerManager().ClearTimer(TH_CourtesyEnd);
	bCourtesyStarted = false;

	// Reset race flags
	if (AGS_RaceState* RS = GetGameState<AGS_RaceState>())
	{
		RS->bRaceRunning = false;
		RS->bRaceFinished = false;
		RS->CountdownEndServerTime = 0.0;
		RS->RaceStartServerTime = 0.0;
		RS->RaceEndServerTime = 0.0;
	}

	// Reset spawn data so next race starts with all PlayerStarts free again
	AvailablePlayerStarts = AllPlayerStarts;
	PlayerToStartMap.Empty();
}
