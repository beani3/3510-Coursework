
#include "RaceManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARaceManager::ARaceManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ARaceManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARaceManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARaceManager::EndPlay(const EEndPlayReason::Type EndPlayReason) //when the game ends
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle_Tick);
	}
	bRaceRunning = false;
	Super::EndPlay(EndPlayReason);
}

void ARaceManager::StartRace()
{
	if (bRaceRunning || bRaceFinished) return;

	bRaceRunning = true;
	ElapsedTime = 0.f;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TimerHandle_Tick, this, &ARaceManager::TickTimer, 0.05f, true);
	}
}

void ARaceManager::TickTimer()
{
	if (!bRaceRunning || bRaceFinished) return;
	ElapsedTime += 0.05f;
}


void ARaceManager::NotifyLapCompleted(int32 NewLap)
{
	if (!bRaceRunning || bRaceFinished) return;


	if (NewLap >= TotalLaps)// If the car just completed/entered the final lap at Start/Finish, end race
	{
		FinishRace();
	}
}


void ARaceManager::FinishRace()
{
	if (bRaceFinished) return;

	bRaceFinished = true;
	bRaceRunning = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle_Tick);
	}

	BP_OnRaceFinished(ElapsedTime);
}
