// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_RaceState.h"
#include "PS_PlayerState.h"
#include "Net/UnrealNetwork.h"

AGS_RaceState::AGS_RaceState()
{
	bReplicates = true;
}

void AGS_RaceState::BeginPlay()
{
	Super::BeginPlay();
	bPrevRaceRunning = bRaceRunning;
	bPrevRaceFinished = bRaceFinished;
	bPrevCountdownActive = IsCountdownActive();
}

void AGS_RaceState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGS_RaceState, bRaceRunning);
	DOREPLIFETIME(AGS_RaceState, bRaceFinished);
	DOREPLIFETIME(AGS_RaceState, TotalLaps);
	DOREPLIFETIME(AGS_RaceState, CountdownEndServerTime);
	DOREPLIFETIME(AGS_RaceState, RaceStartServerTime);
	DOREPLIFETIME(AGS_RaceState, RaceEndServerTime);
	DOREPLIFETIME(AGS_RaceState, RaceResults);
	DOREPLIFETIME(AGS_RaceState, FastestLapTimeSeconds);
	DOREPLIFETIME(AGS_RaceState, FastestLapPlayerName);
}

void AGS_RaceState::OnRep_RaceFlags()
{
	if (!bPrevRaceRunning && bRaceRunning)
	{
		OnRaceStarted.Broadcast();
	}
	if (!bPrevRaceFinished && bRaceFinished)
	{
		OnRaceFinished.Broadcast();
	}

	bPrevRaceRunning = bRaceRunning;
	bPrevRaceFinished = bRaceFinished;
}

void AGS_RaceState::OnRep_Countdown()
{
	
	const bool bNowActive = IsCountdownActive();
	if (!bPrevCountdownActive && bNowActive)
	{
		OnCountdownStarted.Broadcast();
	}
		
	bPrevCountdownActive = bNowActive;
	
}

float AGS_RaceState::GetCountdownSecondsRemaining() const
{
	return FMath::Max(0.0, CountdownEndServerTime - GetServerWorldTimeSeconds());
}

void AGS_RaceState::NotifyCountdownStarted()
{
	// Server/host local notify; clients get it via OnRep_Countdown
	OnCountdownStarted.Broadcast();
	bPrevCountdownActive = IsCountdownActive();
}

float AGS_RaceState::GetElapsedRaceSeconds() const
{
	if (!bRaceRunning && !bRaceFinished) return 0.f;
	const double Now = GetServerWorldTimeSeconds();
	const double End = bRaceFinished ? RaceEndServerTime : Now;
	return End - RaceStartServerTime;
}

void AGS_RaceState::RebuildRaceResultsFromPlayerStates()
{
	RaceResults.Empty();
	FastestLapTimeSeconds = 0.f;
	FastestLapPlayerName = TEXT("");

	TArray<APS_PlayerState*> RacePlayers;

	for (APlayerState* PS : PlayerArray)
	{
		if (APS_PlayerState* RacePS = Cast<APS_PlayerState>(PS))
		{
			RacePlayers.Add(RacePS);
		}
	}

	// Sort: finished first, then by RacePosition
	RacePlayers.Sort([](const APS_PlayerState& A, const APS_PlayerState& B)
		{
			if (A.bHasFinished != B.bHasFinished)
			{
				return A.bHasFinished && !B.bHasFinished; // finished first
			}

			// Lower RacePosition = better (1st, 2nd, 3rd...)
			return A.RacePosition < B.RacePosition;
		});

	for (APS_PlayerState* PS : RacePlayers)
	{
		FRaceResultRow Row;
		Row.PlayerName = PS->GetPlayerName();
		Row.RacePosition = PS->RacePosition;
		Row.bHasFinished = PS->bHasFinished;
		Row.FinishTimeSeconds = PS->FinishTimeSeconds;
		Row.BestLapTimeSeconds = PS->BestLapTimeSeconds;

		RaceResults.Add(Row);

		// Track global fastest lap (ignore players with no laps)
		if (PS->BestLapTimeSeconds > 0.f &&
			(FastestLapTimeSeconds <= 0.f ||
				PS->BestLapTimeSeconds < FastestLapTimeSeconds))
		{
			FastestLapTimeSeconds = PS->BestLapTimeSeconds;
			FastestLapPlayerName = PS->GetPlayerName();
		}
	}
}