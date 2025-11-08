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

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BulletTime")
	float SplineSpeed = 8000.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BulletTime")
	bool bOrientToSpline = true;

	
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
	
	UPROPERTY() USplineComponent* RaceSpline = nullptr;

	
	bool  bActive = false;
	float Duration = 0.f;
	float Elapsed = 0.f;

	
	double EndTimeSeconds = 0.0;

	
	float StartDistance = 0.f;
	float CurrentDistance = 0.f;


	bool bPrevIgnoreMove = false;
	bool bPrevIgnoreLook = false;

	
	FTimerHandle BulletTimerHandle;

private:
	USplineComponent* FindRaceSpline();
	void SetInputIgnored(bool bIgnore);
	void ZeroPhysicsVelocities() const;
};
