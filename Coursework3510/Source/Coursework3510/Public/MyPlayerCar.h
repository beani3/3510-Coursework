// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"
#include "ChaosVehicleMovementComponent.h"
#include "Components/SplineComponent.h"
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



USTRUCT(BlueprintType)
struct FRaceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RacePosition = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lap = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentCheckpoint = 0;

	// 0..1 current lap progress
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LapProgress01 = 0.f;

};




UCLASS()
class COURSEWORK3510_API AMyPlayerCar : public AWheeledVehiclePawn/* public IPowerUpUserInterface, public IBPI_ScoreReceiver*/
{
	GENERATED_BODY()
public:
	AMyPlayerCar();

public:
	void BeginPlay();
	void Tick(float DeltaSeconds) override;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ResetAction;


	// --- Track/Spline refs ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race|Track")
	TObjectPtr<USplineComponent> RaceSpline = nullptr; // set automatically from BP_Racetrack actor

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race|Track")
	FName SplineComponentName = FName("Spline"); // name of spline comp inside BP_Racetrack

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race|Track")
	FName RacetrackActorTag = FName("BP_Racetrack"); // tag to find the track actor (set this tag on your BP)

	// --- Live progress (server-authored, replicated for UI) ---
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race|Progress")
	float DistanceOnSpline = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race|Progress")
	float LapProgress01 = 0.f; // 0..1 for current lap

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race|Progress")
	int32 RacePosition = 1; // 1 = leading

	// --- Respawn / validity ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race|Reset")
	float MaxOffTrackMeters = 1800.f; // exceed -> suspect cut/cheat, auto-reset

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race|Reset")
	float WrongWaySecondsToReset = 2.0f; // going opposite along spline too long -> reset

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race|Reset")
	float FlipSecondsToReset = 1.5f; // car rolled -> reset

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race|Reset")
	float RespawnLateralOffset = 40.f; // cm to the right when respawning

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Race|Reset")
	float RespawnForwardMeters = 200.f; // nudge forward on track to avoid pileups

	// cached / timers
	float WrongWayTimer = 0.f;
	float FlipTimer = 0.f;

	// API
	UFUNCTION(BlueprintCallable, Category = "Race|Track")
	void InitRacetrackSpline(); // finds BP_Racetrack and caches its Spline

	UFUNCTION(BlueprintCallable, Category = "Race|Track")
	float GetDistanceAlongTrackAt(const FVector& WorldLoc) const;

	UFUNCTION(BlueprintCallable, Category = "Race|Track")
	float GetLapProgress01At(const FVector& WorldLoc) const;

	UFUNCTION(BlueprintCallable, Category = "Race|Reset")
	void ResetToCheckpoint(); // use CurrentCheckpoint as anchor

	// Helper for UI (local or BP)
	UFUNCTION(BlueprintPure, Category = "Race|UI")
	float GetNormalizedSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Race|UI")
	FRaceData GetRaceData() const;


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
