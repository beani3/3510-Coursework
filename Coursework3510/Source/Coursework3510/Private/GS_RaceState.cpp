// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_RaceState.h"
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
