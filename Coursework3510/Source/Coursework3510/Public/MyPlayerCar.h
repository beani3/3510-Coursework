// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"
#include "ChaosVehicleMovementComponent.h"
//#include "PowerupUserInterface.h"
//#include "BPI_ScoreReceiver.h"
#include "MyPlayerCar.generated.h"

class UAC_HealthComponent;
class UAC_PowerupComponentC;
class UAC_ProjectileComponent;
class UChaosVehicleMovementComponent;
class UArrowComponent;
class AGM_RaceManager; // assumed GM_RaceManager
class ABP_RaceManager; // assumed BP_RaceManager
class UAC_PointsComponent;







UCLASS()
class COURSEWORK3510_API AMyPlayerCar : public AWheeledVehiclePawn/* public IPowerUpUserInterface, public IBPI_ScoreReceiver*/
{
	GENERATED_BODY()
public:
	AMyPlayerCar();

public:
	void BeginPlay();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	/*virtual void ApplyBuff_Implementation(const UBuffDef* Buff) override;

	virtual void FireProjectileFromDef_Implementation(const UProjectileDef* Def) override;

	virtual void GivePoints_Implementation(int32 Amount, FName Reason, AActor* Causer) override;*/

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SteeringAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PowerupAction;






	void Move(const FInputActionValue& Value);
	void MoveEnd();
	void Steering(const FInputActionValue& Value);
	void SteeringEnd();
	void OnHandbrakePressed();
	void OnHandbrakeReleased();
	void OnPauseEnter();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenu;
	UUserWidget* PauseMenuInst;

	bool bInPauseMenu = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	int32 Lap = 1;
	int32 CurrentCheckpoint = 0;
	void LapCheckpoint(int32 _CheckpointNumber, int32 _MaxCheckpoints, bool _bStartFinishLine);




    UFUNCTION()
    void OnRaceStarted();
    UFUNCTION()
    void OnRaceFinished();

	UFUNCTION()
	void UsePowerup();

    




	/* ===== Components ===== */
// Created in C++ so they’re always valid
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAC_HealthComponent* AC_Health = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAC_PowerupComponentC* AC_PowerupComponentC = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAC_ProjectileComponent* AC_Projectile = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAC_PointsComponent* AC_Points = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* Muzzle = nullptr;

	/* ===== Race Manager ===== */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race")
	AGM_RaceManager* GMRaceRef = nullptr;

private:

protected:
};
