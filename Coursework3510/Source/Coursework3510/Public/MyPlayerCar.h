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

	// Pause input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PauseAction;

	// Main Menu input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MenuAction;

	void Move(const FInputActionValue& Value);
	void MoveEnd();
	void OnHandbrakePressed();
	void OnHandbrakeReleased();
	void OnPauseEnter();
	void OnPauseExit();
	void OnMenuEnter();
	void OnMenuExit();
private:

protected:
};
