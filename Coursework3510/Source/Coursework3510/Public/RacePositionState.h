// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RacePositionState.generated.h"

class AMyPlayerCar;

USTRUCT()
struct FRunner
{
    GENERATED_BODY()
    UPROPERTY() TWeakObjectPtr<AMyPlayerCar> Car;
    UPROPERTY() int32 Lap = 1;
    UPROPERTY() int32 Checkpoint = 0;
    UPROPERTY() float DistanceOnSpline = 0.f;
};

UCLASS()
class COURSEWORK3510_API ARacePositionState : public AActor
{
    GENERATED_BODY()

public:
    ARacePositionState();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race")
    float UpdateHz = 5.f; // 5x/sec

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void RecomputePositions();

    FTimerHandle TimerHandle_Update;
};
