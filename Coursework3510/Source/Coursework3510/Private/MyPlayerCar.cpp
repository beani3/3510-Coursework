// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerCar.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"

AMyPlayerCar::AMyPlayerCar()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerCar::BeginPlay() {
    Super::BeginPlay();

    if (UChaosWheeledVehicleMovementComponent* MoveComp = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
    {
       
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
            if (DefaultMappingContext) { Subsystem->AddMappingContext(DefaultMappingContext, 0); }
        }
    }
}

void AMyPlayerCar::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    SmoothedSteeringInput = FMath::FInterpTo(SmoothedSteeringInput, TargetSteeringInput, DeltaSeconds, SteeringSmoothness);

    if (auto* MoveComp = GetVehicleMovementComponent())
    {
        MoveComp->SetSteeringInput(SmoothedSteeringInput);
    }
}

void AMyPlayerCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Move);
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPlayerCar::MoveEnd);
        }

        if (HandbrakeAction)
        {
            EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::OnHandbrakePressed);
            EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AMyPlayerCar::OnHandbrakeReleased);
        }

        if (PauseAction)
        {
            EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::OnPauseEnter);
            EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Completed, this, &AMyPlayerCar::OnPauseExit);
        }

        if (MenuAction)
        {
            EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::OnMenuEnter);
            EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Completed, this, &AMyPlayerCar::OnMenuExit);
        }
    }
}

void AMyPlayerCar::Move(const FInputActionValue& Value) {
    FVector2D MovementVector = Value.Get<FVector2D>();

    float ThrottleInput = MovementVector.Y * ThrottleSensitivity;
    TargetSteeringInput = MovementVector.X * SteeringSensitivity;

    if (auto* MoveComp = GetVehicleMovementComponent())
    {
        MoveComp->SetThrottleInput(ThrottleInput);

        if (MovementVector.Y < 0) {
            MoveComp->SetBrakeInput(-MovementVector.Y * BrakePower);
        }
        else {
            MoveComp->SetBrakeInput(IdleBrakeInput);
        }
    }
}

void AMyPlayerCar::MoveEnd() {
    if (auto* MoveComp = GetVehicleMovementComponent())
    {
        MoveComp->SetBrakeInput(0);
        MoveComp->SetThrottleInput(0);
        MoveComp->SetSteeringInput(0);
    }
    TargetSteeringInput = 0.0f;
}

void AMyPlayerCar::OnHandbrakePressed() {
    if (auto* MoveComp = GetVehicleMovementComponent())
    {
        MoveComp->SetHandbrakeInput(true);
    }
}

void AMyPlayerCar::OnHandbrakeReleased() {
    if (auto* MoveComp = GetVehicleMovementComponent())
    {
        MoveComp->SetHandbrakeInput(false);
    }
}

float AMyPlayerCar::CalcSpeed() {
    FVector CurrentVelocity = GetVelocity();
    float ForwardSpeed = FVector::DotProduct(CurrentVelocity, GetActorForwardVector());
    return ForwardSpeed;
}

void AMyPlayerCar::OnPauseEnter() {
}

void AMyPlayerCar::OnPauseExit() {
}

void AMyPlayerCar::OnMenuEnter() {
}

void AMyPlayerCar::OnMenuExit() {
}

void AMyPlayerCar::LapCheckpoint(int32 _CheckpointNumber, int32 _MaxCheckpoints, bool _bStartFinishLine)
{
    if (CurrentCheckpoint >= _MaxCheckpoints && _bStartFinishLine == true)
    {
        Lap += 1;
        CurrentCheckpoint = 1;
    }
    else if (_CheckpointNumber == CurrentCheckpoint + 1)
    {
        CurrentCheckpoint += 1;
    }
    else if (_CheckpointNumber < CurrentCheckpoint)
    {
        CurrentCheckpoint = _CheckpointNumber;
    }
    UE_LOG(LogTemp, Warning, TEXT("Lap: %i, Checkpoint: %i"), Lap, CurrentCheckpoint);
}
