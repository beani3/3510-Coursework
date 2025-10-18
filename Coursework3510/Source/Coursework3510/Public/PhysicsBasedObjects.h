// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaclesBase.h"
#include "PhysicsBasedObjects.generated.h"

/**
 * 
 */
UCLASS()
class COURSEWORK3510_API APhysicsBasedObjects : public AObstaclesBase
{
	GENERATED_BODY()
	
public:
	APhysicsBasedObjects();

protected:
	virtual void BeginPlay() override;
	virtual void HandleVehicleHit(AActor* OtherActor, const FHitResult& Hit, float OtherSpeed) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	float ImpulseScale;

	/** Clamp impulse magnitude (N*s). Set to <=0 to disable clamping */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	float MaxImpulse;

	/** If > 0, cap linear speed after impulse (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	float MaxLinearSpeed;

	/** If true, uses AddImpulseAtLocation; otherwise AddImpulse at COM */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	bool bUseAddImpulseAtLocation;

	/** Estimate for vehicle mass if you don’t retrieve a real one (kg) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	float AssumedVehicleMassKg;

	/** Enable Continuous Collision Detection on the obstacle mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Physics")
	bool bEnableCCD;
};
