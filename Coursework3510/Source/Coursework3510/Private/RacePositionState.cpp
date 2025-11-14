// Fill out your copyright notice in the Description page of Project Settings.


#include "RacePositionState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "MyPlayerCar.h"
#include "PS_PlayerState.h"

ARacePositionState::ARacePositionState()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = false;
}

void ARacePositionState::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
    {
        const float Period = (UpdateHz > 0.f) ? 1.f / UpdateHz : 0.2f;
        GetWorldTimerManager().SetTimer(TimerHandle_Update, this, &ARacePositionState::RecomputePositions, Period, true);
    }
}

void ARacePositionState::RecomputePositions()
{
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(this, AMyPlayerCar::StaticClass(), Found);

    TArray<FRunner> Runners;
    Runners.Reserve(Found.Num());
    for (AActor* A : Found)
    {
        if (AMyPlayerCar* Car = Cast<AMyPlayerCar>(A))
        {
            FRunner R;
            R.Car = Car;
            R.Lap = Car->Lap;
            R.Checkpoint = Car->CurrentCheckpoint;
            R.DistanceOnSpline = Car->DistanceOnSpline;
            Runners.Add(R);
        }
    }

    Runners.Sort([](const FRunner& L, const FRunner& R)
        {
            if (L.Lap != R.Lap)         return L.Lap > R.Lap;
            if (L.Checkpoint != R.Checkpoint) return L.Checkpoint > R.Checkpoint;
            return L.DistanceOnSpline > R.DistanceOnSpline;
        });

    for (int32 i = 0; i < Runners.Num(); ++i)
    {
        if (AMyPlayerCar* Car = Runners[i].Car.Get())
        {
            const int32 Pos = i + 1;
            Car->RacePosition = Pos;
            if (AController* Controller = Cast<AController>(Car->GetController()))
            {
                if (APS_PlayerState* PS = Controller->GetPlayerState<APS_PlayerState>())
                {
                    PS->RacePosition = Pos;
                }
            }
        }
    }
}