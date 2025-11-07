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
	bool IsBulletActive() const { return bActive; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	UPROPERTY() USplineComponent* RaceSpline = nullptr;

	
	bool  bActive = false;
	bool  bOrientToSpline = true;           
	float Duration = 0.f;
	float Elapsed = 0.f;

	bool bPrevIgnoreMove = false;
	bool bPrevIgnoreLook = false;

	USplineComponent* FindRaceSpline();

	
	void SetInputIgnored(bool bIgnore);
};

