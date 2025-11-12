// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "AC_BulletTime.generated.h"

UCLASS(ClassGroup = (Powerups), meta = (BlueprintSpawnableComponent))
class COURSEWORK3510_API UAC_BulletTime : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_BulletTime();

	UFUNCTION(BlueprintCallable, Category = "BulletTime")
	void StartBulletTime(float DurationSeconds);

	UFUNCTION(BlueprintCallable, Category = "BulletTime")
	void StopBulletTime();

	UFUNCTION(BlueprintPure, Category = "BulletTime")
	bool IsBulletTimeActive() const { return bActive; }

	// Motion along spline
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BulletTime")
	float SplineSpeed = 8000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BulletTime")
	bool bOrientToSpline = true;

	// Client visual smoothing (does not change authority)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BulletTime|Smoothing")
	bool bClientVisualSmoothing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BulletTime|Smoothing", meta = (ClampMin = "1.0", ClampMax = "60.0"))
	float ClientSmoothStrength = 12.f;

	// Visuals
	UPROPERTY(EditAnywhere, Category = "BulletTime|Visual")
	TSubclassOf<AActor> BulletVisualClass;

	UPROPERTY(EditAnywhere, Category = "BulletTime|Visual")
	FName AttachSocketName = NAME_None;

	UPROPERTY()
	AActor* BulletVisualActor = nullptr;

	UPROPERTY()
	USkeletalMeshComponent* OwnerMesh = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// RPCs
	UFUNCTION(Server, Reliable)            void ServerStartBulletTime(float DurationSeconds);
	UFUNCTION(NetMulticast, Reliable)      void MulticastStartBulletTime(float DurationSeconds, float StartDistanceOnSpline);
	UFUNCTION(Server, Reliable)            void ServerStopBulletTime();
	UFUNCTION(NetMulticast, Reliable)      void MulticastStopBulletTime();

private:
	// Track spline and motion
	UPROPERTY() USplineComponent* RaceSpline = nullptr;

	bool  bActive = false;
	float Duration = 0.f;
	float Elapsed = 0.f;

	double EndTimeSeconds = 0.0;

	float StartDistance = 0.f;
	float CurrentDistance = 0.f;

	// Input ignore (local only)
	bool bPrevIgnoreMove = false;
	bool bPrevIgnoreLook = false;

	// Client-side visual smoothing cache (does not modify authority transform)
	FTransform SmoothedVisualTM;

	FTimerHandle BulletTimerHandle;

private:
	USplineComponent* FindRaceSpline();
	void SetInputIgnored(bool bIgnore);           // local only
	void ZeroPhysicsVelocities() const;           // server only
	void ApplyOwnerVisibility(bool bVisible);     // everywhere
	void SpawnOrDestroyVisual(bool bSpawn);       // everywhere

	// Net rate boost during bullet time (reduce jitter)
	void BoostNetRate(bool bBoost);
};
