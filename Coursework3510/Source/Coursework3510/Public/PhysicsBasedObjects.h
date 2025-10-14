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
};
