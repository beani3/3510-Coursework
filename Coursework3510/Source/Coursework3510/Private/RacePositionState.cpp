// Fill out your copyright notice in the Description page of Project Settings.


#include "RacePositionState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "MyPlayerCar.h"

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
    // Gather all cars
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
            R.Lap = Car->Lap; // replicated on car
            R.Checkpoint = Car->CurrentCheckpoint; // replicated if you want, but we only need server-side
            R.DistanceOnSpline = Car->DistanceOnSpline; // server owned
            Runners.Add(R);
        }
    }

    // Sort by Lap DESC, Checkpoint DESC, Distance DESC
    Runners.Sort([](const FRunner& L, const FRunner& R)
        {
            if (L.Lap != R.Lap) return L.Lap > R.Lap;
            if (L.Checkpoint != R.Checkpoint) return L.Checkpoint > R.Checkpoint;
            return L.DistanceOnSpline > R.DistanceOnSpline;
        });

    // Assign positions
    for (int32 i = 0; i < Runners.Num(); ++i)
    {
        if (AMyPlayerCar* Car = Runners[i].Car.Get())
        {
            Car->RacePosition = i + 1; // replicated int
        }
    }
}
