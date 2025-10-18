// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"
#include "MyPlayerCar.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class COURSEWORK3510_API AMyPlayerCar : public AWheeledVehiclePawn
{
    GENERATED_BODY()

public:
    AMyPlayerCar();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "UI")
    float CalcSpeed();

    void Move(const FInputActionValue& Value);
    void MoveEnd();
    void OnHandbrakePressed();
    void OnHandbrakeReleased();
    void OnPauseEnter();
    void OnPauseExit();
    void OnMenuEnter();
    void OnMenuExit();
    void LapCheckpoint(int32 _CheckpointNumber, int32 _MaxCheckpoints, bool _bStartFinishLine);

    int32 Lap = 1;
    int32 CurrentCheckpoint = 0;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Steering", meta = (ClampMin = "0.1", UIMin = "0.1"))
    float SteeringSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Steering", meta = (ClampMin = "0.1", UIMin = "0.1"))
    float SteeringSmoothness = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Drive")
    float ThrottleSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Drive", meta = (ClampMin = "0", ClampMax = "1"))
    float BrakePower = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Drive", meta = (ClampMin = "0", ClampMax = "1"))
    float IdleBrakeInput = 0.10f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Drive")
    bool bReverseAsBrake = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* HandbrakeAction = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* PauseAction = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* MenuAction = nullptr;

private:
    float TargetSteeringInput = 0.0f;
    float SmoothedSteeringInput = 0.0f;
};
