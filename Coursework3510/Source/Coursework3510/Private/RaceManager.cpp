
#include "RaceManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
ARaceManager::ARaceManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARaceManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
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

	// If the car just completed/entered the final lap at Start/Finish, end race
	if (NewLap >= TotalLaps)
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

	// Let BP show results (pass final time)
	BP_OnRaceFinished(ElapsedTime);
}
