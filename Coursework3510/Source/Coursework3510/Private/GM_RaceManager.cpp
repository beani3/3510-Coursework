// Fill out your copyright notice in the Description page of Project Settings.

#include "GM_RaceManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

AGM_RaceManager::AGM_RaceManager()
{
	MaxRaceTime = 0.f;
	CourtesyTime = 10.f;
	TotalLaps = 2;
	ElapsedTime = 0.f;
	bRaceRunning = false;
	bRaceFinished = false;
	TotalPlayers = 1;
	PlayersFinished = 0;
}

void AGM_RaceManager::BeginPlay()
{
	Super::BeginPlay();

	// Count active player controllers
	TotalPlayers = 0;
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			TotalPlayers++;
		}
	}
	PlayersFinished = 0;
	bRaceFinished = false;
	bRaceRunning = false;
	ElapsedTime = 0.f;
}

void AGM_RaceManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	Super::EndPlay(EndPlayReason);
}

void AGM_RaceManager::StartRace()
{
	if (bRaceRunning || bRaceFinished) return;

	ElapsedTime = 0.f;
	bRaceRunning = true;
	bRaceFinished = false;
	PlayersFinished = 0;

	// start tick timer (0.05s tick matches your existing pattern)
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Tick, this, &AGM_RaceManager::TickTimer, 0.05f, true);
		if (MaxRaceTime > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RaceDuration, this, &AGM_RaceManager::OnRaceDurationExpired, MaxRaceTime, false);
		}
	}

	BP_OnRaceStarted();
	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("GM_RaceManager: Race started"));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
	}
	OnStarted.Broadcast();
}

void AGM_RaceManager::StartRaceWithCountdown(float CountdownSeconds)
{
	if (bRaceRunning || bRaceFinished) return;
	if (CountdownSeconds <= 0.f)
	{
		StartRace();
		return;
	}
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Countdown, this, &AGM_RaceManager::OnCountdownComplete, CountdownSeconds, false);
		if (GEngine)
		{
			FString Message = FString::Printf(TEXT("GM_RaceManager: Countdown started for %f seconds"), CountdownSeconds);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
		}
	}
}

float AGM_RaceManager::GetCountdownRemaining() const
{
	if (GetWorld())
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Countdown);
	}
	return 0.f;
}

float AGM_RaceManager::GetElapsedTime() const
{
	if (GetWorld())
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_RaceDuration);
	}
	return 0.f;
}

void AGM_RaceManager::OnCountdownComplete()
{
	StartRace();
}

void AGM_RaceManager::TickTimer()
{
	if (!bRaceRunning || bRaceFinished) return;
	ElapsedTime += 0.05f;
}

void AGM_RaceManager::NotifyPlayerFinished()
{
	if (!bRaceRunning || bRaceFinished) return;

	PlayersFinished++;

	// Fire a blueprint event for UI; pass finish order and current time
	BP_OnPlayerFinished(PlayersFinished, ElapsedTime);
	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("GM_RaceManager: Player finished (%d/%d) at time %f"), PlayersFinished, TotalPlayers, ElapsedTime);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
	}

	// First finisher: start courtesy timer if configured
	if (PlayersFinished == 1 && CourtesyTime > 0.f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Courtesy, this, &AGM_RaceManager::OnCourtesyExpired, CourtesyTime, false);
		if (GEngine)
		{
			FString Message = FString::Printf(TEXT("GM_RaceManager: Courtesy time started (%f seconds)"), CourtesyTime);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
		}
	}

	// If everyone finished, end immediately
	if (PlayersFinished >= TotalPlayers)
	{
		FinishRaceInternal();
	}
}

void AGM_RaceManager::OnCourtesyExpired()
{
	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("GM_RaceManager: Courtesy expired — ending race"));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
	}
	FinishRaceInternal();
}

void AGM_RaceManager::OnRaceDurationExpired()
{
	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("GM_RaceManager: Max race time expired — ending race"));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
	}
	FinishRaceInternal();
}

void AGM_RaceManager::FinishRaceInternal()
{
	if (bRaceFinished) return;

	// stop all timers
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Tick);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RaceDuration);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Countdown);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Courtesy);
	}

	bRaceFinished = true;
	bRaceRunning = false;

	BP_OnRaceEnded();
	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("GM_RaceManager: Race ended (elapsed %f)"), ElapsedTime);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
	}
	OnFinished.Broadcast();
}

void AGM_RaceManager::EndRace()
{
	FinishRaceInternal();
}



