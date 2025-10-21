// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"
#include "ChaosVehicleMovementComponent.h"
#include "MyPlayerCar.generated.h"

UCLASS()
class COURSEWORK3510_API AMyPlayerCar : public AWheeledVehiclePawn
{
	GENERATED_BODY()
public:
	void BeginPlay();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	// Forward movement input actions	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	// Breaking input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* HandbrakeAction;

	// Pause Menu variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenu;
	UUserWidget* PauseMenuInst;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameUI;
	UUserWidget* GameUIInst;

	bool bInPauseMenu = false;

	void Move(const FInputActionValue& Value);
	void MoveEnd();
	void OnHandbrakePressed();
	void OnHandbrakeReleased();
	void OnPauseEnter();

	int32 Lap = 1;
	int32 CurrentCheckpoint = 0;
	void LapCheckpoint(int32 _CheckpointNumber, int32 _MaxCheckpoints, bool _bStartFinishLine);

private:

protected:
};
